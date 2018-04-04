/*
* Copyright (c) 2013 - 2017, Roland Bock, Frank Park, Aaron Bishop
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#ifndef SQLPP_THREAD_POOL_H
#define SQLPP_THREAD_POOL_H

#include <vector>
#include <list>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <condition_variable>
#include <cassert>
#include <sqlpp11/type_traits.h>

//Auto growing/shrinking thread_pool
//Threads finish if they haven't run for shrink_timeout
//Finished threads are destructed on enqueue
namespace sqlpp
{
  namespace detail
  {
    class ptr_holder
    {
      void *data;
      std::function<void(void*)> deleter;
    public:
      ptr_holder(void* d, std::function<void(void*)> f)
        : data(d), deleter(f) {}

      ptr_holder(const ptr_holder&) = delete;
      ptr_holder& operator=(const ptr_holder&) = delete;

      template<class T>
      T get() { return reinterpret_cast<T>(data); }

      void reset()
      {
        if(data)
        {
          deleter(data);
          data = nullptr;
        }
      }

      ~ptr_holder() { reset(); }
      
      ptr_holder(ptr_holder&& h)
        : data(std::move(h.data)), deleter(std::move(h.deleter))
      {
        h.data = nullptr;
      }
      ptr_holder& operator=(ptr_holder&& h)
      {
        reset();
        data = std::move(h.data);
        deleter = std::move(h.deleter);
        h.data = nullptr;
        return *this;
      }
    };
  }

  class thread_pool
  {
  public:
    thread_pool(const std::chrono::seconds& timeout=std::chrono::seconds(5));

    template<class F, class... Args>
    auto enqueue_future(F&& f, Args&&... args)
      -> std::future<invoke_result_t<F, Args...> >;

    template<class Cb, class F, class... Args>
    void enqueue_callback(Cb&& cb, F&& f, Args&&... args);

    void enqueue(std::function<void()> task);

    void stop();

    std::chrono::seconds get_timeout() const { return recycle_timeout; }

    void set_timeout(const std::chrono::seconds& t)
    {
      std::lock_guard<std::mutex> lock(task_mutex);
      recycle_timeout = t;
    }

    ~thread_pool() { stop(); }
  private:
    std::chrono::seconds               recycle_timeout;
    std::vector<std::thread>           workers;
    std::vector<std::thread::id>       finished_threads;
    std::queue<std::function<void()> > tasks;
    size_t                             waiting_threads;
    std::mutex                         task_mutex;
    std::mutex                         holder_mutex;
    std::condition_variable            task_condition;
    std::condition_variable            recycle_condition;
    std::atomic_bool                   needs_stop;
    std::thread                        recycler;
    std::list<detail::ptr_holder>      pointer_holders;

    template<class Cb, class Arg, typename std::enable_if<is_invocable<Cb,Arg>::valu,int>::type = 0>
    static void async_callback(Cb&& cb, Arg&& arg) { cb(std::move(arg)); }

    template<class Cb, class Arg, typename std::enable_if<is_invocable<Cb>::valu,int>::type = 0>
    static void async_callback(Cb&& cb, Arg&& /*arg*/) { cb(); }
  };

  namespace detail
  {
    static sqlpp::thread_pool thread_pool;
  }

  template<class Function, class... Args>
  std::future<invoke_result_t<Function,Args...> >
  async(Function&& f, Args&&... args)
  {
    return std::move(detail::thread_pool.enqueue_future(std::forward<Function>(f), std::forward<Args>(args)...));
  }

  template<class Callback, class Function, class... Args>
  void async_callback(Callback&& callback, Function&& f, Args&&... args)
  {
    detail::thread_pool.enqueue_callback(std::forward<Callback>(callback), std::forward<Function>(f), std::forward<Args>(args)...);
  }

  inline thread_pool::thread_pool(const std::chrono::seconds& timeout)
    : recycle_timeout(timeout)
    , workers()
    , tasks()
    , waiting_threads(0)
    , task_mutex()
    , task_condition()
    , recycle_condition()
    , needs_stop(false)
    , recycler([this](){
      std::mutex recycle_mutex;
      for(;;)
      {
        std::unique_lock<std::mutex> lock(recycle_mutex);
        recycle_condition.wait(lock);
        if(needs_stop)
        {
          for(auto& thread : workers)
          {
            if(thread.joinable())
              thread.join();
          }
          workers.clear();
          finished_threads.clear();
          return;
        }
        for(const auto& id : finished_threads)
        {
          for(auto itr = workers.begin(); itr != workers.end(); ++itr) {
            if(id == itr->get_id())
            {
              if(itr->joinable())itr->join();
              std::swap(*itr, *(workers.end()-1));
              auto index = std::distance(workers.begin(), itr);
              workers.erase(workers.end()-1);
              itr = workers.begin() + index;
            }
            else
            {
              ++itr;
            }
          }
        }
        finished_threads.clear();
      }
    })
  {}

  
  // add new work item to the pool
  template<class F, class... Args>
  auto thread_pool::enqueue_future(F&& f, Args&&... args)
    -> std::future<invoke_result_t<F, Args...> >
  {
    using return_type = invoke_result_t<F, Args...>;
    using task_type = std::packaged_task<return_type()>;
    using task_ptr = task_type*;

    std::unique_lock<std::mutex> lock(holder_mutex);
    auto task = new task_type(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    pointer_holders.emplace_front(task, [](void* p) { delete reinterpret_cast<task_ptr>(p); });
    auto ptr = pointer_holders.begin();
    lock.unlock();
    enqueue([this,ptr](){
      task_ptr task = ptr->get<task_ptr>();
      assert(task);
      (*task)();
      std::lock_guard<std::mutex> lock(holder_mutex);
      pointer_holders.erase(ptr);
    });
    return task->get_future();
  }

  template<class Cb, class F, class... Args>
  void thread_pool::enqueue_callback(Cb&& cb, F&& f, Args&&... args)
  {
    using return_type = invoke_result_t<F, Args...>;
    static_assert(
      is_invocable<Cb, std::future<return_type> >::value ||
      is_invocable<Cb>::value
      , "Invalid Callback");

    using task_type = std::pair<Cb, std::packaged_task<return_type()> >;
    using task_ptr = task_type*;

    std::unique_lock<std::mutex> lock(holder_mutex);
    auto task = new task_type(std::move(cb), std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    pointer_holders.emplace_front(task, [](void* p) { delete reinterpret_cast<task_ptr>(p); });
    auto ptr = pointer_holders.begin();
    lock.unlock();
    enqueue([this,ptr](){
      task_ptr task = ptr->get<task_ptr>();
      assert(task);
      auto fut = task->second.get_future();
      auto cb = std::move(task->first);
      task->second();
      {
        std::lock_guard<std::mutex> lock(holder_mutex);
        pointer_holders.erase(ptr);
      }
      task = nullptr;
      async_callback(std::move(cb), std::move(fut));
    });
  }

  // add pre-allocated task item to the pool
  void thread_pool::enqueue(std::function<void()> task)
  {
    std::unique_lock<std::mutex> lock(task_mutex);
    if (needs_stop)
    {
      throw std::runtime_error("cannot enqueue on a stopped thread_pool");
    }
    tasks.push(std::move(task));
    if(waiting_threads < 1)
    {
      workers.emplace_back([this](){
        std::unique_lock<std::mutex> lock(task_mutex);
        for(;;) {
          ++waiting_threads;
          const auto&& empty = !task_condition.wait_for(lock, recycle_timeout, [this](){ return !tasks.empty(); });
          assert(lock.owns_lock());
          --waiting_threads;
          if(empty)
          {
            finished_threads.push_back(std::this_thread::get_id());
            return;
          }
          assert(!tasks.empty());
          auto&& task = std::move(tasks.front());
          tasks.pop();
          lock.unlock();
          try{ task(); }catch(...) {}
          lock.lock();
        }
      });
    }
    task_condition.notify_one();
  }

  void thread_pool::stop()
  {
      needs_stop = true;
      std::unique_lock<std::mutex> lock(task_mutex);
      while(!tasks.empty())
        tasks.pop();
      lock.unlock();
      task_condition.notify_all();
      recycle_condition.notify_all();
      if(recycler.joinable())
        recycler.join();
  }
}

#endif // SQLPP_THREAD_POOL_H

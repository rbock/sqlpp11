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

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <atomic>

namespace sqlpp
{
  class thread_pool
  {
  public:
    thread_pool();
    thread_pool(size_t);
    ~thread_pool();

    void spawn_thread(size_t count);

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
      ->std::future<typename std::result_of<F(Args...)>::type>;
  private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::atomic<size_t> free_worker_count;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
  };

  namespace impl
  {
    static sqlpp::thread_pool thread_pool;
  }

  inline thread_pool::thread_pool() : stop(false), free_worker_count(0) {}
  inline thread_pool::thread_pool(size_t thread_count) : stop(false), free_worker_count(0)
  {
    spawn_thread(thread_count);
  }

  // the destructor joins all threads
  inline thread_pool::~thread_pool()
  {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
    {
      worker.join();
    }
  }

  void thread_pool::spawn_thread(size_t thread_count = 1)
  {
    for (size_t i = 0; i < thread_count; ++i)
    {
      workers.emplace_back([this]
      {
        for (;;)
        {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->condition.wait(lock, [this]
            {
              return this->stop || !this->tasks.empty();
            });

            if (this->stop && this->tasks.empty())
            {
              return;
            }

            task = std::move(this->tasks.front());
            this->tasks.pop();
          }

          free_worker_count--;
          task();
          free_worker_count++;
        }
      });
      free_worker_count++;
    }
  }

  // add new work item to the pool
  template<class F, class... Args>
  auto thread_pool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
  {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    auto result = std::future<return_type>(task->get_future());

    if (free_worker_count.load() == 0)
    {
      spawn_thread();
    }

    {
      std::unique_lock<std::mutex> lock(queue_mutex);

      if (stop)
      {
        throw std::runtime_error("cannot enqueue on a stopped thread_pool");
      }

      tasks.emplace([task]() { (*task)(); });
    }

    condition.notify_one();
    return result;
  }
}

#endif

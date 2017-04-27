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

#ifndef SQLPP_FUTURE_H
#define SQLPP_FUTURE_H

#include <exception>
#include <future>

#include <sqlpp11/connection_pool.h>

namespace sqlpp
{
  template<typename Connection_pool, typename Query, typename Callback>
  struct async_query_task;

  template<typename Pool_connection, typename Result, typename Connection_pool, typename Query, typename Callback>
  struct async_query_future
  {
  private:
    using Task = sqlpp::async_query_task<Connection_pool, Query, Callback>;

    template <typename Connection_pool, typename Query, typename Callback>
    friend struct enqueue_on_suspend;

    std::shared_ptr<Task> async_query_task;

    Pool_connection& connection;
    Result& result;
    std::future<void>& future;

    template<typename T>
    bool is_ready(std::future<T> const& f)
    {
      return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

  public:
    async_query_future(std::shared_ptr<Task>&& task)
      : async_query_task(std::move(task)),
      connection(async_query_task->connection),
      result(async_query_task->result),
      future(async_query_task->future)
    {
    }

    async_query_future(async_query_future&& other)
      : async_query_task(std::move(other.async_query_task)),
      connection(std::move(other.connection)),
      result(std::move(other.result)),
      future(std::move(other.future))
      {
    }

    ~async_query_future()
    {
    }

    auto& get_connection()
    {
      if (!future.valid())
      {
        throw sqlpp::exception("Invalid connection_future or result has already been retrieved.");
      }

      if (is_ready(future))
      {
        return connection;
      }
      else
      {
        throw sqlpp::exception("Calling get_connection() before executing query.");
      }
    }

    auto& get_result()
    {
      if (!future.valid())
      {
        throw sqlpp::exception("Invalid result_future or result has already been retrieved.");
      }

      if (is_ready(future))
      {
        return result;
      }
      else
      {
        throw sqlpp::exception("Calling get_result() before executing query.");
      }
    }
  };

  template<typename Connection_pool, typename Query, typename Callback>
  struct async_query_task : std::enable_shared_from_this<async_query_task<Connection_pool, Query, Callback>>
  {
  private:
    using Pool_connection = decltype(std::declval<Connection_pool>().get_connection());
    using Result = std::result_of_t<Pool_connection(Query)>;
    using Future = sqlpp::async_query_future<Pool_connection, Result, Connection_pool, Query, Callback>;

    friend Future;

    Connection_pool& pool;
    Query query;
    Callback callback;

    Pool_connection connection;
    Result result;

    std::promise<void> promise;
    std::future<void> future;

    template<typename Callback, typename Future,
      typename std::enable_if<!is_invocable<Callback>::value &&
      !is_invocable<Callback, Future>::value, int>::type = 0>
      void invoke_callback(Callback& cb, Future&& f)
    {
      static_assert(false, "Callback signature is incompatible. Refer to the wiki for further instructions.");
    }

    template<typename Callback, typename Future,
      typename std::enable_if<is_invocable<Callback>::value, int>::type = 0>
      void invoke_callback(Callback& cb, Future&& f)
    {
      callback();
    }

    template<typename Callback, typename Future,
      typename std::enable_if<is_invocable<Callback, Future>::value, int>::type = 0>
      void invoke_callback(Callback& cb, Future&& f)
    {
      callback(std::move(f));
    }
  public:
    async_query_task(Connection_pool& pool, Query&& query, Callback&& callback)
      : pool(pool), query(query), callback(callback), future(promise.get_future())
    {
    }

    ~async_query_task()
    {
    }

    auto get_future()
    {
      return Future(shared_from_this());
    }

    auto operator()()
    {
      try
      {
        connection = pool.get_connection();
        result = connection(query);
        promise.set_value();
        invoke_callback(callback, get_future());
      }
      catch (sqlpp::exception e)
      {
        promise.set_exception(std::current_exception());
      }
    }
  };

  template<typename Connection_pool, typename Query>
  auto async(Connection_pool& pool, Query&& query)
  {
    auto lambda = [] {};
    using Task = sqlpp::async_query_task<Connection_pool, Query, decltype(lambda)>;
    auto async_query_task = std::make_shared<Task>(pool, query, std::move(lambda));

    sqlpp::impl::thread_pool.emplace(async_query_task);

    return async_query_task->get_future();
  }

  template<typename Connection_pool, typename Query, typename Callback>
  auto async(Connection_pool& pool, Query&& query, Callback&& callback)
  {
    using Task = sqlpp::async_query_task<Connection_pool, Query, Callback>;
    auto async_query_task = std::make_shared<Task>(pool, query, callback);

    sqlpp::impl::thread_pool.emplace(async_query_task);
    
    return async_query_task->get_future();
  }

  template<typename Connection_pool, typename Query>
  auto deferred(Connection_pool& pool, Query&& query)
  {
    auto lambda = [] {};
    using Task = sqlpp::async_query_task<Connection_pool, Query, decltype(lambda)>;
    auto async_query_task = std::make_shared<Task>(pool, query, std::move(lambda));
    return async_query_task->get_future();
  }

  template<typename Connection_pool, typename Query, typename Callback>
  auto deferred(Connection_pool& pool, Query&& query, Callback&& callback)
  {
    using Task = sqlpp::async_query_task<Connection_pool, Query, Callback>;
    auto async_query_task = std::make_shared<Task>(pool, query, callback);
    return async_query_task->get_future();
  }
}

#endif

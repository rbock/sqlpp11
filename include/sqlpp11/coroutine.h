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

#ifndef SQLPP_COROUTINE_H
#define SQLPP_COROUTINE_H

#include <future>
#include <sqlpp11/thread_pool.h>
#include <sqlpp11/resumable_function.h>
#include <sqlpp11/future.h>

namespace sqlpp
{
  template <typename Connection_pool, typename Query, typename Callback>
  struct enqueue_on_suspend
  {
    using Pool_connection = decltype(std::declval<Connection_pool>().get_connection());
    using Result = std::result_of_t<Pool_connection(Query)>;
    using Future = sqlpp::async_query_future<Pool_connection, Result, Connection_pool, Query, Callback>;

    Future& _async_query;

    enqueue_on_suspend(Future&& async_query_future)
      : _async_query(async_query_future)
    {
    }
    bool await_ready() _NOEXCEPT
    {
      return false;
    }

    void await_suspend(std::experimental::coroutine_handle<> coroutine) _NOEXCEPT
    {
      sqlpp::impl::thread_pool.enqueue([&, coroutine]
      {
        _async_query.async_query_task->operator()();
        coroutine();
      });
      return;
    }

    void await_resume() _NOEXCEPT
    {
      return;
    }
  };

  template <typename Pool_connection, typename Result, typename Pool, typename Query, typename Callback>
  inline auto operator co_await(sqlpp::async_query_future<Pool_connection, Result, Pool, Query, Callback>& async_query_future)
  {
    return sqlpp::enqueue_on_suspend<Pool, Query, Callback>(std::move(async_query_future));
  }
}

#endif
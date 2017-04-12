/*
* Copyright (c) 2013 - 2017, Roland Bock, Frank Park
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

#ifndef SQLPP_BIND_H
#define SQLPP_BIND_H

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <utility>
#include <type_traits>
#include <system_error>

#include <sqlpp11/type_traits.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/connection_pool.h>

namespace sqlpp
{

  template<typename Pool_connection, typename Result, typename Lambda,
    typename std::enable_if<!is_invocable<Lambda>::value &&
    !is_invocable<Lambda, sqlpp::exception>::value &&
    !is_invocable<Lambda, sqlpp::exception, Result>::value &&
    !is_invocable<Lambda, sqlpp::exception, Result, Pool_connection>::value, int>::type = 0>
    void invoke_callback(sqlpp::exception exception, Pool_connection connection, Result result, Lambda& callback)
  {
    static_assert(false, "Callback signature is incompatible. Refer to the wiki for further instructions.");
  }

  template<typename Pool_connection, typename Result, typename Lambda,
    typename std::enable_if<is_invocable<Lambda>::value, int>::type = 0>
  void invoke_callback(sqlpp::exception exception, Pool_connection connection, Result result, Lambda& callback)
  {
    callback();
  }

  template<typename Pool_connection, typename Result, typename Lambda,
    typename std::enable_if<is_invocable<Lambda, sqlpp::exception>::value, int>::type = 0>
  void invoke_callback(sqlpp::exception exception, Pool_connection connection, Result result, Lambda& callback)
  {
    callback(std::move(exception));
  }

  template<typename Pool_connection, typename Result, typename Lambda,
    typename std::enable_if<is_invocable<Lambda, sqlpp::exception, Result>::value, int>::type = 0>
  void invoke_callback(sqlpp::exception exception, Pool_connection connection, Result result, Lambda& callback)
  {
    callback(std::move(exception), std::move(result));
  }

  template<typename Pool_connection, typename Result, typename Lambda,
    typename std::enable_if<is_invocable<Lambda, sqlpp::exception, Result, Pool_connection>::value, int>::type = 0>
  void invoke_callback(sqlpp::exception exception, Pool_connection connection, Result result, Lambda& callback)
  {
    callback(std::move(exception), std::move(result), std::move(connection));
  }

  template<typename Database, typename Query, typename Lambda,
    typename std::enable_if<std::is_base_of<connection, Database>::value, int>::type = 0>
  class bind_t_move
  {
  private:
    Database db;
    Query query;
    Lambda callback;

  public:
    bind_t_move(Database& d, Query& q, Lambda& cb) : db(std::move(d)), query(q), callback(cb)
    {
    }

    template<typename Database = Database,
      typename std::enable_if<std::is_base_of<connection, Database>::value, int>::type = 0>
    void operator()()
    {
      using Pool_connection = Database;
      using Result = decltype((*((Pool_connection*)0))(Query()));

      try
      {
        invoke_callback(sqlpp::exception(sqlpp::exception::ok), std::move(db), std::move(db(query)), callback);
      }
      catch (const std::exception& e)
      {
        invoke_callback(sqlpp::exception(sqlpp::exception::query_error, e.what()), std::move(db), Result(), callback);
      }
    }
  };

  template<typename Database, typename Query, typename Lambda,
    typename std::enable_if<!std::is_base_of<connection, Database>::value, int>::type = 0>
  class bind_t_ref
  {
  private:
    Database& db;
    Query query;
    Lambda callback;

  public:
    bind_t_ref(Database& d, Query& q, Lambda& cb) : db(d), query(q), callback(cb)
    {
    }

    template<typename Database = Database, typename std::enable_if<!std::is_base_of<connection, Database>::value, int>::type = 0>
    void operator()()
    {
      using Pool_connection = decltype((*((Database*)0)).get_connection());
      using Result = decltype((*((Pool_connection*)0))(Query()));
      Pool_connection connection;
      try
      {
        connection = std::move(db.get_connection());
      }
      catch (const sqlpp::exception& e)
      {
        invoke_callback(sqlpp::exception(sqlpp::exception::connection_error, e.what()), std::move(connection), Result(), callback);
      }

      try
      {
        invoke_callback(sqlpp::exception(sqlpp::exception::ok), std::move(connection), std::move(connection(query)), callback);
      }
      catch (const std::exception& e)
      {
        invoke_callback(sqlpp::exception(sqlpp::exception::query_error, e.what()), std::move(connection), Result(), callback);
      }
    }


  };

  template<typename Database, typename Query, typename Lambda,
    typename std::enable_if<std::is_base_of<connection, Database>::value, int>::type = 0>
    bind_t_move<Database, Query, Lambda> bind(Database db, Query& query, Lambda& lambda)
  {
    return sqlpp::bind_t_move<Database, Query, Lambda>(std::move(db), query, lambda);
  }

  template<typename Database, typename Query, typename Lambda,
    typename std::enable_if<!std::is_base_of<connection, Database>::value, int>::type = 0>
    bind_t_ref<Database, Query, Lambda> bind(Database& db, Query& query, Lambda& lambda)
  {
    return sqlpp::bind_t_ref<Database, Query, Lambda>(db, query, lambda);
  }

  template<typename Connection_pool, typename Query, typename Lambda>
  void async(Connection_pool& pool, Query& query, Lambda& callback)
  {
    std::async(std::launch::async, std::ref(pool), query, std::ref(callback));
  }

  template<typename Bind, typename std::enable_if<std::is_copy_constructible<Bind>::value, int>::type = 0>
  void async(Bind& bind)
  {
    std::async(std::launch::async, std::move(bind));
  }

  template<typename Bind, typename std::enable_if<!std::is_copy_constructible<Bind>::value, int>::type = 0>
  void async(Bind& bind)
  {
    static_assert(false, "Only copy constructible bindings can be executed asynchronously.");
  }
}

#endif

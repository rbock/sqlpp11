#pragma once

/*
 * Copyright (c) 2013 - 2016, Roland Bock
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

#include <string_view>
#include <span>

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/database/connection.h>
#include <sqlpp11/core/chrono.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/database/exception.h>
#include <sqlpp11/core/basic/parameter.h>
#include <sqlpp11/core/basic/join.h>
#include <sqlpp11/core/clause/using.h>
#include <sqlpp11/core/clause/with.h>

#include <cmath>

namespace sqlpp
{
  // Disable some stuff that won't work with sqlite3
  // See https://www.sqlite.org/changes.html
#if SQLITE_VERSION_NUMBER < 3039000
  template <typename Lhs, typename Rhs, typename Condition>
  auto to_sql_string(sqlite3::context_t&, const join_t<Lhs, full_outer_join_t, Rhs, Condition>&) -> std::string
  {
    SQLPP_STATIC_ASSERT((wrong_t<Lhs, Rhs>::value), "Sqlite3: No support for full outer join before version 3.39.0");
    return {};
  }

  template <typename Lhs, typename Rhs, typename Condition>
  auto to_sql_string(sqlite3::context_t&, const join_t<Lhs, right_outer_join_t, Rhs, Condition>&) -> std::string
  {
    SQLPP_STATIC_ASSERT((wrong_t<Lhs, Rhs>::value), "Sqlite3: No support for right outer join before version 3.39.0");
    return {};
  }
#endif

#if SQLITE_VERSION_NUMBER < 3008003
  template <typename... Ctes>
  auto to_sql_string(sqlite3::context_t& , const with_t<Ctes...>&)-> std::string
  {
    SQLPP_STATIC_ASSERT(wrong_t<Ctes...>::value, "Sqlite3: No support for WITH before version 3.8.3");
    return {};
  }
#endif

  template <typename Select>
  auto to_sql_string(sqlite3::context_t&, const any_t<Select>&) -> std::string
  {
    SQLPP_STATIC_ASSERT(wrong_t<Select>::value, "Sqlite3: No support for any()");
    return {};
  }

  template <typename _Table>
  auto to_sql_string(sqlite3::context_t& , const using_t<_Table>&) -> std::string
  {
    SQLPP_STATIC_ASSERT(wrong_t<_Table>::value, "Sqlite3: No support for USING");
    return {};
  }

  template <typename Lhs, typename Rhs>
  auto to_sql_string(sqlite3::context_t& context, const union_t<union_distinct_t, Lhs, Rhs>& t) -> std::string
  {
    // Note: Temporary required to enforce parameter ordering.
    auto ret_val = to_sql_string(context, t._lhs) + " UNION ";
    return ret_val += to_sql_string(context, t._rhs);
  }

  // Serialize parameters
  template <typename ValueType, typename NameType>
  auto to_sql_string(sqlite3::context_t& context, const parameter_t<ValueType, NameType>&) -> std::string
  {
    return "?" + std::to_string(++context._count);
  }

  namespace sqlite3
  {
    // Some special treatment of data types
    template <typename Period>
    auto to_sql_string(sqlite3::context_t&, const std::chrono::time_point<std::chrono::system_clock, Period>& t)
        -> std::string
    {
      return std::format("DATETIME('{0:%Y-%m-%d %H:%M:%S}', 'subsec')", t);
    }

    inline auto to_sql_string(sqlite3::context_t&, const std::chrono::microseconds& t) -> std::string
    {
      return std::format("TIME('{0:%H:%M:%S}', 'subsec')", t);
    }

    inline auto to_sql_string(sqlite3::context_t&, const sqlpp::chrono::day_point& t) -> std::string
    {
      return std::format("DATE('{0:%Y-%m-%d}')", t);
    }

    inline auto nan_to_sql_string(sqlite3::context_t&) -> std::string
    {
      return "'NaN'";
    }

    inline auto inf_to_sql_string(sqlite3::context_t&) -> std::string
    {
      return "'Inf'";
    }

    inline auto neg_inf_to_sql_string(sqlite3::context_t&) -> std::string
    {
      return "'-Inf'";
    }
  }  // namespace sqlite3

}  // namespace sqlpp

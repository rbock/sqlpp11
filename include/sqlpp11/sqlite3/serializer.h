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

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <sqlpp11/any.h>
#include <sqlpp11/data_types/day_point/operand.h>
#include <sqlpp11/data_types/floating_point/operand.h>
#include <sqlpp11/data_types/integral/operand.h>
#include <sqlpp11/data_types/time_point/operand.h>
#include <sqlpp11/data_types/unsigned_integral/operand.h>
#include <sqlpp11/parameter.h>
#include <sqlpp11/pre_join.h>
#include <sqlpp11/some.h>
#include <sqlpp11/with.h>

#include <cmath>

namespace sqlpp
{
  // Serialize parameters
  template <typename ValueType, typename NameType>
  sqlite3::context_t& serialize(const parameter_t<ValueType, NameType>&, sqlite3::context_t& context)
  {
    context << "?" << context.count();
    context.pop_count();
    return context;
  }

  // disable some stuff that won't work with sqlite3
#if SQLITE_VERSION_NUMBER < 3008003
  template <typename Database, typename... Expressions>
  sqlite3::context_t& serialize(const with_data_t<Database, Expressions...>&, sqlite3::context_t& context)
  {
    static_assert(wrong_t<Expressions...>::value, "Sqlite3: No support for with before version 3.8.3");
    return context;
  }
#endif

  template <typename Select>
  sqlite3::context_t& serialize(const any_t<Select>&, sqlite3::context_t& context)
  {
    static_assert(wrong_t<Select>::value, "Sqlite3: No support for any()");
    return context;
  }

  template <typename Select>
  sqlite3::context_t& serialize(const some_t<Select>&, sqlite3::context_t& context)
  {
    static_assert(wrong_t<Select>::value, "Sqlite3: No support for some()");
    return context;
  }

  template <typename Lhs, typename Rhs>
  sqlite3::context_t& serialize(const pre_join_t<outer_join_t, Lhs, Rhs>&, sqlite3::context_t& context)
  {
    static_assert(wrong_t<Lhs, Rhs>::value, "Sqlite3: No support for outer join");
    return context;
  }

  template <typename Lhs, typename Rhs>
  sqlite3::context_t& serialize(const pre_join_t<right_outer_join_t, Lhs, Rhs>&, sqlite3::context_t& context)
  {
    static_assert(wrong_t<Lhs, Rhs>::value, "Sqlite3: No support for right_outer join");
    return context;
  }

  // Some special treatment of data types
  template <typename Period>
  sqlite3::context_t& serialize(const time_point_operand<Period>& t, sqlite3::context_t& context)
  {
    const auto dp = ::sqlpp::chrono::floor<::date::days>(t._t);
    const auto time = ::date::make_time(t._t - dp);
    const auto ymd = ::date::year_month_day{dp};
    context << "STRFTIME('%Y-%m-%d %H:%M:%f', '" << ymd << ' ' << time << "')";
    return context;
  }

  inline sqlite3::context_t& serialize(const day_point_operand& t, sqlite3::context_t& context)
  {
    const auto ymd = ::date::year_month_day{t._t};
    context << "DATE('" << ymd << "')";
    return context;
  }

  inline sqlite3::context_t& serialize(const floating_point_operand& t, sqlite3::context_t& context)
  {
    if (std::isnan(t._t))
      context << "'NaN'";
    else if (std::isinf(t._t))
    {
      if (t._t > std::numeric_limits<double>::max())
        context << "'Inf'";
      else
        context << "'-Inf'";
    }
    else
      context << t._t;
    return context;
  }

  // sqlite3 accepts only signed integers,
  // so we MUST perform a conversion from unsigned to signed
  inline sqlite3::context_t& serialize(const unsigned_integral_operand& t, sqlite3::context_t& context)
  {
    context << static_cast<typename integral_operand::_value_t>(t._t);
    return context;
  }
}  // namespace sqlpp

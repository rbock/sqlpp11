#pragma once

/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <sqlpp11/sqlpp11.h>

template <typename Clock, typename Period>
std::ostream& operator<<(std::ostream& os, const std::chrono::time_point<Clock, Period>& t)
{
  const auto dp = ::sqlpp::chrono::floor<::date::days>(t);
  const auto ymd = ::date::year_month_day{dp};
  const auto time = ::date::make_time(t - dp);
  os << "TIMESTAMP '" << ymd << ' ' << time << "'";
  return os;
}

template <typename Rep, typename Period>
std::ostream& operator<<(std::ostream& os, const std::chrono::duration<Rep, Period>& t)
{
  return os << '\'' << ::date::make_time(t) << '\'';
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const sqlpp::optional<T>& t)
{
  if (not t)
    return os << "NULL";
  return os << t.value();
}

inline std::ostream& operator<<(std::ostream& stream, const sqlpp::isolation_level& level)
{
  switch (level)
  {
    case sqlpp::isolation_level::serializable:
    {
      stream << "SERIALIZABLE";
      break;
    }
    case sqlpp::isolation_level::repeatable_read:
    {
      stream << "REPEATABLE READ";
      break;
    }
    case sqlpp::isolation_level::read_committed:
    {
      stream << "READ COMMITTED";
      break;
    }
    case sqlpp::isolation_level::read_uncommitted:
    {
      stream << "READ UNCOMMITTED";
      break;
    }
    case sqlpp::isolation_level::undefined:
    {
      stream << "BEGIN";
      break;
    }
  }

  return stream;
}

template <typename L, typename R>
auto require_equal(int line, const L& l, const R& r) -> void
{
  if (l != r)
  {
    std::cerr << line << ": " << l << " != " << r << std::endl;
    throw std::runtime_error("Unexpected result");
  }
}

template <typename T>
struct is_optional : public std::false_type{};

template <typename T>
struct is_optional<sqlpp::optional<T>> : public std::true_type{};


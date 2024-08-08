#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename Context, typename X = void>
  auto serialize(Context& context, ...) -> Context&
  {
    static_assert(wrong_t<X>::value, "Missing specialization");
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const bool& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const int8_t& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const int16_t& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const int32_t& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const int64_t& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const uint8_t& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const uint16_t& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const uint32_t& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const uint64_t& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const float& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const double& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const long double& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const char& t) -> Context&
  {
    context << t;
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const char* t) -> Context&
  {
    context << '\'' << context.escape(std::string(t)) << '\'';
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const ::sqlpp::string_view& t) -> Context&
  {
    context << '\'' << context.escape(std::string(t)) << '\'';
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const std::string& t) -> Context&
  {
    context << '\'' << context.escape(t) << '\'';
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const ::sqlpp::span<uint8_t>& t) -> Context&
  {
    constexpr char hexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    context << "x'";
    for (const auto c : t)
    {
      context << hexChars[c >> 4] << hexChars[c & 0x0F];
    }
    context << '\'';

    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const std::vector<uint8_t>& t) -> Context&
  {
    return serialize(context, ::sqlpp::span<uint8_t>(t.data(), t.size()));
  }

  template <typename Context, std::size_t N>
  auto serialize(Context& context, const std::array<uint8_t, N>& t) -> Context&
  {
    return serialize(context, ::sqlpp::span<uint8_t>(t.data(), t.size()));
  }

  template <typename Context>
  auto serialize(Context& context, const ::sqlpp::chrono::day_point& t) -> Context&
  {
    const auto ymd = ::date::year_month_day{t};
    context << "DATE '" << ymd << "'";
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const std::chrono::microseconds& t) -> Context&
  {
    context << '\'' << ::date::make_time(t) << '\'';
    return context;
  }

  template <typename Period, typename Context>
  auto serialize(Context& context, const std::chrono::time_point<std::chrono::system_clock, Period>& t) -> Context&
  {
    const auto dp = ::sqlpp::chrono::floor<::date::days>(t);
    const auto time = ::date::make_time(t - dp);
    const auto ymd = ::date::year_month_day{dp};
    context << "TIMESTAMP '" << ymd << ' ' << time << "'";
    return context;
  }

  template <typename Context>
  auto serialize(Context& context, const ::sqlpp::nullopt_t&) -> Context&
  {
    context << "NULL";
    return context;
  }

  template <typename T, typename Context>
  auto serialize(Context& context, const ::sqlpp::optional<T>& t) -> Context&
  {
    if (not t.has_value())
    {
      return serialize(context, ::sqlpp::nullopt);
    }
    return serialize(context, *t);
  }

  template <typename T, typename Context>
  auto serialize_operand(Context& context, const T& t) -> Context&
  {
    if (requires_parentheses<T>::value)
    {
      context << '(';
      serialize(context, t);
      context << ')';
    }
    else
    {
      serialize(context, t);
    }

    return context;
  }

  template <typename T, typename Context>
  auto serialize_name(Context& context, const T& t) -> Context&
  {
#warning: We used to have a version of SQLPP_ALIAS_PROVIDER that escaped the name

    context << t;

    return context;
  }

}  // namespace sqlpp

#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>

#include <sqlpp11/core/compat/optional.h>
#include <sqlpp11/core/chrono.h>
#include <sqlpp11/core/compat/span.h>
#include <sqlpp11/core/compat/string_view.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/database/exception.h>

namespace sqlpp
{
  template <typename Context, typename X = void>
  auto to_sql_string(Context& , ...) -> std::string
  {
    static_assert(wrong_t<X>::value, "Missing specialization");
    return {};
  }

  template <typename Context>
  auto to_sql_string(Context& , const bool& t) -> std::string
  {
    return std::to_string(t);
  }

  template <typename Context>
  auto to_sql_string(Context& , const int8_t& t) -> std::string
  {
    return std::to_string(t);
  }

  template <typename Context>
  auto to_sql_string(Context& , const int16_t& t) -> std::string
  {
    return std::to_string(t);
  }

  template <typename Context>
  auto to_sql_string(Context& , const int32_t& t) -> std::string
  {
    return std::to_string(t);
  }

  template <typename Context>
  auto to_sql_string(Context& , const int64_t& t) -> std::string
  {
    return std::to_string(t);
  }

  template <typename Context>
  auto to_sql_string(Context& , const uint8_t& t) -> std::string
  {
    return std::to_string(t);
  }

  template <typename Context>
  auto to_sql_string(Context& , const uint16_t& t) -> std::string
  {
    return std::to_string(t);
  }

  template <typename Context>
  auto to_sql_string(Context& , const uint32_t& t) -> std::string
  {
    return std::to_string(t);
  }

  template <typename Context>
  auto to_sql_string(Context& , const uint64_t& t) -> std::string
  {
    return std::to_string(t);
  }

  template <typename Context>
  auto nan_to_sql_string(Context& ) -> std::string
  {
    throw ::sqlpp::exception("Serialization of NaN is not supported by this connector");
  }

  template <typename Context>
  auto inf_to_sql_string(Context& ) -> std::string
  {
    throw ::sqlpp::exception("Serialization of Infinity is not supported by this connector");
  }

  template <typename Context>
  auto neg_inf_to_sql_string(Context& ) -> std::string
  {
    throw ::sqlpp::exception("Serialization of Infinity is not supported by this connector");
  }

  template <typename Context, typename T>
  auto float_to_sql_string(Context& context, const T& f) -> std::string
  {
    if (std::isnan(f))
    {
      return nan_to_sql_string(context);
    }
    else if (std::isinf(f))
    {
      return f > std::numeric_limits<T>::max() ? inf_to_sql_string(context) : neg_inf_to_sql_string(context);
    }
    else
    {
      // TODO: Once gcc and clang support to_chars, try that
      auto oss = std::ostringstream{};
      oss << std::setprecision(std::numeric_limits<T>::max_digits10) << f;
      return oss.str();
    }
  }

  template <typename Context>
  auto to_sql_string(Context& context, const float& t) -> std::string
  {
    return float_to_sql_string(context, t);
  }

  template <typename Context>
  auto to_sql_string(Context& context, const double& t) -> std::string
  {
    return float_to_sql_string(context, t);
  }

  template <typename Context>
  auto to_sql_string(Context& context, const long double& t) -> std::string
  {
    return float_to_sql_string(context, t);
  }

  template <typename Context>
  auto to_sql_string(Context& , const ::sqlpp::string_view& t) -> std::string
  {
    auto result = std::string{"'"};
    result.reserve(t.size() * 2);
    for (const auto c : t)
    {
      if (c == '\'')
        result.push_back(c);  // Escaping
      result.push_back(c);
    }
    result.push_back('\'');
    return result;

  }

  template <typename Context>
  auto to_sql_string(Context& context, const char* t) -> std::string
  {
    return to_sql_string(context, std::string_view(t));
  }

  template <typename Context>
  auto to_sql_string(Context& context, const std::string& t) -> std::string
  {
    return to_sql_string(context, std::string_view(t));
  }

  template <typename Context>
  auto to_sql_string(Context& context, const char& t) -> std::string
  {
    return to_sql_string(context, sqlpp::string_view(&t, 1));
  }

  template <typename Context>
  auto to_sql_string(Context& , const ::sqlpp::span<uint8_t>& t) -> std::string
  {
    constexpr char hexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    auto result = std::string{"x'"};
    result.reserve(t.size() * 2 + 3);

    for (const auto c : t)
    {
      result.push_back(hexChars[c >> 4]);
      result.push_back(hexChars[c & 0x0F]);
    }
    result.push_back('\'');

    return result;
  }

  template <typename Context>
  auto to_sql_string(Context& context, const std::vector<uint8_t>& t) -> std::string
  {
    return to_sql_string(context, ::sqlpp::span<uint8_t>(t.data(), t.size()));
  }

  template <typename Context, std::size_t N>
  auto to_sql_string(Context& context, const std::array<uint8_t, N>& t) -> std::string
  {
    return to_sql_string(context, ::sqlpp::span<uint8_t>(t.data(), t.size()));
  }

  template <typename Context>
  auto to_sql_string(Context& , const ::sqlpp::chrono::day_point& t) -> std::string
  {
    return date::format("DATE '%Y-%m-%d'", t);
  }

  template <typename Context>
  auto to_sql_string(Context&, const std::chrono::microseconds& t) -> std::string
  {
    return date::format("'%H:%M:%S'", t);
  }

  template <typename Period, typename Context>
  auto to_sql_string(Context&, const std::chrono::time_point<std::chrono::system_clock, Period>& t) -> std::string
  {
    return date::format("TIMESTAMP '%Y-%m-%dT%H:%M:%S'", t);
  }

  template <typename Context>
  auto to_sql_string(Context&, const ::sqlpp::nullopt_t&) -> std::string
  {
    return "NULL";
  }

  template <typename T, typename Context>
  auto to_sql_string(Context& context, const ::sqlpp::optional<T>& t) -> std::string
  {
    if (not t.has_value())
    {
      return to_sql_string(context, ::sqlpp::nullopt);
    }
    return to_sql_string(context, *t);
  }

  template <typename T, typename Context>
  auto operand_to_sql_string(Context& context, const T& t) -> std::string
  {
    if (requires_parentheses<T>::value)
    {
      return "(" + to_sql_string(context, t) + ")";
    }
    return to_sql_string(context, t);
  }

  template <typename Context>
  auto quoted_name_to_sql_string(Context&, const ::sqlpp::string_view& name) -> std::string
  {
#warning : Need to change quotes for MySQL to forward ticks `name`
    return '"' + std::string(name) + '"';
  }

  template <typename NameTag, typename Context>
  auto name_to_sql_string(Context& context, const NameTag&) -> std::string
  {
    if (NameTag::require_quotes)
    {
      return quoted_name_to_sql_string(context, NameTag::name);
    }
    else
    {
      return std::string(NameTag::name);
    }
  }

}  // namespace sqlpp

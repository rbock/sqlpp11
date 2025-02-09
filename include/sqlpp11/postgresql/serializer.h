#pragma once

/**
 * Copyright © 2014-2015, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/core/chrono.h>
#include <sqlpp11/core/basic/parameter.h>

namespace sqlpp
{
  // Serialize parameters
  template <typename ValueType, typename NameType>
  auto to_sql_string(postgresql::context_t& context, const parameter_t<ValueType, NameType>&) -> std::string
  {
      return std::string("$") + std::to_string(++context._count);
  }

  namespace postgresql {
    // MySQL and sqlite3 use x'...', but PostgreSQL uses '\x...' to encode hexadecimal literals
    inline auto to_sql_string(postgresql::context_t&, const std::span<const uint8_t>& t) -> std::string
    {
      constexpr char hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
      auto result = std::string("'\\x");
      result.reserve(t.size() * 2 + 4);
      for (const auto c : t)
      {
        result.push_back(hex_chars[c >> 4]);
        result.push_back(hex_chars[c & 0x0F]);
      }
      result.push_back('\'');

      return result;
    }

    template <typename Period>
    auto to_sql_string(postgresql::context_t&,
                       const std::chrono::time_point<std::chrono::system_clock, Period>& t) -> std::string
    {
      return std::format("TIMESTAMP WITH TIME ZONE '{0:%Y-%m-%d %H:%M:%S+00}'", t);
    }

    inline auto to_sql_string(postgresql::context_t&, const ::sqlpp::chrono::day_point& t) -> std::string
    {
      return std::format("DATE '{0:%Y-%m-%d}'", t);
    }

    inline auto to_sql_string(postgresql::context_t&, const std::chrono::microseconds& t) -> std::string
    {
      return std::format("TIME WITH TIME ZONE'{0:%H:%M:%S+00}'", t);
    }

    inline auto to_sql_string(postgresql::context_t&, const bool& t) -> std::string
    {
      return t ? "'t'" : "'f'";
    }
  }  // namespace postgresql

}

/*
 * Copyright (c) 2023, Roland Bock
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

#include <sqlpp11/data_types/floating_point.h>
#include <sqlpp11/detail/float_safe_ostringstream.h>
// #include <sqlpp11/mysql/connection.h>
// #include <sqlpp11/postgresql/connection.h>
// #include <sqlpp11/sqlite3/connection.h>
#include <sqlpp11/value.h>

#include "compare.h"

namespace
{
  template <typename T>
  void wrapper_serializes_value_as(int line, T value, std::string expected)
  {
    sqlpp::detail::float_safe_ostringstream os;
    os << value;
    assert_equal(line, os.str(), expected);
  }

  template <typename T>
  void wrapper_serializes_in_deserializable_format(int line, T value)
  {
    sqlpp::detail::float_safe_ostringstream os;
    os << value;
    std::istringstream is{os.str()};
    T deserialized;
    is >> deserialized;
    assert_equal(line, deserialized, value);
  }

  template <typename Serializer, typename Expression>
  void given_serializer_serializes_expr_as(int lineNo,
                                           Serializer& serializer,
                                           const Expression& expr,
                                           const std::string& expected)
  {
    assert_equal(lineNo, serialize(expr, serializer).str(), expected);
  }

  template <typename T>
  std::string string_for_10_0000086()
  {
    switch (std::numeric_limits<T>::max_digits10)
    {
      case 9:
        return "10.0000086";
      case 17:
        return "10.000008599999999";
      case 21:
        return "10.0000086000000000001";
      default:
        throw std::logic_error("Unknown floating point digit count");
    }
  }
}  // namespace

int Float(int, char*[])
{
  wrapper_serializes_value_as(__LINE__, 10.0000086f, string_for_10_0000086<float>());
  wrapper_serializes_value_as(__LINE__, 10.0000086, string_for_10_0000086<double>());
  wrapper_serializes_value_as(__LINE__, 10.0000086l, string_for_10_0000086<long double>());

  wrapper_serializes_in_deserializable_format(__LINE__, 10.0000086f);
  wrapper_serializes_in_deserializable_format(__LINE__, 10.0000086);
  wrapper_serializes_in_deserializable_format(__LINE__, 10.0000086l);

  {
    auto const value{sqlpp::value(10.0000114)};
    auto const expected{"10.0000114"};
    {
      MockDb::_serializer_context_t serializer;
      given_serializer_serializes_expr_as(__LINE__, serializer, value, expected);
    }

    // The following should also work, but require dependencies. The assumption is that
    // since they use the wrapper it is fine.

    // {
    //   sqlpp::sqlite3::connection_config config;
    //   sqlpp::sqlite3::connection connection {config};
    //   sqlpp::sqlite3::serializer_t serializer {connection};
    //   given_serializer_serializes_expr_as(__LINE__, serializer, value, expected);
    // }

    // {
    //   auto config {std::make_shared<sqlpp::mysql::connection_config>()};
    //   sqlpp::mysql::connection connection {config};
    //   sqlpp::mysql::serializer_t serializer {connection};
    //   given_serializer_serializes_expr_as(__LINE__, serializer, value, expected);
    // }

    // {
    //     sqlpp::postgresql::connection connection;
    //     sqlpp::postgresql::context_t serializer {connection};
    //     given_serializer_serializes_expr_as(__LINE__, serializer, value, expected);
    // }
  }

  return 0;
}

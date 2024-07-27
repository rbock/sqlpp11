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

#include <sqlpp11/sqlpp11.h>

namespace
{
  template <typename T, typename V>
  using is_same_type = std::is_same<sqlpp::value_type_of_t<T>, V>;

  SQLPP_ALIAS_PROVIDER(r_not_null);
  SQLPP_ALIAS_PROVIDER(r_maybe_null);
}

template <typename Value>
void test_case_when(Value v)
{
  using ValueType = sqlpp::value_type_of_t<Value>;
  using OptValueType = sqlpp::value_type_of_t<sqlpp::compat::optional<Value>>;

  // Selectable values.
  auto v_not_null = sqlpp::value(v);
  const auto v_maybe_null = sqlpp::value(sqlpp::compat::make_optional(v));

  // No value types for incomplete clauses
  static_assert(is_same_type<decltype(sqlpp::case_when(true)), sqlpp::no_value_t>::value, "");
  static_assert(is_same_type<decltype(sqlpp::case_when(true).then(v_not_null)), sqlpp::no_value_t>::value, "");

  // The value type is optional if either of the of the values is optional
  static_assert(is_same_type<decltype(sqlpp::case_when(true).then(v_not_null).else_(v_not_null)), ValueType>::value,
                "");
  static_assert(
      is_same_type<decltype(sqlpp::case_when(true).then(v_not_null).else_(v_maybe_null)), OptValueType>::value, "");
  static_assert(
      is_same_type<decltype(sqlpp::case_when(true).then(v_maybe_null).else_(v_not_null)), OptValueType>::value, "");
  static_assert(
      is_same_type<decltype(sqlpp::case_when(true).then(v_maybe_null).else_(v_maybe_null)), OptValueType>::value, "");

  // The value type is always optional if the condition is optional
  const auto opt_bool = sqlpp::compat::make_optional(true);
  static_assert(
      is_same_type<decltype(sqlpp::case_when(opt_bool).then(v_not_null).else_(v_maybe_null)), OptValueType>::value, "");

#warning: test can be aliased
#warning: test has comparison operators
#warning: test nodes
}

int main()
{
  // boolean
  test_case_when(bool{true});

  // integral
  test_case_when(int8_t{7});
  test_case_when(int16_t{7});
  test_case_when(int32_t{7});
  test_case_when(int64_t{7});

  // unsigned integral
  test_case_when(uint8_t{7});
  test_case_when(uint16_t{7});
  test_case_when(uint32_t{7});
  test_case_when(uint64_t{7});

  // floating point
  test_case_when(float{7.7});
  test_case_when(double{7.7});

  // text
  test_case_when('7');
  test_case_when("seven");
  test_case_when(std::string("seven"));
  test_case_when(sqlpp::compat::string_view("seven"));

  // blob
  test_case_when(std::vector<uint8_t>{});

  // date
  test_case_when(::sqlpp::chrono::day_point{});

  // timestamp
  test_case_when(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_case_when(minute_point{});

  // time_of_day
  test_case_when(std::chrono::microseconds{});
}


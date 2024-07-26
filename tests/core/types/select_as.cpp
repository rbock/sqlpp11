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

#include "MockDb.h"
#include "Sample.h"
#include <sqlpp11/sqlpp11.h>

namespace
{
  template <typename T, typename V>
  using is_same_type = std::is_same<sqlpp::value_type_of_t<T>, V>;

  SQLPP_ALIAS_PROVIDER(always);
  SQLPP_ALIAS_PROVIDER(sometimes);
  SQLPP_ALIAS_PROVIDER(column)
  SQLPP_ALIAS_PROVIDER(table)
  SQLPP_ALIAS_PROVIDER(foo)
}

template <typename Value>
void test_select_as(Value v)
{
  auto v_not_null = sqlpp::value(v).as(always);
  auto v_maybe_null = sqlpp::value(sqlpp::compat::make_optional(v)).as(sometimes);

  using ValueType = sqlpp::value_type_of_t<Value>;
  using OptValueType = sqlpp::value_type_of_t<sqlpp::compat::optional<Value>>;

  // SINGLE VALUE

  // A select of a single value can be used as a value.
  static_assert(is_same_type<decltype(select(v_not_null)), ValueType>(), "");
  static_assert(is_same_type<decltype(select(v_maybe_null)), OptValueType>(), "");

  // A select of a single value can be named and used as a named value.
  static_assert(sqlpp::has_name<decltype(select(v_not_null).as(column))>::value, "");
  static_assert(sqlpp::has_name<decltype(select(v_maybe_null).as(column))>::value, "");

  static_assert(is_same_type<decltype(select(v_not_null).as(column)), ValueType>(), "");
  static_assert(is_same_type<decltype(select(v_maybe_null).as(column)), OptValueType>(), "");

  // A select of a single value can be named and used as a pseudo table
  static_assert(sqlpp::is_table<decltype(select(v_not_null).as(column))>::value, "");
  static_assert(sqlpp::is_table<decltype(select(v_maybe_null).as(column))>::value, "");

  // The column of a single-value pseudo table can be used as named value
  static_assert(sqlpp::has_name<decltype(select(v_not_null).as(column).always)>::value, "");
  static_assert(sqlpp::has_name<decltype(select(v_maybe_null).as(column).sometimes)>::value, "");

  static_assert(is_same_type<decltype(select(v_not_null).as(column).always), ValueType>(), "");
  static_assert(is_same_type<decltype(select(v_maybe_null).as(column).sometimes), OptValueType>(), "");

  // The column of a single-value pseudo table can be renamed and used as named value
  static_assert(sqlpp::has_name<decltype(select(v_not_null).as(column).always.as(foo))>::value, "");
  static_assert(sqlpp::has_name<decltype(select(v_maybe_null).as(column).sometimes.as(foo))>::value, "");

  static_assert(is_same_type<decltype(select(v_not_null).as(column).always.as(foo)), ValueType>(), "");
  static_assert(is_same_type<decltype(select(v_maybe_null).as(column).sometimes.as(foo)), OptValueType>(), "");

  // MULTIPLE VALUES

  // A select of multiple values can not be used as a value.
  static_assert(not sqlpp::has_value_type<decltype(select(v_not_null, v_maybe_null))>::value, "");

  // A select of multiple values can be named and used as a named value.
  static_assert(sqlpp::has_name<decltype(select(v_not_null, v_maybe_null).as(column))>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(select(v_not_null, v_maybe_null).as(column))>::value, "");

  // A select of multiple values can be named and used as a pseudo table
  static_assert(sqlpp::is_table<decltype(select(v_not_null, v_maybe_null).as(table))>::value, "");

  // The column of a multi-value pseudo table can be used as named value
  static_assert(sqlpp::has_name<decltype(select(v_not_null, v_maybe_null).as(table).always)>::value, "");
  static_assert(sqlpp::has_name<decltype(select(v_not_null, v_maybe_null).as(table).sometimes)>::value, "");

  static_assert(is_same_type<decltype(select(v_not_null, v_maybe_null).as(table).always), ValueType>(), "");
  static_assert(is_same_type<decltype(select(v_not_null, v_maybe_null).as(table).sometimes), OptValueType>(), "");

  // The column of a multi-value pseudo table can be renamed and used as named value
  static_assert(sqlpp::has_name<decltype(select(v_not_null, v_maybe_null).as(table).always.as(foo))>::value, "");
  static_assert(sqlpp::has_name<decltype(select(v_not_null, v_maybe_null).as(table).sometimes.as(foo))>::value, "");

  static_assert(is_same_type<decltype(select(v_not_null, v_maybe_null).as(table).always.as(foo)), ValueType>(), "");
  static_assert(is_same_type<decltype(select(v_not_null, v_maybe_null).as(table).sometimes.as(foo)), OptValueType>(), "");


#warning: test can be aliased
#warning: test has comparison operators
#warning: test nodes

}

int main()
{
  // boolean
  test_select_as(bool{true});

  // integral
  test_select_as(int8_t{7});
  test_select_as(int16_t{7});
  test_select_as(int32_t{7});
  test_select_as(int64_t{7});

  // unsigned integral
  test_select_as(uint8_t{7});
  test_select_as(uint16_t{7});
  test_select_as(uint32_t{7});
  test_select_as(uint64_t{7});

  // floating point
  test_select_as(float{7.7});
  test_select_as(double{7.7});

  // text
  test_select_as('7');
  test_select_as("seven");
  test_select_as(std::string("seven"));
  test_select_as(sqlpp::compat::string_view("seven"));

  // blob
  test_select_as(std::vector<uint8_t>{});

  // date
  test_select_as(::sqlpp::chrono::day_point{});

  // timestamp
  test_select_as(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_select_as(minute_point{});

  // time_of_day
  test_select_as(std::chrono::microseconds{});
}


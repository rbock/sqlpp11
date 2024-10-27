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
  auto db = MockDb{};

  template <typename T, typename V>
  using is_same_type = std::is_same<sqlpp::value_type_of_t<T>, V>;
}

template <typename Value>
void test_max(Value v)
{
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(::sqlpp::make_optional(v));

  using OptValueType = sqlpp::value_type_of_t<sqlpp::optional<Value>>;

  // max of non-nullable can be null because there could be zero result rows.
  static_assert(is_same_type<decltype(max(v_not_null)), OptValueType>::value, "");
  static_assert(is_same_type<decltype(max(sqlpp::distinct, v_not_null)), OptValueType>::value, "");

  // max of nullable
  static_assert(is_same_type<decltype(max(v_maybe_null)), OptValueType>::value, "");
  static_assert(is_same_type<decltype(max(sqlpp::distinct, v_maybe_null)), OptValueType>::value, "");

  // max enables the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(max(v_not_null))>::value, "");
  static_assert(sqlpp::has_enabled_as<decltype(sqlpp::distinct, max(v_not_null))>::value, "");

  // max has a name
  static_assert(sqlpp::has_name_tag<decltype(max(v_not_null))>::value, "");
  static_assert(sqlpp::has_name_tag<decltype(max(sqlpp::distinct, v_not_null))>::value, "");

  static_assert(std::is_same<sqlpp::name_tag_of_t<decltype(max(v_not_null))>, sqlpp::alias::_max_t::_sqlpp_name_tag>::value, "");
  static_assert(std::is_same<sqlpp::name_tag_of_t<decltype(max(sqlpp::distinct, v_not_null))>, sqlpp::alias::_max_t::_sqlpp_name_tag>::value, "");

  // max enables comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(max(v_not_null))>::value, "");
  static_assert(sqlpp::has_enabled_comparison<decltype(max(sqlpp::distinct, v_not_null))>::value, "");

  // max enables OVER.
  static_assert(sqlpp::has_enabled_over<decltype(max(v_not_null))>::value, "");
  static_assert(sqlpp::has_enabled_over<decltype(max(sqlpp::distinct, v_not_null))>::value, "");

  // max has its argument as nodes
  using L = typename std::decay<decltype(v_not_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(max(v_not_null))>, sqlpp::detail::type_vector<L>>::value, "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(max(sqlpp::distinct, v_not_null))>, sqlpp::detail::type_vector<L>>::value, "");
}

int main()
{
  // boolean
  test_max(bool{true});

  // integral
  test_max(int8_t{7});
  test_max(int16_t{7});
  test_max(int32_t{7});
  test_max(int64_t{7});

  // unsigned integral
  test_max(uint8_t{7});
  test_max(uint16_t{7});
  test_max(uint32_t{7});
  test_max(uint64_t{7});

  // floating point
  test_max(float{7.7});
  test_max(double{7.7});

  // text
  test_max('7');
  test_max("seven");
  test_max(std::string("seven"));
  test_max(::sqlpp::string_view("seven"));

  // blob
  test_max(std::vector<uint8_t>{});

  // date
  test_max(::sqlpp::chrono::day_point{});

  // timestamp
  test_max(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_max(minute_point{});

  // time_of_day
  test_max(std::chrono::microseconds{});
}


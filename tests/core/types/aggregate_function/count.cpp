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
void test_count(Value v)
{
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(::sqlpp::make_optional(v));

  // count of non-nullable
  static_assert(is_same_type<decltype(count(v_not_null)), sqlpp::integral>::value, "");
  static_assert(is_same_type<decltype(count(sqlpp::distinct, v_not_null)), sqlpp::integral>::value, "");

  // count of nullable
  static_assert(is_same_type<decltype(count(v_maybe_null)), sqlpp::integral>::value, "");
  static_assert(is_same_type<decltype(count(sqlpp::distinct, v_maybe_null)), sqlpp::integral>::value, "");

  // count enables the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(count(v_not_null))>::value, "");
  static_assert(sqlpp::has_enabled_as<decltype(sqlpp::distinct, count(v_not_null))>::value, "");

  // count has a name
  static_assert(sqlpp::has_name<decltype(count(v_not_null))>::value, "");
  static_assert(sqlpp::has_name<decltype(count(sqlpp::distinct, v_not_null))>::value, "");

  static_assert(std::is_same<sqlpp::name_tag_of_t<decltype(count(v_not_null))>, sqlpp::alias::_count_t::_sqlpp_name_tag>::value, "");
  static_assert(std::is_same<sqlpp::name_tag_of_t<decltype(count(sqlpp::distinct, v_not_null))>, sqlpp::alias::_count_t::_sqlpp_name_tag>::value, "");

  // count enables comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(count(v_not_null))>::value, "");
  static_assert(sqlpp::has_enabled_comparison<decltype(count(sqlpp::distinct, v_not_null))>::value, "");

  // count enables OVER.
  static_assert(sqlpp::has_enabled_over<decltype(count(v_not_null))>::value, "");
  static_assert(sqlpp::has_enabled_over<decltype(count(sqlpp::distinct, v_not_null))>::value, "");

  // count has its argument as nodes
  using L = typename std::decay<decltype(v_not_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(count(v_not_null))>, sqlpp::detail::type_vector<L>>::value, "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(count(sqlpp::distinct, v_not_null))>, sqlpp::detail::type_vector<L>>::value, "");
}

int main()
{
  // boolean
  test_count(bool{true});

  // integral
  test_count(int8_t{7});
  test_count(int16_t{7});
  test_count(int32_t{7});
  test_count(int64_t{7});

  // unsigned integral
  test_count(uint8_t{7});
  test_count(uint16_t{7});
  test_count(uint32_t{7});
  test_count(uint64_t{7});

  // floating point
  test_count(float{7.7});
  test_count(double{7.7});

  // text
  test_count('7');
  test_count("seven");
  test_count(std::string("seven"));
  test_count(::sqlpp::string_view("seven"));

  // blob
  test_count(std::vector<uint8_t>{});

  // date
  test_count(::sqlpp::chrono::day_point{});

  // timestamp
  test_count(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_count(minute_point{});

  // time_of_day
  test_count(std::chrono::microseconds{});
}

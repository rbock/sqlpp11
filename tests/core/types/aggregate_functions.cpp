/*
 * Copyright (c) 2016-2016, Roland Bock
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
void test_aggregate_functions(Value v)
{
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(sqlpp::compat::make_optional(v));

  using OptValueType = sqlpp::value_type_of_t<sqlpp::compat::optional<Value>>;

  // Aggregate of non-nullable
  static_assert(is_same_type<decltype(count(v_not_null)), sqlpp::integral>::value, "");
  static_assert(is_same_type<decltype(max(v_not_null)), OptValueType>::value, "");
  static_assert(is_same_type<decltype(min(v_not_null)), OptValueType>::value, "");

  // Aggregate of nullable
  static_assert(is_same_type<decltype(count(v_not_null)), sqlpp::integral>::value, "");
  static_assert(is_same_type<decltype(max(v_maybe_null)), OptValueType>::value, "");
  static_assert(is_same_type<decltype(min(v_maybe_null)), OptValueType>::value, "");
}

template <typename Value>
void test_numeric_aggregate_functions(Value v)
{
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(sqlpp::compat::make_optional(v));

  using ValueType = typename std::conditional<std::is_same<Value, bool>::value, int, Value>::type;
  using OptValueType = sqlpp::value_type_of_t<sqlpp::compat::optional<ValueType>>;
  using OptFloat = sqlpp::value_type_of_t<sqlpp::compat::optional<float>>;

  // Aggregate of non-nullable
  static_assert(is_same_type<decltype(sum(v_not_null)), OptValueType>::value, "");
  static_assert(is_same_type<decltype(avg(v_not_null)), OptFloat>::value, "");

  // Aggregate of nullable
  static_assert(is_same_type<decltype(sum(v_maybe_null)), OptValueType>::value, "");
  static_assert(is_same_type<decltype(avg(v_maybe_null)), OptFloat>::value, "");
}

int main()
{
  // boolean
  test_aggregate_functions(bool{true});
  test_numeric_aggregate_functions(bool{true});

  // integral
  test_aggregate_functions(int8_t{7});
  test_aggregate_functions(int16_t{7});
  test_aggregate_functions(int32_t{7});
  test_aggregate_functions(int64_t{7});

  test_numeric_aggregate_functions(int8_t{7});
  test_numeric_aggregate_functions(int16_t{7});
  test_numeric_aggregate_functions(int32_t{7});
  test_numeric_aggregate_functions(int64_t{7});

  // unsigned integral
  test_aggregate_functions(uint8_t{7});
  test_aggregate_functions(uint16_t{7});
  test_aggregate_functions(uint32_t{7});
  test_aggregate_functions(uint64_t{7});

  test_numeric_aggregate_functions(uint8_t{7});
  test_numeric_aggregate_functions(uint16_t{7});
  test_numeric_aggregate_functions(uint32_t{7});
  test_numeric_aggregate_functions(uint64_t{7});

  // floating point
  test_aggregate_functions(float{7.7});
  test_aggregate_functions(double{7.7});

  test_numeric_aggregate_functions(float{7.7});
  test_numeric_aggregate_functions(double{7.7});

  // text
  test_aggregate_functions('7');
  test_aggregate_functions("seven");
  test_aggregate_functions(std::string("seven"));
  test_aggregate_functions(sqlpp::compat::string_view("seven"));

  // blob
  test_aggregate_functions(std::vector<uint8_t>{});

  // date
  test_aggregate_functions(::sqlpp::chrono::day_point{});

  // timestamp
  test_aggregate_functions(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_aggregate_functions(minute_point{});

  // time_of_day
  test_aggregate_functions(std::chrono::microseconds{});
}


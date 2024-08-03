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

#warning: implement serialize instead of type tests here!
namespace
{
  template <typename T>
  using is_bool = std::is_same<sqlpp::value_type_of_t<T>, sqlpp::boolean>;

  template <typename T>
  using is_maybe_bool = std::is_same<sqlpp::value_type_of_t<T>, ::sqlpp::optional<sqlpp::boolean>>;
}

template <typename Value>
void test_in_expression(Value v)
{
  using OptValue = ::sqlpp::optional<Value>;

  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(::sqlpp::make_optional(v));

  // Compare non-nullable with non-nullable.
  static_assert(is_bool<decltype(in(v_not_null, std::make_tuple(v_not_null, v_not_null)))>::value, "");
  static_assert(is_bool<decltype(in(v_not_null, std::vector<Value>{}))>::value, "");
  static_assert(is_bool<decltype(in(v_not_null, select(v_not_null.as(sqlpp::alias::a))))>::value, "");

  // Compare non-nullable with nullable.
  static_assert(is_maybe_bool<decltype(in(v_not_null, std::make_tuple(v_not_null, v_maybe_null)))>::value, "");
  static_assert(is_maybe_bool<decltype(in(v_not_null, std::vector<OptValue>{}))>::value, "");
  static_assert(is_maybe_bool<decltype(in(v_not_null, select(v_maybe_null.as(sqlpp::alias::a))))>::value, "");

  // Compare nullable with non-nullable.
  static_assert(is_maybe_bool<decltype(in(v_maybe_null, std::make_tuple(v_not_null, v_not_null)))>::value, "");
  static_assert(is_maybe_bool<decltype(in(v_maybe_null, std::vector<Value>{}))>::value, "");
  static_assert(is_maybe_bool<decltype(in(v_maybe_null, select(v_not_null.as(sqlpp::alias::a))))>::value, "");

  // Compare nullable with nullable.
  static_assert(is_maybe_bool<decltype(in(v_maybe_null, std::make_tuple(v_not_null, v_maybe_null)))>::value, "");
  static_assert(is_maybe_bool<decltype(in(v_maybe_null, std::vector<OptValue>{}))>::value, "");
  static_assert(is_maybe_bool<decltype(in(v_maybe_null, select(v_maybe_null.as(sqlpp::alias::a))))>::value, "");

  // IN expressions have the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(in(v_maybe_null, std::vector<OptValue>{}))>::value, "");

  // IN expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(in(v_maybe_null, std::vector<OptValue>{}))>::value, "");

  // IN expressions have their arguments as nodes.
  using L = typename std::decay<decltype(v_maybe_null)>::type;
  using R1= Value;
  using R2= OptValue;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(in(v_maybe_null, std::vector<Value>{}))>, sqlpp::detail::type_vector<L, R1>>::value, "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(in(v_maybe_null, v, ::sqlpp::make_optional(v)))>, sqlpp::detail::type_vector<L, R1, R2>>::value, "");
}

int main()
{
  // boolean
  test_in_expression(bool{true});
#warning reactivate
#if 0
  // integral
  test_in_expression(int8_t{7});
  test_in_expression(int16_t{7});
  test_in_expression(int32_t{7});
  test_in_expression(int64_t{7});

  // unsigned integral
  test_in_expression(uint8_t{7});
  test_in_expression(uint16_t{7});
  test_in_expression(uint32_t{7});
  test_in_expression(uint64_t{7});

  // floating point
  test_in_expression(float{7.7});
  test_in_expression(double{7.7});

  // text
  test_in_expression('7');
  test_in_expression("seven");
  test_in_expression(std::string("seven"));
  test_in_expression(::sqlpp::string_view("seven"));

  // blob
  test_in_expression(std::vector<uint8_t>{});

  // date
  test_in_expression(::sqlpp::chrono::day_point{});

  // timestamp
  test_in_expression(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_in_expression(minute_point{});

  // time_of_day
  test_in_expression(std::chrono::microseconds{});
#endif
}


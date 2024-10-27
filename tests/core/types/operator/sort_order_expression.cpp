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

template<typename Value>
void test_as_expression(Value v)
{
  using ValueType = sqlpp::value_type_of_t<Value>;
  using OptValueType = ::sqlpp::optional<ValueType>;

  auto v_not_null= sqlpp::value(v);
  auto v_maybe_null= sqlpp::value(::sqlpp::make_optional(v));

  // Sort order expressions have no value.
  static_assert(not sqlpp::has_value_type<decltype(v_not_null.asc())>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(v_not_null.desc())>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(v_not_null.order(sqlpp::sort_type::asc))>::value, "");

  static_assert(not sqlpp::has_value_type<decltype(v_maybe_null.asc())>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(v_maybe_null.desc())>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(v_maybe_null.order(sqlpp::sort_type::asc))>::value, "");

  static_assert(not sqlpp::has_value_type<decltype(dynamic(true, v_not_null.asc()))>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(dynamic(true, v_not_null.desc()))>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(dynamic(true, v_not_null.order(sqlpp::sort_type::asc)))>::value, "");

  static_assert(not sqlpp::has_value_type<decltype(dynamic(true, v_maybe_null.asc()))>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(dynamic(true, v_maybe_null.desc()))>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(dynamic(true, v_maybe_null.order(sqlpp::sort_type::asc)))>::value, "");

  // Sort order expressions have no name.
  static_assert(not sqlpp::has_name_tag<decltype(v_not_null.asc())>::value, "");
  static_assert(not sqlpp::has_name_tag<decltype(v_maybe_null.asc())>::value, "");
  static_assert(not sqlpp::has_name_tag<decltype(dynamic(true, v_not_null.asc()))>::value, "");
  static_assert(not sqlpp::has_name_tag<decltype(dynamic(true, v_maybe_null.asc()))>::value, "");

  // Sort order expression do not enable the `as` member function.
  static_assert(not sqlpp::has_enabled_as<decltype(v_not_null.asc())>::value, "");

  // Sort order expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(v_not_null.asc())>::value, "");

  // Sort order expressions have their arguments as nodes.
  using L = typename std::decay<decltype(v_not_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(v_not_null.asc())>, sqlpp::detail::type_vector<L>>::value, "");
}

int main()
{
  // boolean
  test_as_expression(bool{true});

  // integral
  test_as_expression(int8_t{7});
  test_as_expression(int16_t{7});
  test_as_expression(int32_t{7});
  test_as_expression(int64_t{7});

  // unsigned integral
  test_as_expression(uint8_t{7});
  test_as_expression(uint16_t{7});
  test_as_expression(uint32_t{7});
  test_as_expression(uint64_t{7});

  // floating point
  test_as_expression(float{7.7});
  test_as_expression(double{7.7});

  // text
  test_as_expression('7');
  test_as_expression("seven");
  test_as_expression(std::string("seven"));
  test_as_expression(::sqlpp::string_view("seven"));

  // blob
  test_as_expression(std::vector<uint8_t>{});

  // date
  test_as_expression(::sqlpp::chrono::day_point{});

  // timestamp
  test_as_expression(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_as_expression(minute_point{});

  // time_of_day
  test_as_expression(std::chrono::microseconds{});
}


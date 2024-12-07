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

#include <sqlpp11/tests/core/MockDb.h>
#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/sqlpp11.h>

namespace
{
  auto db = MockDb{};

  template <typename T>
  using is_bool = std::is_same<sqlpp::value_type_of_t<T>, sqlpp::boolean>;

  template <typename T>
  using is_maybe_bool = std::is_same<sqlpp::value_type_of_t<T>, ::sqlpp::optional<sqlpp::boolean>>;
}

template <typename Value>
void test_comparison_expression(Value v)
{
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(::sqlpp::make_optional(v));

#warning : Add tests for between member function (here or in between tests)!

  // Compare non-nullable with non-nullable.
  static_assert(is_bool<decltype(v_not_null < v_not_null)>::value, "");
  static_assert(is_bool<decltype(v_not_null <= v_not_null)>::value, "");
  static_assert(is_bool<decltype(v_not_null == v_not_null)>::value, "");
  static_assert(is_bool<decltype(v_not_null != v_not_null)>::value, "");
  static_assert(is_bool<decltype(v_not_null == v_not_null)>::value, "");
  static_assert(is_bool<decltype(v_not_null >= v_not_null)>::value, "");
  static_assert(is_bool<decltype(v_not_null > v_not_null)>::value, "");
  static_assert(is_bool<decltype(is_distinct_from(v_not_null, v_not_null))>::value, "");
  static_assert(is_bool<decltype(is_not_distinct_from(v_not_null, v_not_null))>::value, "");

  // Compare non-nullable with nullable.
  static_assert(is_maybe_bool<decltype(v_not_null < v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null <= v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null == v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null != v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null == v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null >= v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null > v_maybe_null)>::value, "");
  static_assert(is_bool<decltype(is_distinct_from(v_not_null, v_maybe_null))>::value, "");
  static_assert(is_bool<decltype(is_not_distinct_from(v_not_null, v_maybe_null))>::value, "");

  // Compare nullable with non-nullable.
  static_assert(is_maybe_bool<decltype(v_maybe_null < v_not_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null <= v_not_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null == v_not_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null != v_not_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null == v_not_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null >= v_not_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null > v_not_null)>::value, "");
  static_assert(is_bool<decltype(is_distinct_from(v_maybe_null, v_not_null))>::value, "");
  static_assert(is_bool<decltype(is_not_distinct_from(v_maybe_null, v_not_null))>::value, "");

  // Compare nullable with nullable.
  static_assert(is_maybe_bool<decltype(v_maybe_null < v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null <= v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null == v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null != v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null == v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null >= v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null > v_maybe_null)>::value, "");
  static_assert(is_bool<decltype(is_distinct_from(v_maybe_null, v_maybe_null))>::value, "");
  static_assert(is_bool<decltype(is_not_distinct_from(v_maybe_null, v_maybe_null))>::value, "");

  // Compare with null.
  static_assert(is_bool<decltype(is_null(v_not_null))>::value, "");
  static_assert(is_bool<decltype(is_null(v_maybe_null))>::value, "");
  static_assert(is_bool<decltype(is_not_null(v_maybe_null))>::value, "");
  static_assert(is_bool<decltype(is_not_null(v_not_null))>::value, "");

  // Comparison expressions have the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(v_not_null == v_maybe_null)>::value, "");
  static_assert(sqlpp::has_enabled_as<decltype(is_null(v_not_null))>::value, "");

  // Comparison expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(v_not_null == v_maybe_null)>::value, "");

  // Comparison expressions have their arguments as nodes.
  using L = typename std::decay<decltype(v_not_null)>::type;
  using R = typename std::decay<decltype(v_maybe_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(v_not_null == v_maybe_null)>, sqlpp::detail::type_vector<L, R>>::value, "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(is_null(v_not_null))>, sqlpp::detail::type_vector<L, ::sqlpp::nullopt_t>>::value, "");
}

template<typename Value>
void test_like(Value v)
{
  auto v_not_null= sqlpp::value(v);
  auto v_maybe_null= sqlpp::value(::sqlpp::make_optional(v));

  // Compare non-nullable with non-nullable.
  static_assert(is_bool<decltype(like(v_not_null, v_not_null))>::value, "");

  // Compare non-nullable with nullable.
  static_assert(is_maybe_bool<decltype(like(v_not_null, v_maybe_null))>::value, "");

  // Compare nullable with non-nullable.
  static_assert(is_maybe_bool<decltype(like(v_maybe_null, v_not_null))>::value, "");

  // Compare nullable with nullable.
  static_assert(is_maybe_bool<decltype(like(v_maybe_null, v_maybe_null))>::value, "");
}

int main()
{
  // boolean
  test_comparison_expression(bool{true});

  // integral
  test_comparison_expression(int8_t{7});
  test_comparison_expression(int16_t{7});
  test_comparison_expression(int32_t{7});
  test_comparison_expression(int64_t{7});

  // unsigned integral
  test_comparison_expression(uint8_t{7});
  test_comparison_expression(uint16_t{7});
  test_comparison_expression(uint32_t{7});
  test_comparison_expression(uint64_t{7});

  // floating point
  test_comparison_expression(float{7.7});
  test_comparison_expression(double{7.7});

  // text
  test_comparison_expression('7');
  test_comparison_expression("seven");
  test_comparison_expression(std::string("seven"));
  test_comparison_expression(::sqlpp::string_view("seven"));

  // blob
  test_comparison_expression(std::vector<uint8_t>{});

  // date
  test_comparison_expression(::sqlpp::chrono::day_point{});

  // timestamp
  test_comparison_expression(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_comparison_expression(minute_point{});

  // time_of_day
  test_comparison_expression(std::chrono::microseconds{});

  // text
  test_like('7');
  test_like("seven");
  test_like(std::string("seven"));
  test_like(::sqlpp::string_view("seven"));

}


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
  using is_maybe_bool = std::is_same<sqlpp::value_type_of_t<T>, std::optional<sqlpp::boolean>>;
}

template <typename Column, typename Value>
void test_assign_expression(const Column& col, const Value& v)
{
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(std::make_optional(v));

  using ValueType = decltype(v_not_null);
  using OptValueType = decltype(v_maybe_null);

  // Assignments have no value
  static_assert(not sqlpp::has_value_type<decltype(col = sqlpp::default_value)>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(col = v_not_null)>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(col = v_maybe_null)>::value, "");

  // Assignments have no name
  static_assert(not sqlpp::has_name_tag<decltype(col = sqlpp::default_value)>::value, "");
  static_assert(not sqlpp::has_name_tag<decltype(col = v_not_null)>::value, "");
  static_assert(not sqlpp::has_name_tag<decltype(col = v_maybe_null)>::value, "");

  // Assignment nodes
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col = sqlpp::default_value)>,
                             sqlpp::detail::type_vector<Column, sqlpp::default_value_t>>::value,
                "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col = v_not_null)>,
                             sqlpp::detail::type_vector<Column, ValueType>>::value,
                "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col = v_maybe_null)>,
                             sqlpp::detail::type_vector<Column, OptValueType>>::value,
                "");

  // Assign expressions do not have the `as` member function.
  static_assert(not sqlpp::has_enabled_as<decltype(col = v_not_null)>::value, "");

  // Assign expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(col = v_not_null)>::value, "");

  // Assign expressions have their arguments as nodes.
  using L = typename std::decay<decltype(col)>::type;
  using R = typename std::decay<decltype(v_not_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col = v_not_null)>, sqlpp::detail::type_vector<L, R>>::value, "");
}

template <typename Column, typename Value>
void test_compound_assign_expression(const Column& col, const Value& v)
{
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(std::make_optional(v));

  using ValueType = decltype(v_not_null);
  using OptValueType = decltype(v_maybe_null);

  // Assignments have no value
  static_assert(not sqlpp::has_value_type<decltype(col += v_not_null)>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(col += v_maybe_null)>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(col -= v_not_null)>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(col -= v_maybe_null)>::value, "");

  // Assignments have no name
  static_assert(not sqlpp::has_name_tag<decltype(col += v_not_null)>::value, "");
  static_assert(not sqlpp::has_name_tag<decltype(col += v_maybe_null)>::value, "");
  static_assert(not sqlpp::has_name_tag<decltype(col -= v_not_null)>::value, "");
  static_assert(not sqlpp::has_name_tag<decltype(col -= v_maybe_null)>::value, "");

  // Assignment nodes
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col += v_not_null)>,
                             sqlpp::detail::type_vector<Column, ValueType>>::value,
                "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col += v_maybe_null)>,
                             sqlpp::detail::type_vector<Column, OptValueType>>::value,
                "");

  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col -= v_not_null)>,
                             sqlpp::detail::type_vector<Column, ValueType>>::value,
                "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col -= v_maybe_null)>,
                             sqlpp::detail::type_vector<Column, OptValueType>>::value,
                "");

  // Assign expressions do not have the `as` member function.
  static_assert(not sqlpp::has_enabled_as<decltype(col += v_not_null)>::value, "");
  static_assert(not sqlpp::has_enabled_as<decltype(col -= v_not_null)>::value, "");

  // Assign expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(col += v_not_null)>::value, "");
  static_assert(not sqlpp::has_enabled_comparison<decltype(col -= v_not_null)>::value, "");

  // Assign expressions have their arguments as nodes.
  using L = typename std::decay<decltype(col)>::type;
  using R = typename std::decay<decltype(v_not_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col += v_not_null)>, sqlpp::detail::type_vector<L, R>>::value, "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(col -= v_not_null)>, sqlpp::detail::type_vector<L, R>>::value, "");
}

int main()
{
  const auto bar = test::TabBar{};
  const auto foo = test::TabFoo{};
  const auto date_time = test::TabDateTime{};

  // boolean
  test_assign_expression(foo.boolN, bool{true});

  // integral
  test_assign_expression(foo.intN, int8_t{7});
  test_compound_assign_expression(foo.intN, int8_t{7});

  // unsigned integral
  test_compound_assign_expression(foo.uIntN, uint8_t{7});

  // floating point
  test_compound_assign_expression(foo.doubleN, float{7.7});

  // text
  test_assign_expression(bar.textN, '7');
  test_assign_expression(bar.textN, "seven");
  test_assign_expression(bar.textN, std::string("seven"));
  test_assign_expression(bar.textN, std::string_view("seven"));

  // blob
  test_assign_expression(foo.blobN, std::vector<uint8_t>{});

  // date
  test_assign_expression(date_time.dayPointN, ::sqlpp::chrono::day_point{});

  // timestamp
  test_assign_expression(date_time.timePointN, ::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_assign_expression(date_time.timePointN, minute_point{});

  // time_of_day
  test_assign_expression(date_time.timeOfDayN, std::chrono::microseconds{});
}


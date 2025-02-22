/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/tests/core/MockDb.h>
#include <sqlpp11/tests/core/tables.h>

namespace {
template <typename T>
using is_bool = std::is_same<sqlpp::value_type_of_t<T>, sqlpp::boolean>;

template <typename T>
using is_maybe_bool =
    std::is_same<sqlpp::value_type_of_t<T>, std::optional<sqlpp::boolean>>;
} // namespace

template <typename Value> void test_between_expression(Value v) {
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(std::make_optional(v));

  // Variations of nullable and non-nullable values
  static_assert(
      is_bool<decltype(between(v_not_null, v_not_null, v_not_null))>::value,
      "");
  static_assert(is_maybe_bool<decltype(between(v_not_null, v_not_null,
                                               v_maybe_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(between(v_not_null, v_maybe_null,
                                               v_not_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(between(v_not_null, v_maybe_null,
                                               v_maybe_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(between(v_maybe_null, v_not_null,
                                               v_not_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(between(v_maybe_null, v_not_null,
                                               v_maybe_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(between(v_maybe_null, v_maybe_null,
                                               v_not_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(between(v_maybe_null, v_maybe_null,
                                               v_maybe_null))>::value,
                "");

  // Between expressions have the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(between(v_not_null, v_not_null,
                                                       v_not_null))>::value,
                "");

  // Between expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(between(
                    v_not_null, v_not_null, v_not_null))>::value,
                "");

  // Between expressions have their arguments as nodes.
  using V = Value;
  using L = typename std::decay<decltype(v_not_null)>::type;
  using R = typename std::decay<decltype(v_maybe_null)>::type;
  static_assert(
      std::is_same<
          sqlpp::nodes_of_t<decltype(between(v, v_not_null, v_maybe_null))>,
          sqlpp::detail::type_vector<V, L, R>>::value,
      "");
}

void test_between_member_function() {
  auto foo = test::TabFoo{};
  auto bar = test::TabBar{};
  auto v_not_null = foo.id;
  auto v_maybe_null = foo.intN;

  // Variations of nullable and non-nullable values
  static_assert(
      is_bool<decltype(v_not_null.between(v_not_null, v_not_null))>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null.between(
                    v_not_null, v_maybe_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(v_not_null.between(v_maybe_null,
                                                          v_not_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(v_not_null.between(
                    v_maybe_null, v_maybe_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(v_maybe_null.between(
                    v_not_null, v_not_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(v_maybe_null.between(
                    v_not_null, v_maybe_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(v_maybe_null.between(
                    v_maybe_null, v_not_null))>::value,
                "");
  static_assert(is_maybe_bool<decltype(v_maybe_null.between(
                    v_maybe_null, v_maybe_null))>::value,
                "");

  // Between expressions have the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(v_not_null.between(
                    v_not_null, v_not_null))>::value,
                "");

  // Between expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(v_not_null.between(
                    v_not_null, v_not_null))>::value,
                "");

  // Between expressions have their arguments as nodes.
  auto v = bar.id;
  using V = decltype(v);
  using L = typename std::decay<decltype(v_not_null)>::type;
  using R = typename std::decay<decltype(v_maybe_null)>::type;
  static_assert(
      std::is_same<
          sqlpp::nodes_of_t<decltype(bar.id.between(v_not_null, v_maybe_null))>,
          sqlpp::detail::type_vector<V, L, R>>::value,
      "");
}

int main() {
  // boolean
  test_between_expression(bool{true});

  // integral
  test_between_expression(int8_t{7});
  test_between_expression(int16_t{7});
  test_between_expression(int32_t{7});
  test_between_expression(int64_t{7});

  // unsigned integral
  test_between_expression(uint8_t{7});
  test_between_expression(uint16_t{7});
  test_between_expression(uint32_t{7});
  test_between_expression(uint64_t{7});

  // floating point
  test_between_expression(float{7.7});
  test_between_expression(double{7.7});

  // text
  test_between_expression('7');
  test_between_expression("seven");
  test_between_expression(std::string("seven"));
  test_between_expression(std::string_view("seven"));

  // blob
  test_between_expression(std::vector<uint8_t>{});

  // date
  test_between_expression(::sqlpp::chrono::day_point{});

  // timestamp
  test_between_expression(::sqlpp::chrono::microsecond_point{});
  using minute_point =
      std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_between_expression(minute_point{});

  // time_of_day
  test_between_expression(std::chrono::microseconds{});

  test_between_member_function();
}

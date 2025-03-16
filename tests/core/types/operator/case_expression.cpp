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

#include <sqlpp23/sqlpp23.h>

template <typename T, typename Value>
using is_same_type =
    std::is_same<sqlpp::value_type_of_t<T>, sqlpp::value_type_of_t<Value>>;

template <typename Value>
void test_case_expression(Value v) {
  auto c_not_null = sqlpp::value(true);
  auto c_maybe_null = sqlpp::value(std::make_optional(false));

  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(std::make_optional(v));

  using ValueType = sqlpp::value_type_of_t<decltype(v_not_null)>;
  using OptValueType = sqlpp::value_type_of_t<decltype(v_maybe_null)>;

  // Variations of nullable and non-nullable values
  static_assert(
      is_same_type<
          decltype(case_when(c_not_null).then(v_not_null).else_(v_not_null)),
          ValueType>::value,
      "");
  static_assert(
      is_same_type<
          decltype(case_when(c_not_null).then(v_not_null).else_(v_maybe_null)),
          OptValueType>::value,
      "");
  static_assert(
      is_same_type<
          decltype(case_when(c_not_null).then(v_maybe_null).else_(v_not_null)),
          OptValueType>::value,
      "");
  static_assert(is_same_type<decltype(case_when(c_not_null)
                                          .then(v_maybe_null)
                                          .else_(v_maybe_null)),
                             OptValueType>::value,
                "");
  static_assert(
      is_same_type<
          decltype(case_when(c_maybe_null).then(v_not_null).else_(v_not_null)),
          OptValueType>::value,
      "");
  static_assert(is_same_type<decltype(case_when(c_maybe_null)
                                          .then(v_not_null)
                                          .else_(v_maybe_null)),
                             OptValueType>::value,
                "");
  static_assert(is_same_type<decltype(case_when(c_maybe_null)
                                          .then(v_maybe_null)
                                          .else_(v_not_null)),
                             OptValueType>::value,
                "");
  static_assert(is_same_type<decltype(case_when(c_maybe_null)
                                          .then(v_maybe_null)
                                          .else_(v_maybe_null)),
                             OptValueType>::value,
                "");

  // Incomplete case expressions have no value.
  static_assert(
      not sqlpp::has_value_type<decltype(case_when(c_not_null))>::value, "");
  static_assert(not sqlpp::has_value_type<
                    decltype(case_when(c_not_null).then(v_not_null))>::value,
                "");

  // Case expressions have the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(case_when(c_not_null)
                                                   .then(v_not_null)
                                                   .else_(v_not_null))>::value,
                "");

  // Case expressions enable comparison member functions.
  static_assert(
      sqlpp::has_enabled_comparison<decltype(case_when(c_not_null)
                                                 .then(v_not_null)
                                                 .else_(v_not_null))>::value,
      "");

  // Between expressions have their arguments as nodes.
  using L = typename std::decay<decltype(c_not_null)>::type;
  using M = typename std::decay<decltype(v_not_null)>::type;
  using R = typename std::decay<decltype(v_maybe_null)>::type;
  static_assert(
      std::is_same<sqlpp::nodes_of_t<decltype(case_when(c_not_null)
                                                  .then(v_not_null)
                                                  .else_(v_maybe_null))>,
                   sqlpp::detail::type_vector<L, M, R>>::value,
      "");
}

int main() {
  // boolean
  test_case_expression(bool{true});

  // integral
  test_case_expression(int8_t{7});
  test_case_expression(int16_t{7});
  test_case_expression(int32_t{7});
  test_case_expression(int64_t{7});

  // unsigned integral
  test_case_expression(uint8_t{7});
  test_case_expression(uint16_t{7});
  test_case_expression(uint32_t{7});
  test_case_expression(uint64_t{7});

  // floating point
  test_case_expression(float{7.7});
  test_case_expression(double{7.7});

  // text
  test_case_expression('7');
  test_case_expression("seven");
  test_case_expression(std::string("seven"));
  test_case_expression(std::string_view("seven"));

  // blob
  test_case_expression(std::vector<uint8_t>{});

  // date
  test_case_expression(::sqlpp::chrono::day_point{});

  // timestamp
  test_case_expression(::sqlpp::chrono::microsecond_point{});
  using minute_point =
      std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_case_expression(minute_point{});

  // time_of_day
  test_case_expression(std::chrono::microseconds{});
}

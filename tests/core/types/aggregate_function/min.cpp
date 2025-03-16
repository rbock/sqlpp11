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
#include <sqlpp23/tests/core/MockDb.h>
#include <sqlpp23/tests/core/tables.h>

namespace {
template <typename T, typename V>
using is_same_type = std::is_same<sqlpp::value_type_of_t<T>, V>;
}

template <typename Value>
void test_min(Value v) {
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(std::make_optional(v));

  using OptValueType = sqlpp::value_type_of_t<std::optional<Value>>;

  // min of non-nullable can be null because there could be zero result rows.
  static_assert(is_same_type<decltype(min(v_not_null)), OptValueType>::value,
                "");
  static_assert(is_same_type<decltype(min(sqlpp::distinct, v_not_null)),
                             OptValueType>::value,
                "");

  // min of nullable
  static_assert(is_same_type<decltype(min(v_maybe_null)), OptValueType>::value,
                "");
  static_assert(is_same_type<decltype(min(sqlpp::distinct, v_maybe_null)),
                             OptValueType>::value,
                "");

  // min enables the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(min(v_not_null))>::value, "");
  static_assert(
      sqlpp::has_enabled_as<decltype(sqlpp::distinct, min(v_not_null))>::value,
      "");

  // min has a name
  static_assert(not sqlpp::has_name_tag<decltype(min(v_not_null))>::value, "");
  static_assert(not sqlpp::has_name_tag<decltype(min(sqlpp::distinct,
                                                     v_not_null))>::value,
                "");

  // min enables comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(min(v_not_null))>::value,
                "");
  static_assert(sqlpp::has_enabled_comparison<decltype(min(sqlpp::distinct,
                                                           v_not_null))>::value,
                "");

  // min enables OVER.
  static_assert(sqlpp::has_enabled_over<decltype(min(v_not_null))>::value, "");
  static_assert(sqlpp::has_enabled_over<decltype(min(sqlpp::distinct,
                                                     v_not_null))>::value,
                "");

  // min has its argument as nodes
  using L = typename std::decay<decltype(v_not_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(min(v_not_null))>,
                             sqlpp::detail::type_vector<L>>::value,
                "");
  static_assert(
      std::is_same<
          sqlpp::nodes_of_t<decltype(min(sqlpp::distinct, v_not_null))>,
          sqlpp::detail::type_vector<L>>::value,
      "");
}

int main() {
  // boolean
  test_min(bool{true});

  // integral
  test_min(int8_t{7});
  test_min(int16_t{7});
  test_min(int32_t{7});
  test_min(int64_t{7});

  // unsigned integral
  test_min(uint8_t{7});
  test_min(uint16_t{7});
  test_min(uint32_t{7});
  test_min(uint64_t{7});

  // floating point
  test_min(float{7.7});
  test_min(double{7.7});

  // text
  test_min('7');
  test_min("seven");
  test_min(std::string("seven"));
  test_min(std::string_view("seven"));

  // blob
  test_min(std::vector<uint8_t>{});

  // date
  test_min(::sqlpp::chrono::day_point{});

  // timestamp
  test_min(::sqlpp::chrono::microsecond_point{});
  using minute_point =
      std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_min(minute_point{});

  // time_of_day
  test_min(std::chrono::microseconds{});
}

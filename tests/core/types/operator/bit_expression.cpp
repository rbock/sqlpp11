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

  template <typename T>
  using is_integral = std::is_same<sqlpp::value_type_of_t<T>, sqlpp::integral>;

  template <typename T>
  using is_maybe_integral = std::is_same<sqlpp::value_type_of_t<T>, ::sqlpp::optional<sqlpp::integral>>;
}

template <typename Value>
void test_bit_expression(Value v)
{
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(::sqlpp::make_optional(v));

  // Compare non-nullable with non-nullable.
  static_assert(is_integral<decltype(v_not_null << v_not_null)>::value, "");
  static_assert(is_integral<decltype(v_not_null >> v_not_null)>::value, "");
  static_assert(is_integral<decltype(v_not_null | v_not_null)>::value, "");
  static_assert(is_integral<decltype(v_not_null & v_not_null)>::value, "");
  static_assert(is_integral<decltype(v_not_null ^ v_not_null)>::value, "");

  // Compare non-nullable with nullable.
  static_assert(is_maybe_integral<decltype(v_not_null << v_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_not_null >> v_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_not_null | v_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_not_null & v_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_not_null ^ v_maybe_null)>::value, "");

  // Compare nullable with non-nullable.
  static_assert(is_maybe_integral<decltype(v_maybe_null << v_not_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_maybe_null >> v_not_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_maybe_null | v_not_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_maybe_null & v_not_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_maybe_null ^ v_not_null)>::value, "");

  // Compare nullable with nullable.
  static_assert(is_maybe_integral<decltype(v_maybe_null << v_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_maybe_null >> v_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_maybe_null | v_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_maybe_null & v_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(v_maybe_null ^ v_maybe_null)>::value, "");

  // Compare with null.
  static_assert(is_integral<decltype(~v_not_null)>::value, "");
  static_assert(is_maybe_integral<decltype(~v_maybe_null)>::value, "");

  // Comparison expressions have the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(v_not_null << v_maybe_null)>::value, "");
  static_assert(sqlpp::has_enabled_as<decltype(~v_not_null)>::value, "");

  // Comparison expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(v_not_null << v_maybe_null)>::value, "");

  // Comparison expressions have their arguments as nodes.
  using L = typename std::decay<decltype(v_not_null)>::type;
  using R = typename std::decay<decltype(v_maybe_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(v_not_null << v_maybe_null)>, sqlpp::detail::type_vector<L, R>>::value, "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(~v_not_null)>, sqlpp::detail::type_vector<sqlpp::noop, L>>::value, "");
}

template <typename Left, typename Right>
void test_bit_shift_expression(Left l, Right r)
{
  auto l_not_null = sqlpp::value(l);
  auto l_maybe_null = sqlpp::value(::sqlpp::make_optional(l));
  auto r_not_null = sqlpp::value(r);
  auto r_maybe_null = sqlpp::value(::sqlpp::make_optional(r));

  // Compare non-nullable with non-nullable.
  static_assert(is_integral<decltype(l_not_null << r_not_null)>::value, "");
  static_assert(is_integral<decltype(l_not_null >> r_not_null)>::value, "");

  // Compare non-nullable with nullable.
  static_assert(is_maybe_integral<decltype(l_not_null << r_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(l_not_null >> r_maybe_null)>::value, "");

  // Compare nullable with non-nullable.
  static_assert(is_maybe_integral<decltype(l_maybe_null << r_not_null)>::value, "");
  static_assert(is_maybe_integral<decltype(l_maybe_null >> r_not_null)>::value, "");

  // Compare nullable with nullable.
  static_assert(is_maybe_integral<decltype(l_maybe_null << r_maybe_null)>::value, "");
  static_assert(is_maybe_integral<decltype(l_maybe_null >> r_maybe_null)>::value, "");

  // Comparison expressions have the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(l_not_null << r_maybe_null)>::value, "");

  // Comparison expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(l_not_null << r_maybe_null)>::value, "");

  // Comparison expressions have their arguments as nodes.
  using L = typename std::decay<decltype(l_not_null)>::type;
  using R = typename std::decay<decltype(r_maybe_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(l_not_null << r_maybe_null)>, sqlpp::detail::type_vector<L, R>>::value, "");
}


int main()
{
  // bit expression require integral operands
  test_bit_expression(int8_t{7});
  test_bit_expression(int16_t{7});
  test_bit_expression(int32_t{7});
  test_bit_expression(int64_t{7});

  // bit shift operations can have unsigned rhs operands
  test_bit_shift_expression(int8_t{7}, uint8_t{7});
  test_bit_shift_expression(int8_t{7}, uint16_t{7});
  test_bit_shift_expression(int8_t{7}, uint32_t{7});
  test_bit_shift_expression(int8_t{7}, uint64_t{7});
}


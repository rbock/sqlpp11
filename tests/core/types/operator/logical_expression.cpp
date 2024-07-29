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

namespace
{
  template <typename T>
  using is_bool = std::is_same<sqlpp::value_type_of_t<T>, sqlpp::boolean>;

  template <typename T>
  using is_maybe_bool = std::is_same<sqlpp::value_type_of_t<T>, ::sqlpp::optional<sqlpp::boolean>>;
}

template<typename Value>
void test_logical_expression(Value v)
{
  auto v_not_null= sqlpp::value(v);
  auto v_maybe_null= sqlpp::value(::sqlpp::make_optional(v));

  // Combine non-nullable with non-nullable.
  static_assert(is_bool<decltype(v_not_null and v_not_null)>::value, "");
  static_assert(is_bool<decltype(v_not_null or v_not_null)>::value, "");

  static_assert(is_bool<decltype(v_not_null and dynamic(true, v_not_null))>::value, "");
  static_assert(is_bool<decltype(v_not_null or dynamic(true, v_not_null))>::value, "");

  // Combine nullable with non-nullable.
  static_assert(is_maybe_bool<decltype(v_maybe_null and v_not_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null or v_not_null)>::value, "");

  static_assert(is_maybe_bool<decltype(v_maybe_null and dynamic(true, v_not_null))>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null or dynamic(true, v_not_null))>::value, "");

  // Combine non-nullable with nullable.
  static_assert(is_maybe_bool<decltype(v_not_null and v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null or v_maybe_null)>::value, "");

  static_assert(is_maybe_bool<decltype(v_not_null and dynamic(true, v_maybe_null))>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null or dynamic(true, v_maybe_null))>::value, "");

  // Combine nullable with nullable.
  static_assert(is_maybe_bool<decltype(v_maybe_null and v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null or v_maybe_null)>::value, "");

  static_assert(is_maybe_bool<decltype(v_maybe_null and dynamic(true, v_maybe_null))>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null or dynamic(true, v_maybe_null))>::value, "");

  // not.
  static_assert(is_bool<decltype(not(v_not_null))>::value, "");
  static_assert(is_maybe_bool<decltype(not(v_maybe_null))>::value, "");

  // Logical expressions have the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(v_not_null and v_maybe_null)>::value, "");
  static_assert(sqlpp::has_enabled_as<decltype(v_maybe_null or dynamic(true, v_maybe_null))>::value, "");

  // Logical expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(v_not_null == v_maybe_null)>::value, "");
  static_assert(not sqlpp::has_enabled_comparison<decltype(v_maybe_null or dynamic(true, v_maybe_null))>::value, "");

  // Logical expressions have their arguments as nodes.
  using L = typename std::decay<decltype(v_not_null)>::type;
  using R = typename std::decay<decltype(v_maybe_null)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(v_not_null and v_maybe_null)>, sqlpp::detail::type_vector<L, R>>::value, "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(v_not_null and dynamic(true, v_maybe_null))>, sqlpp::detail::type_vector<L, sqlpp::dynamic_t<R>>>::value, "");
}

int main()
{
  // boolean
  test_logical_expression(bool{true});
}


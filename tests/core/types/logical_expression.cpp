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

#include <sqlpp11/sqlpp11.h>

namespace
{
  template <typename T>
  using is_bool = std::is_same<sqlpp::value_type_of_t<T>, sqlpp::boolean>;

  template <typename T>
  using is_maybe_bool = std::is_same<sqlpp::value_type_of_t<T>, sqlpp::compat::optional<sqlpp::boolean>>;
}

template<typename Value>
void test_logical_expression(Value v)
{
  auto v_not_null= sqlpp::value(v);
  auto v_maybe_null= sqlpp::value(sqlpp::compat::make_optional(v));

  // Compare non-nullable with non-nullable.
  static_assert(is_bool<decltype(v_not_null and v_not_null)>::value, "");
  static_assert(is_bool<decltype(v_not_null or v_not_null)>::value, "");

  static_assert(is_bool<decltype(v_not_null and dynamic(true, v_not_null))>::value, "");
  static_assert(is_bool<decltype(v_not_null or dynamic(true, v_not_null))>::value, "");

  // Compare nullable with non-nullable.
  static_assert(is_maybe_bool<decltype(v_maybe_null and v_not_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null or v_not_null)>::value, "");

  static_assert(is_maybe_bool<decltype(v_maybe_null and dynamic(true, v_not_null))>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null or dynamic(true, v_not_null))>::value, "");

  // Compare non-nullable with nullable.
  static_assert(is_maybe_bool<decltype(v_not_null and v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null or v_maybe_null)>::value, "");

  static_assert(is_maybe_bool<decltype(v_not_null and dynamic(true, v_maybe_null))>::value, "");
  static_assert(is_maybe_bool<decltype(v_not_null or dynamic(true, v_maybe_null))>::value, "");

  // Compare nullable with nullable.
  static_assert(is_maybe_bool<decltype(v_maybe_null and v_maybe_null)>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null or v_maybe_null)>::value, "");

  static_assert(is_maybe_bool<decltype(v_maybe_null and dynamic(true, v_maybe_null))>::value, "");
  static_assert(is_maybe_bool<decltype(v_maybe_null or dynamic(true, v_maybe_null))>::value, "");

  // not.
  static_assert(is_bool<decltype(not(v_not_null))>::value, "");
  static_assert(is_maybe_bool<decltype(not(v_maybe_null))>::value, "");

#warning: test can be aliased
#warning: test has comparison operators
#warning: test nodes
}

int main()
{
  // boolean
  test_logical_expression(bool{true});
}


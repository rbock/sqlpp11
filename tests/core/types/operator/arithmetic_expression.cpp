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

#include "../../include/test_helpers.h"

namespace
{
  template<typename A, typename B>
  constexpr bool is_same_type()
  {
    return std::is_same<A, B>::value;
  }
}

SQLPP_ALIAS_PROVIDER(r_not_null);
SQLPP_ALIAS_PROVIDER(r_maybe_null);
SQLPP_ALIAS_PROVIDER(r_opt_not_null);
SQLPP_ALIAS_PROVIDER(r_opt_maybe_null);

template<typename Value>
void test_arithmetic_expressions(Value v)
{
  using ValueType = sqlpp::numeric;
  using OptValueType = ::sqlpp::optional<sqlpp::numeric>;

  auto value = sqlpp::value(v);
  auto opt_value = sqlpp::value(::sqlpp::make_optional(v));

  // Arithmetically combining non-optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value + value)>, ValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value - value)>, ValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value * value)>, ValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value / value)>, ValueType>(), "");

  // Arithmetically combining non-optional with optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value + opt_value)>, OptValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value - opt_value)>, OptValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value * opt_value)>, OptValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value / opt_value)>, OptValueType>(), "");

  // Arithmetically combining optional with non-optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value + value)>, OptValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value - value)>, OptValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value * value)>, OptValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value / value)>, OptValueType>(), "");

  // Arithmetically combining optional with optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value + opt_value)>, OptValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value - opt_value)>, OptValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value * opt_value)>, OptValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value / opt_value)>, OptValueType>(), "");

  // Same with negate.
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(-value)>, ValueType>(), "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(-opt_value)>, OptValueType>(), "");

  // Arithmetic expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(value + opt_value)>::value, "");
  static_assert(sqlpp::has_enabled_as<decltype(-opt_value)>::value, "");

  // Arithmetic expressions enable comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(-opt_value)>::value, "");

  // Arithmetic expressions have their arguments as nodes
  using L = typename std::decay<decltype(value)>::type;
  using R = typename std::decay<decltype(opt_value)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(value + opt_value)>, sqlpp::detail::type_vector<L, R>>::value, "");
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(-opt_value)>, sqlpp::detail::type_vector<sqlpp::noop, R>>::value, "");
}

template<typename Value>
void test_modulus_expressions(Value v)
{
  using ValueType = sqlpp::numeric;
  using OptValueType = ::sqlpp::optional<sqlpp::numeric>;

  auto value = sqlpp::value(v);
  auto opt_value = sqlpp::value(::sqlpp::make_optional(v));

  // Modulus combining non-optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value % value)>, ValueType>(), "");

  // Modulus combining non-optional with optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value % opt_value)>, OptValueType>(), "");

  // Modulus combining optional with non-optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value % value)>, OptValueType>(), "");

  // Modulus combining optional with optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value % opt_value)>, OptValueType>(), "");

  // Modulus expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(value % opt_value)>::value, "");

  // Modulus expressions enable comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(value % opt_value)>::value, "");

  // Modulus expressions have their arguments as nodes
  using L = typename std::decay<decltype(value)>::type;
  using R = typename std::decay<decltype(opt_value)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(value % opt_value)>, sqlpp::detail::type_vector<L, R>>::value, "");
}

template<typename Value>
void test_concatenation_expressions(Value v)
{
  using ValueType = sqlpp::text;
  using OptValueType = ::sqlpp::optional<sqlpp::text>;

  auto value = sqlpp::value(v);
  auto opt_value = sqlpp::value(::sqlpp::make_optional(v));

  // Concatenating non-optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value + value)>, ValueType>(), "");

  // Concatenating non-optional with optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value + opt_value)>, OptValueType>(), "");

  // Concatenating optional with non-optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value + value)>, OptValueType>(), "");

  // Concatenating optional with optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_value + opt_value)>, OptValueType>(), "");

  // Modulus expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(value + opt_value)>::value, "");

  // Modulus expressions enable comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(value + opt_value)>::value, "");

  // Modulus expressions have their arguments as nodes
  using L = typename std::decay<decltype(value)>::type;
  using R = typename std::decay<decltype(opt_value)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(value + opt_value)>, sqlpp::detail::type_vector<L, R>>::value, "");
}

int main()
{
 // integral
  test_arithmetic_expressions(int8_t{7});
  test_arithmetic_expressions(int16_t{7});
  test_arithmetic_expressions(int32_t{7});
  test_arithmetic_expressions(int64_t{7});
  test_modulus_expressions(int8_t{7});
  test_modulus_expressions(int16_t{7});
  test_modulus_expressions(int32_t{7});
  test_modulus_expressions(int64_t{7});

  // unsigned integral
  test_arithmetic_expressions(uint8_t{7});
  test_arithmetic_expressions(uint16_t{7});
  test_arithmetic_expressions(uint32_t{7});
  test_arithmetic_expressions(uint64_t{7});
  test_modulus_expressions(uint8_t{7});
  test_modulus_expressions(uint16_t{7});
  test_modulus_expressions(uint32_t{7});
  test_modulus_expressions(uint64_t{7});

  // floating point
  test_arithmetic_expressions(float{7.7});
  test_arithmetic_expressions(double{7.7});

  // text
  test_concatenation_expressions('7');
  test_concatenation_expressions("seven");
  test_concatenation_expressions(std::string("seven"));
  test_concatenation_expressions(::sqlpp::string_view("seven"));
}

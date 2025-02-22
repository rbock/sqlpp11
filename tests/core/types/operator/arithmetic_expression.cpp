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

#include <sqlpp11/tests/core/types_helpers.h>

namespace {
template <typename A, typename B> constexpr bool is_same_type() {
  return std::is_same<A, B>::value;
}
} // namespace

template <typename Left, typename Right, typename ValueType>
void test_plus(Left raw_l, Right raw_r, ValueType) {
  using OptValueType = std::optional<ValueType>;

  auto l = sqlpp::value(raw_l);
  auto r = sqlpp::value(raw_r);

  auto opt_l = sqlpp::value(std::make_optional(raw_l));
  auto opt_r = sqlpp::value(std::make_optional(raw_r));

  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l + r)>, ValueType>(), "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l + opt_r)>, OptValueType>(),
      "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(opt_l + r)>, OptValueType>(),
      "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_l + opt_r)>,
                             OptValueType>(),
                "");

  // Arithmetic expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(l + opt_r)>::value, "");

  // Arithmetic expressions enable comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(l + opt_r)>::value, "");

  // Arithmetic expressions have their arguments as nodes
  using L = typename std::decay<decltype(l)>::type;
  using R = typename std::decay<decltype(opt_r)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(l + opt_r)>,
                             sqlpp::detail::type_vector<L, R>>::value,
                "");
}

template <typename Left, typename Right, typename ValueType>
void test_minus(Left raw_l, Right raw_r, ValueType) {
  using OptValueType = std::optional<ValueType>;

  auto l = sqlpp::value(raw_l);
  auto r = sqlpp::value(raw_r);

  auto opt_l = sqlpp::value(std::make_optional(raw_l));
  auto opt_r = sqlpp::value(std::make_optional(raw_r));

  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l - r)>, ValueType>(), "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l - opt_r)>, OptValueType>(),
      "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(opt_l - r)>, OptValueType>(),
      "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_l - opt_r)>,
                             OptValueType>(),
                "");

  // Arithmetic expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(l - opt_r)>::value, "");

  // Arithmetic expressions enable comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(l - opt_r)>::value, "");

  // Arithmetic expressions have their arguments as nodes
  using L = typename std::decay<decltype(l)>::type;
  using R = typename std::decay<decltype(opt_r)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(l - opt_r)>,
                             sqlpp::detail::type_vector<L, R>>::value,
                "");
}

template <typename Left, typename Right, typename ValueType>
void test_multiplies(Left raw_l, Right raw_r, ValueType) {
  using OptValueType = std::optional<ValueType>;

  auto l = sqlpp::value(raw_l);
  auto r = sqlpp::value(raw_r);

  auto opt_l = sqlpp::value(std::make_optional(raw_l));
  auto opt_r = sqlpp::value(std::make_optional(raw_r));

  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l * r)>, ValueType>(), "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l * opt_r)>, OptValueType>(),
      "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(opt_l * r)>, OptValueType>(),
      "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_l * opt_r)>,
                             OptValueType>(),
                "");

  // Arithmetic expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(l * opt_r)>::value, "");

  // Arithmetic expressions enable comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(l * opt_r)>::value, "");

  // Arithmetic expressions have their arguments as nodes
  using L = typename std::decay<decltype(l)>::type;
  using R = typename std::decay<decltype(opt_r)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(l * opt_r)>,
                             sqlpp::detail::type_vector<L, R>>::value,
                "");
}

template <typename Right, typename ValueType>
void test_negate(Right raw_r, ValueType) {
  using OptValueType = std::optional<ValueType>;

  auto r = sqlpp::value(raw_r);

  auto opt_r = sqlpp::value(std::make_optional(raw_r));

  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(-r)>, ValueType>(),
                "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(-opt_r)>, OptValueType>(),
      "");

  // Arithmetic expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(-opt_r)>::value, "");

  // Arithmetic expressions enable comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(-opt_r)>::value, "");

  // Arithmetic expressions have their arguments as nodes
  using R = typename std::decay<decltype(opt_r)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(-opt_r)>,
                             sqlpp::detail::type_vector<sqlpp::noop, R>>::value,
                "");
}

template <typename Left, typename Right, typename ValueType>
void test_divides(Left raw_l, Right raw_r, ValueType) {
  using OptValueType = std::optional<ValueType>;

  auto l = sqlpp::value(raw_l);
  auto r = sqlpp::value(raw_r);

  auto opt_l = sqlpp::value(std::make_optional(raw_l));
  auto opt_r = sqlpp::value(std::make_optional(raw_r));

  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l / r)>, ValueType>(), "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l / opt_r)>, OptValueType>(),
      "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(opt_l / r)>, OptValueType>(),
      "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_l / opt_r)>,
                             OptValueType>(),
                "");

  // Arithmetic expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(l / opt_r)>::value, "");

  // Arithmetic expressions enable comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(l / opt_r)>::value, "");

  // Arithmetic expressions have their arguments as nodes
  using L = typename std::decay<decltype(l)>::type;
  using R = typename std::decay<decltype(opt_r)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(l / opt_r)>,
                             sqlpp::detail::type_vector<L, R>>::value,
                "");
}

template <typename Left, typename Right, typename ValueType>
void test_modulus(Left raw_l, Right raw_r, ValueType) {
  using OptValueType = std::optional<ValueType>;

  auto l = sqlpp::value(raw_l);
  auto r = sqlpp::value(raw_r);

  auto opt_l = sqlpp::value(std::make_optional(raw_l));
  auto opt_r = sqlpp::value(std::make_optional(raw_r));

  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l % r)>, ValueType>(), "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(l % opt_r)>, OptValueType>(),
      "");
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(opt_l % r)>, OptValueType>(),
      "");
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(opt_l % opt_r)>,
                             OptValueType>(),
                "");

  // Arithmetic expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(l % opt_r)>::value, "");

  // Arithmetic expressions enable comparison member functions.
  static_assert(sqlpp::has_enabled_comparison<decltype(l % opt_r)>::value, "");

  // Arithmetic expressions have their arguments as nodes
  using L = typename std::decay<decltype(l)>::type;
  using R = typename std::decay<decltype(opt_r)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(l % opt_r)>,
                             sqlpp::detail::type_vector<L, R>>::value,
                "");
}

template <typename Value> void test_concatenation_expressions(Value v) {
  using ValueType = sqlpp::text;
  using OptValueType = std::optional<sqlpp::text>;

  auto value = sqlpp::value(v);
  auto opt_value = sqlpp::value(std::make_optional(v));

  // Concatenating non-optional values
  static_assert(is_same_type<sqlpp::value_type_of_t<decltype(value + value)>,
                             ValueType>(),
                "");

  // Concatenating non-optional with optional values
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(value + opt_value)>,
                   OptValueType>(),
      "");

  // Concatenating optional with non-optional values
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(opt_value + value)>,
                   OptValueType>(),
      "");

  // Concatenating optional with optional values
  static_assert(
      is_same_type<sqlpp::value_type_of_t<decltype(opt_value + opt_value)>,
                   OptValueType>(),
      "");

  // Modulus expressions enable the `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(value + opt_value)>::value, "");

  // Modulus expressions enable comparison member functions.
  static_assert(
      sqlpp::has_enabled_comparison<decltype(value + opt_value)>::value, "");

  // Modulus expressions have their arguments as nodes
  using L = typename std::decay<decltype(value)>::type;
  using R = typename std::decay<decltype(opt_value)>::type;
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(value + opt_value)>,
                             sqlpp::detail::type_vector<L, R>>::value,
                "");
}

int main() {
  auto fp = float{7};
  auto in = int{7};
  auto ui = unsigned{7};
  auto bo = bool{1};

  // plus
  test_plus(fp, fp, sqlpp::floating_point{});
  test_plus(fp, in, sqlpp::floating_point{});
  test_plus(fp, ui, sqlpp::floating_point{});
  test_plus(fp, bo, sqlpp::floating_point{});

  test_plus(in, fp, sqlpp::floating_point{});
  test_plus(in, in, sqlpp::integral{});
  test_plus(in, ui, sqlpp::integral{});
  test_plus(in, bo, sqlpp::integral{});

  test_plus(ui, fp, sqlpp::floating_point{});
  test_plus(ui, in, sqlpp::integral{});
  test_plus(ui, ui, sqlpp::unsigned_integral{});
  test_plus(ui, bo, sqlpp::unsigned_integral{});

  test_plus(bo, fp, sqlpp::floating_point{});
  test_plus(bo, in, sqlpp::integral{});
  test_plus(bo, ui, sqlpp::unsigned_integral{});
  test_plus(bo, bo, sqlpp::unsigned_integral{});

  // minus
  test_minus(fp, fp, sqlpp::floating_point{});
  test_minus(fp, in, sqlpp::floating_point{});
  test_minus(fp, ui, sqlpp::floating_point{});
  test_minus(fp, bo, sqlpp::floating_point{});

  test_minus(in, fp, sqlpp::floating_point{});
  test_minus(in, in, sqlpp::integral{});
  test_minus(in, ui, sqlpp::integral{});
  test_minus(in, bo, sqlpp::integral{});

  test_minus(ui, fp, sqlpp::floating_point{});
  test_minus(ui, in, sqlpp::integral{});
  test_minus(ui, ui, sqlpp::integral{});
  test_minus(ui, bo, sqlpp::integral{});

  test_minus(bo, fp, sqlpp::floating_point{});
  test_minus(bo, in, sqlpp::integral{});
  test_minus(bo, ui, sqlpp::integral{});
  test_minus(bo, bo, sqlpp::integral{});

  // multiplies
  test_multiplies(fp, fp, sqlpp::floating_point{});
  test_multiplies(fp, in, sqlpp::floating_point{});
  test_multiplies(fp, ui, sqlpp::floating_point{});
  test_multiplies(fp, bo, sqlpp::floating_point{});

  test_multiplies(in, fp, sqlpp::floating_point{});
  test_multiplies(in, in, sqlpp::integral{});
  test_multiplies(in, ui, sqlpp::integral{});
  test_multiplies(in, bo, sqlpp::integral{});

  test_multiplies(ui, fp, sqlpp::floating_point{});
  test_multiplies(ui, in, sqlpp::integral{});
  test_multiplies(ui, ui, sqlpp::unsigned_integral{});
  test_multiplies(ui, bo, sqlpp::unsigned_integral{});

  test_multiplies(bo, fp, sqlpp::floating_point{});
  test_multiplies(bo, in, sqlpp::integral{});
  test_multiplies(bo, ui, sqlpp::unsigned_integral{});
  test_multiplies(bo, bo, sqlpp::boolean{});

  // divides
  test_divides(fp, fp, sqlpp::floating_point{});
  test_divides(fp, in, sqlpp::floating_point{});
  test_divides(fp, ui, sqlpp::floating_point{});
  test_divides(fp, bo, sqlpp::floating_point{});

  test_divides(in, fp, sqlpp::floating_point{});
  test_divides(in, in, sqlpp::floating_point{});
  test_divides(in, ui, sqlpp::floating_point{});
  test_divides(in, bo, sqlpp::floating_point{});

  test_divides(ui, fp, sqlpp::floating_point{});
  test_divides(ui, in, sqlpp::floating_point{});
  test_divides(ui, ui, sqlpp::floating_point{});
  test_divides(ui, bo, sqlpp::floating_point{});

  test_divides(bo, fp, sqlpp::floating_point{});
  test_divides(bo, in, sqlpp::floating_point{});
  test_divides(bo, ui, sqlpp::floating_point{});
  test_divides(bo, bo, sqlpp::floating_point{});

  // negate
  test_negate(fp, sqlpp::floating_point{});
  test_negate(in, sqlpp::integral{});
  test_negate(ui, sqlpp::integral{});
  test_negate(bo, sqlpp::integral{});

  // modulus
  test_modulus(in, in, sqlpp::unsigned_integral{});
  test_modulus(in, ui, sqlpp::unsigned_integral{});

  test_modulus(ui, in, sqlpp::unsigned_integral{});
  test_modulus(ui, ui, sqlpp::unsigned_integral{});

  // concatenation
  test_concatenation_expressions('7');
  test_concatenation_expressions("seven");
  test_concatenation_expressions(std::string("seven"));
  test_concatenation_expressions(std::string_view("seven"));
}

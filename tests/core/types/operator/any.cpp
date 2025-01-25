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

#include <sqlpp11/tests/core/tables.h>

SQLPP_CREATE_NAME_TAG(r_not_null);
SQLPP_CREATE_NAME_TAG(r_maybe_null);

template <typename Value>
void test_any(Value v)
{
  using ValueType = sqlpp::value_type_of_t<Value>;
  using OptValueType = sqlpp::value_type_of_t<::sqlpp::optional<Value>>;

  // Selectable values.
  const auto v_not_null = sqlpp::value(v).as(r_not_null);
  const auto v_maybe_null = sqlpp::value(::sqlpp::make_optional(v)).as(r_maybe_null);

  // ANY expression are not to be in most expressions and therefore have no value defined.
  static_assert(not sqlpp::has_value_type<decltype(any(select(v_not_null)))>::value, "");
  static_assert(not sqlpp::has_value_type<decltype(any(select(v_maybe_null)))>::value, "");

  // ANY expression can be used in basic comparison expressions, which use remove_any_t to look inside.
  static_assert(
      std::is_same<sqlpp::value_type_of_t<sqlpp::remove_any_t<decltype(any(select(v_not_null)))>>, ValueType>::value,
      "");
  static_assert(std::is_same<sqlpp::value_type_of_t<sqlpp::remove_any_t<decltype(any(select(v_maybe_null)))>>,
                             OptValueType>::value,
                "");

  // ANY expressions do not have `as` member function.
  static_assert(not sqlpp::has_enabled_as<decltype(any(select(v_not_null)))>::value, "");

  // ANY expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(any(select(v_not_null)))>::value, "");

  // ANY expressions have the SELECT as node.
  using S = decltype(select(v_not_null));
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(any(select(v_not_null)))>, sqlpp::detail::type_vector<S>>::value, "");

  static_assert(not sqlpp::requires_parentheses<decltype(any(select(v_not_null)))>::value, "");
}

void test_any_sub_select()
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Use a select that depends on a table that would need to be provided by the enclosing query.
  auto s = select(foo.id).from(foo).where(foo.id == bar.id);
  auto a = any(s);

  using S = decltype(s);
  using A = decltype(a);

  static_assert(std::is_same<sqlpp::required_tables_of_t<A>, sqlpp::required_tables_of_t<S>>::value, "");
}

int main()
{
  // boolean
  test_any(bool{true});

  // integral
  test_any(int8_t{7});
  test_any(int16_t{7});
  test_any(int32_t{7});
  test_any(int64_t{7});

  // unsigned integral
  test_any(uint8_t{7});
  test_any(uint16_t{7});
  test_any(uint32_t{7});
  test_any(uint64_t{7});

  // floating point
  test_any(float{7.7});
  test_any(double{7.7});

  // text
  test_any('7');
  test_any("seven");
  test_any(std::string("seven"));
  test_any(::sqlpp::string_view("seven"));

  // blob
  test_any(std::vector<uint8_t>{});

  // date
  test_any(::sqlpp::chrono::day_point{});

  // timestamp
  test_any(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_any(minute_point{});

  // time_of_day
  test_any(std::chrono::microseconds{});

  test_any_sub_select();
}


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

#warning: implement serialize instead of type tests here!
SQLPP_ALIAS_PROVIDER(r_not_null);
SQLPP_ALIAS_PROVIDER(r_maybe_null);

template <typename Value>
void test_exists(Value v)
{
  // Selectable values.
  const auto v_not_null = sqlpp::value(v).as(r_not_null);
  const auto v_maybe_null = sqlpp::value(::sqlpp::make_optional(v)).as(r_maybe_null);

  // EXISTS expression can be used in basic comparison expressions, which use remove_exists_t to look inside.
  static_assert(std::is_same<sqlpp::value_type_of_t<decltype(exists(select(v_not_null)))>, sqlpp::boolean>::value, "");
  static_assert(std::is_same<sqlpp::value_type_of_t<decltype(exists(select(v_maybe_null)))>, sqlpp::boolean>::value, "");

  // EXISTS expressions enable `as` member function.
  static_assert(sqlpp::has_enabled_as<decltype(exists(select(v_not_null)))>::value, "");

  // EXISTS expressions do not enable comparison member functions.
  static_assert(not sqlpp::has_enabled_comparison<decltype(exists(select(v_not_null)))>::value, "");

  // EXISTS expressions have the SELECT as node.
  using S = decltype(select(v_not_null));
  static_assert(std::is_same<sqlpp::nodes_of_t<decltype(exists(select(v_not_null)))>, sqlpp::detail::type_vector<S>>::value, "");

#warning: Note that a sub select may require tables from the enclosing select. This is currently not correctly implemented. We need to test that.
}

int main()
{
  // boolean
  test_exists(bool{true});

  // integral
  test_exists(int8_t{7});
  test_exists(int16_t{7});
  test_exists(int32_t{7});
  test_exists(int64_t{7});

  // unsigned integral
  test_exists(uint8_t{7});
  test_exists(uint16_t{7});
  test_exists(uint32_t{7});
  test_exists(uint64_t{7});

  // floating point
  test_exists(float{7.7});
  test_exists(double{7.7});

  // text
  test_exists('7');
  test_exists("seven");
  test_exists(std::string("seven"));
  test_exists(::sqlpp::string_view("seven"));

  // blob
  test_exists(std::vector<uint8_t>{});

  // date
  test_exists(::sqlpp::chrono::day_point{});

  // timestamp
  test_exists(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_exists(minute_point{});

  // time_of_day
  test_exists(std::chrono::microseconds{});
}


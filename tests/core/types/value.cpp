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

SQLPP_CREATE_NAME_TAG(r_not_null);
SQLPP_CREATE_NAME_TAG(r_maybe_null);
SQLPP_CREATE_NAME_TAG(r_opt_not_null);
SQLPP_CREATE_NAME_TAG(r_opt_maybe_null);

template <typename T, typename ValueType>
using is_value_type = std::is_same<sqlpp::value_type_of_t<T>, ValueType>;

template<typename Value>
void test_value(Value v)
{
  using ValueType = sqlpp::value_type_of_t<Value>;
  using OptValueType = ::sqlpp::optional<ValueType>;

  auto v_not_null= sqlpp::value(v);
  auto v_maybe_null= sqlpp::value(::sqlpp::make_optional(v));

  static_assert(is_value_type<decltype(v_not_null), ValueType>::value, "");
  static_assert(is_value_type<decltype(v_maybe_null), OptValueType>::value, "");

  static_assert(not sqlpp::can_be_null<decltype(v_not_null)>::value, "");
  static_assert(sqlpp::can_be_null<decltype(v_maybe_null)>::value, "");

#warning: test can be aliased
#warning: test has comparison operators
#warning: test nodes
}

int main()
{
  // boolean
  test_value(bool{true});

  // integral
  test_value(int8_t{7});
  test_value(int16_t{7});
  test_value(int32_t{7});
  test_value(int64_t{7});

  // unsigned integral
  test_value(uint8_t{7});
  test_value(uint16_t{7});
  test_value(uint32_t{7});
  test_value(uint64_t{7});

  // floating point
  test_value(float{7.7});
  test_value(double{7.7});

  // text
  test_value('7');
  test_value("seven");
  test_value(std::string("seven"));
  test_value(::sqlpp::string_view("seven"));

  // blob
  test_value(std::vector<uint8_t>{});

  // date
  test_value(::sqlpp::chrono::day_point{});

  // timestamp
  test_value(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_value(minute_point{});

  // time_of_day
  test_value(std::chrono::microseconds{});

}


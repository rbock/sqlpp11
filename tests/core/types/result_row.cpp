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

#include <sqlpp11/tests/core/MockDb.h>
#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/sqlpp11.h>

#include <sqlpp11/tests/core/types_helpers.h>

namespace
{
  template<typename A, typename B>
  constexpr bool is_same_type()
  {
    return std::is_same<A, B>::value;
  }

  constexpr auto bar = test::TabBar{};
  constexpr auto foo = test::TabFoo{};
  auto db = MockDb{};
}

SQLPP_CREATE_NAME_TAG(r_not_null);
SQLPP_CREATE_NAME_TAG(r_maybe_null);
SQLPP_CREATE_NAME_TAG(r_opt_not_null);
SQLPP_CREATE_NAME_TAG(r_opt_maybe_null);

template<typename ResultType, typename Value>
void test_result_row(Value v)
{
  using OptResultType = ::sqlpp::optional<ResultType>;

  // Selectable values.
  auto v_not_null = sqlpp::value(v).as(r_not_null);
  const auto v_maybe_null = sqlpp::value(::sqlpp::make_optional(v)).as(r_maybe_null);

  // Dynamically selectable values.
  const auto v_opt_not_null = dynamic(true, sqlpp::value(v)).as(r_opt_not_null);
  const auto v_opt_maybe_null = dynamic(true, sqlpp::value(::sqlpp::make_optional(v))).as(r_opt_maybe_null);

  auto s = select(v_not_null, v_maybe_null, v_opt_not_null, v_opt_maybe_null);
  using S = decltype(s);
  static_assert(std::is_same<sqlpp::run_check_t<typename MockDb::_context_t, S>, sqlpp::consistent_t>::value,
                "");
  for (const auto& row : db(select(v_not_null, v_maybe_null, v_opt_not_null, v_opt_maybe_null)))
  {
    static_assert(std::is_same<decltype(row.r_not_null), ResultType>::value, "");
    static_assert(std::is_same<decltype(row.r_maybe_null), OptResultType>::value, "");
    static_assert(std::is_same<decltype(row.r_opt_not_null), OptResultType>::value, "");
    static_assert(std::is_same<decltype(row.r_opt_maybe_null), OptResultType>::value, "");
  }
}

int main()
{
  // boolean
  test_result_row<bool>(bool{true});

  // integral
  test_result_row<int64_t>(int8_t{7});
  test_result_row<int64_t>(int16_t{7});
  test_result_row<int64_t>(int32_t{7});
  test_result_row<int64_t>(int64_t{7});

  // unsigned integral
  test_result_row<uint64_t>(uint8_t{7});
  test_result_row<uint64_t>(uint16_t{7});
  test_result_row<uint64_t>(uint32_t{7});
  test_result_row<uint64_t>(uint64_t{7});

  // floating point
  test_result_row<double>(float{7.7});
  test_result_row<double>(double{7.7});

  // text
  test_result_row<::sqlpp::string_view>('7');
  test_result_row<::sqlpp::string_view>("seven");
  test_result_row<::sqlpp::string_view>(std::string("seven"));
  test_result_row<::sqlpp::string_view>(::sqlpp::string_view("seven"));

  // blob
  test_result_row<::sqlpp::span<uint8_t>>(std::vector<uint8_t>{});

  // date
  test_result_row<::sqlpp::chrono::day_point>(::sqlpp::chrono::day_point{});

  // timestamp
  test_result_row<::sqlpp::chrono::microsecond_point>(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_result_row<::sqlpp::chrono::microsecond_point>(minute_point{});

  // time_of_day
  test_result_row<std::chrono::microseconds>(std::chrono::microseconds{});

}

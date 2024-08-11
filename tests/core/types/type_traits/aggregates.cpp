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

  template <typename T, typename V>
  using is_same_type = std::is_same<sqlpp::value_type_of_t<T>, V>;
}

template <typename Value>
void test_aggregates(Value v)
{
  auto v_not_null = sqlpp::value(v);
  auto v_maybe_null = sqlpp::value(::sqlpp::make_optional(v));

  using OptFloat = sqlpp::value_type_of_t<::sqlpp::optional<float>>;

#warning: Need to test contains_aggregate

}

int main()
{
  // boolean
  test_avg(bool{true});

  // integral
  test_avg(int8_t{7});
  test_avg(int16_t{7});
  test_avg(int32_t{7});
  test_avg(int64_t{7});

  // unsigned integral
  test_avg(uint8_t{7});
  test_avg(uint16_t{7});
  test_avg(uint32_t{7});
  test_avg(uint64_t{7});

  // floating point
  test_avg(float{7.7});
  test_avg(double{7.7});

#warning: Should there be avg date time duration?
#if 0
  // date
  test_avg(::sqlpp::chrono::day_point{});

  // timestamp
  test_avg(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_avg(minute_point{});

  // time_of_day
  test_avg(std::chrono::microseconds{});
#endif
}


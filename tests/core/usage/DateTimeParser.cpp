/*
 * Copyright (c) 2023, Vesselin Atanasov
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

#include <sqlpp11/detail/parse_date_time.h>
#include <sqlpp11/sqlpp11.h>

#include <iostream>
#include <vector>

namespace
{
  std::chrono::microseconds build_tod(int hour = 0, int minute = 0, int second = 0, int us = 0, bool tz_plus = true, int tz_hour = 0, int tz_minute = 0, int tz_second = 0)
  {
    std::chrono::microseconds result{0};
    // We add time components one by one to the resulting microsecond_point in order to avoid going through temporary time_point values
    // with small bitsize which could cause in integer overflow.
    result += std::chrono::hours{hour};
    result += std::chrono::minutes{minute};
    result += std::chrono::seconds{second};
    result += std::chrono::microseconds{us};
    std::chrono::microseconds tz_offset{std::chrono::hours{tz_hour} + std::chrono::minutes{tz_minute} + std::chrono::seconds{tz_second}};
    if (tz_plus) {
      tz_offset = -tz_offset;
    }
    result += tz_offset;
    return result;
  }

  sqlpp::chrono::microsecond_point build_timestamp(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int us = 0, bool tz_plus = true, int tz_hour = 0, int tz_minute = 0, int tz_second = 0)
  {
    return
      date::sys_days{date::year{year}/month/day} +
      build_tod(hour, minute, second, us, tz_plus, tz_hour,tz_minute,tz_second);
  }

  template <typename L, typename R>
  void require_equal(int line, const L& l, const R& r)
  {
    if (l != r)
    {
      std::cerr << line << ": ";
      serialize(sqlpp::wrap_operand_t<L>{l}, std::cerr);
      std::cerr << " != ";
      serialize(sqlpp::wrap_operand_t<R>{r}, std::cerr);
      std::cerr << std::endl;
      throw std::runtime_error("Unexpected result");
    }
  }

  void test_valid_dates()
  {
    using namespace sqlpp::chrono;
    using namespace date;

    for (const auto& date_pair : std::vector<std::pair<const char*, day_point>>{
      // Minimum and maximum dates
      {"0001-01-01", year{1}/1/1}, {"9999-12-31", year{9999}/12/31},
      // Month minimum and maximum days
      {"1999-01-01", year{1999}/1/1}, {"1999-01-31", year{1999}/1/31},
      {"1999-02-01", year{1999}/2/1}, {"1999-02-28", year{1999}/2/28},
      {"1999-03-01", year{1999}/3/1}, {"1999-03-31", year{1999}/3/31},
      {"1999-04-01", year{1999}/4/1}, {"1999-04-30", year{1999}/4/30},
      {"1999-05-01", year{1999}/5/1}, {"1999-05-31", year{1999}/5/31},
      {"1999-06-01", year{1999}/6/1}, {"1999-06-30", year{1999}/6/30},
      {"1999-07-01", year{1999}/7/1}, {"1999-07-31", year{1999}/7/31},
      {"1999-08-01", year{1999}/8/1}, {"1999-08-31", year{1999}/8/31},
      {"1999-09-01", year{1999}/9/1}, {"1999-09-30", year{1999}/9/30},
      {"1999-10-01", year{1999}/10/1}, {"1999-10-31", year{1999}/10/31},
      {"1999-11-01", year{1999}/11/1}, {"1999-11-30", year{1999}/11/30},
      {"1999-12-01", year{1999}/12/1}, {"1999-12-31", year{1999}/12/31},
      // YYYY-02-29
      {"2396-02-29", year{2396}/2/29}, {"2400-02-29", year{2400}/2/29},
      // Valid format, but the year, month and/or day fall outside of the correct ranges
      {"1980-00-02", year{1980}/0/2}, {"1980-13-02", year{1980}/13/2},
      {"1980-01-00", year{1980}/1/0}, {"1980-01-32", year{1980}/1/32},
      {"1981-02-29", year{1981}/2/29}, {"2100-02-29", year{2100}/2/29}
    })
    {
      day_point dp;
      if (sqlpp::detail::parse_date(dp, date_pair.first) == false)
      {
        std::cerr << "Could not parse a valid date string: " << date_pair.first << std::endl;
        throw std::runtime_error{"Parse error"};
      }
      require_equal(__LINE__, dp, date_pair.second);
    }
  }

  void test_invalid_dates()
  {
    using namespace sqlpp::chrono;

    for (const auto& date_str : std::vector<const char*>{
      // Invalid year
      "", "1", "12", "123" , "1234", "A",
      // Invalid month
      "1980--02", "1980-1-02", "1980-123-02", "1980-W-02",
      // Invalid day
      "1980-01-", "1980-01-0", "1980-01-123", "1980-01-Q",
      // Invalid separator
      "1980 01 02", "1980- 01-02", "1980 -01-02", "1980-01 -02", "1980-01- 02", "1980-01T02"
      // Trailing characters
      "1980-01-02 ", "1980-01-02T", "1980-01-02 UTC", "1980-01-02EST", "1980-01-02+01"
    })
    {
      day_point dp;
      if (sqlpp::detail::parse_date(dp, date_str))
      {
        std::cerr << "Parsed successfully an invalid date string " << date_str << ", value ";
        serialize(sqlpp::wrap_operand_t<day_point>{dp}, std::cerr);
        std::cerr << std::endl;
        throw std::runtime_error{"Parse error"};
      }
    }
  }

  void test_valid_time_of_day()
  {
    using namespace std::chrono;

    for (const auto& tod_pair : std::vector<std::pair<const char*, microseconds>>{
      // Minimum value
      {"00:00:00", build_tod()},
      // Maximum hours
      {"23:00:00", build_tod(23)},
      // Maximum minutes
      {"00:59:00", build_tod(0, 59)},
      // Maximum seconds
      {"00:00:59", build_tod(0, 0, 59)},
      // Second fractions
      {"01:23:54.000001", build_tod(1, 23, 54, 1)},
      {"01:23:54.999999", build_tod(1, 23, 54, 999999)},
      // Timezone offsets
      {"10:09:08+03", build_tod(10, 9, 8, 0, true, 3)},
      {"10:09:08-03", build_tod(10, 9, 8, 0, false, 3)},
      {"10:09:08+03:02", build_tod(10, 9, 8, 0, true, 3, 2)},
      {"10:09:08-03:02", build_tod(10, 9, 8, 0, false, 3, 2)},
      {"10:09:08+13:12:11", build_tod(10, 9, 8, 0, true, 13, 12, 11)},
      {"10:09:08-13:12:11", build_tod(10, 9, 8, 0, false, 13, 12, 11)},
      // Second fraction and timezone offset
      {"10:09:08.1+03", build_tod(10, 9, 8, 100000, true, 3)},
      {"10:09:08.12-07:40", build_tod(10, 9, 8, 120000, false, 7, 40)},
      {"10:09:08.123+12:38:49", build_tod(10, 9, 8, 123000, true, 12, 38, 49)},
      // Valid format but invalid hour, minute or second range
      {"25:00:10", build_tod(25, 0, 10)}
    })
    {
      microseconds us;
      if (sqlpp::detail::parse_time_of_day(us, tod_pair.first) == false)
      {
        std::cerr << "Could not parse a valid time-of-day string: " << tod_pair.first << std::endl;
        throw std::runtime_error{"Parse error"};
      }
      require_equal(__LINE__, us, tod_pair.second);
    }
  }

  void test_invalid_time_of_day()
  {
    using namespace std::chrono;

    for (const auto& tod_str : std::vector<const char*>{
      // Generic string
      "A", "BC", "!()",
      // Invalid hour
      "-01:23:45", "AA:10:11",
      // Invalid minute
      "13::07", "13:A:07", "13:1:07", "13:-01:07"
      // Invalid second
      "04:07:", "04:07:A", "04:07:1", "04:07:-01"
      // Invalid fraction
      "01:02:03.", "01:02:03.A", "01:02:03.1234567", "01:02:03.1A2",
      // Invalid timezone
      "01:03:03!01", "01:03:03+A", "01:03:03+1", "01:03:03+1A", "01:03:03+456",
      "01:03:03+12:", "01:03:03+12:1", "01:03:03+12:1A", "01:03:03+12:01:",
      "01:03:03+12:01:1", "01:03:03+12:01:1A"
    })
    {
      microseconds us;
      if (sqlpp::detail::parse_time_of_day(us, tod_str))
      {
        std::cerr << "Parsed successfully an invalid time-of-day string " << tod_str << ", value ";
        serialize(sqlpp::wrap_operand_t<microseconds>{us}, std::cerr);
        std::cerr << std::endl;
        throw std::runtime_error{"Parse error"};
      }
    }
  }

  void test_valid_timestamp()
  {
    using namespace sqlpp::chrono;
    using namespace date;
    using namespace std::chrono;

    for (const auto& timestamp_pair : std::vector<std::pair<const char*, microsecond_point>>{
      // Minimum and maximum timestamps
      {"0001-01-01 00:00:00", build_timestamp(1, 1, 1)}, {"9999-12-31 23:59:59.999999", build_timestamp(9999, 12, 31, 23, 59, 59, 999999)},
      // Timestamp with time zone
      {"1234-03-25 23:17:08.479210+10:17:29", build_timestamp(1234, 3, 25, 23, 17, 8, 479210, true, 10, 17, 29)}
    })
    {
      microsecond_point tp;
      if (sqlpp::detail::parse_timestamp(tp, timestamp_pair.first) == false)
      {
        std::cerr << "Could not parse a valid timestamp string: " << timestamp_pair.first << std::endl;
        throw std::runtime_error{"Parse error"};
      }
      require_equal(__LINE__, tp, timestamp_pair.second);
    }
  }

  void test_invalid_timestamp()
  {
    using namespace sqlpp::chrono;

    for (const auto& timestamp_str : std::vector<const char*>{
      // Generic string
      "", "B", ")-#\\",
      // Invalid date
      "197%-03-17 10:32:09",
      // Invalid time of day
      "2020-02-18 22:2:28"
      // Invalid time zone
      "1924-02-28 18:35:36+1"
      // Leading space
      " 2030-17-01 15:20:30",
      // Trailing space
      "2030-17-01 15:20:30 "
    })
    {
      microsecond_point tp;
      if (sqlpp::detail::parse_timestamp(tp, timestamp_str))
      {
        std::cerr << "Parsed successfully an invalid timestamp string " << timestamp_str << ", value ";
        serialize(sqlpp::wrap_operand_t<microsecond_point>{tp}, std::cerr);
        std::cerr << std::endl;
        throw std::runtime_error{"Parse error"};
      }
    }
  }

  void test_valid_date_or_timestamp()
  {
    using namespace sqlpp::chrono;
    using namespace date;
    using namespace std::chrono;

    for (const auto& timestamp_pair : std::vector<std::pair<const char*, microsecond_point>>{
      // Valid date
      {"1998-02-03", build_timestamp(1998, 2, 3)},
      // Valid timestamp
      {"2015-07-08 06:32:45.872+23:14:39", build_timestamp(2015,7,8,6,32,45,872000,true,23,14,39)}
    })
    {
      microsecond_point tp;
      if (sqlpp::detail::parse_date_or_timestamp(tp, timestamp_pair.first) == false)
      {
        std::cerr << "Could not parse a valid date or timestamp string: " << timestamp_pair.first << std::endl;
        throw std::runtime_error{"Parse error"};
      }
      require_equal(__LINE__, tp, timestamp_pair.second);
    }
  }

  void test_invalid_date_or_timestamp()
  {
    using namespace sqlpp::chrono;

    for (const auto& timestamp_str : std::vector<const char*>{
      // Generic string
      "", "C", "/=",
      // Invalid dates
      "A123-01-02", "1980-E-04", "1981-09-",
      // Invalid timestamps
      "2023-12-31 1:02:03", "2024-03-04 05::06",
      // Invalid time zone
      "1930-03-18 17:30:31+01:",
      // Leading space
      " 1930-03-18 17:30:31+01",
      // Trailing space
      "1930-03-18 17:30:31+01 "
    })
    {
      microsecond_point tp;
      if (sqlpp::detail::parse_date_or_timestamp(tp, timestamp_str))
      {
        std::cerr << "Parsed successfully an invalid date or timestamp string " << timestamp_str << ", value ";
        serialize(sqlpp::wrap_operand_t<microsecond_point>{tp}, std::cerr);
        std::cerr << std::endl;
        throw std::runtime_error{"Parse error"};
      }
    }
  }
}

int DateTimeParser(int, char*[])
{
  test_valid_dates();
  test_invalid_dates();
  test_valid_time_of_day();
  test_invalid_time_of_day();
  test_valid_timestamp();
  test_invalid_timestamp();
  test_valid_date_or_timestamp();
  test_invalid_date_or_timestamp();
  return 0;
}

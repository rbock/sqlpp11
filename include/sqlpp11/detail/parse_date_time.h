#pragma once

/**
 * Copyright (c) 2023, Vesselin Atanasov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
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

#include <sqlpp11/chrono.h>

#include <cctype>

namespace sqlpp
{
  namespace detail
  {
    inline bool parse_unsigned(int& value, const char*& input, int length)
    {
      value = 0;
      auto new_input = input;
      while (length--)
      {
        auto ch = *new_input++;
        if (std::isdigit(ch) == false)
        {
          return false;
        }
        value = value * 10 + ch - '0';
      }
      input = new_input;
      return true;
    }

    inline bool parse_character(const char*& input, char ch)
    {
      if (*input != ch)
      {
        return false;
      }
      ++input;
      return true;
    }

    inline bool parse_yyyy_mm_dd(sqlpp::chrono::day_point& dp, const char*& input)
    {
      auto new_input = input;
      int year, month, day;
      if ((parse_unsigned(year, new_input, 4) == false) || (parse_character(new_input, '-') == false) ||
          (parse_unsigned(month, new_input, 2) == false) || (parse_character(new_input, '-') == false) ||
          (parse_unsigned(day, new_input, 2) == false))
      {
        return false;
      }
      dp = ::date::year{year} / month / day;
      input = new_input;
      return true;
    }

    inline bool parse_hh_mm_ss(std::chrono::microseconds& us, const char*& input)
    {
      auto new_input = input;
      int hour, minute, second;
      if ((parse_unsigned(hour, new_input, 2) == false) || (parse_character(new_input, ':') == false) ||
          (parse_unsigned(minute, new_input, 2) == false) || (parse_character(new_input, ':') == false) ||
          (parse_unsigned(second, new_input, 2) == false))
      {
        return false;
      }
      // Strings that have valid format but year, month and/or day values that fall outside of the
      // correct ranges are still mapped to day_point values. For the exact rules of the mapping see
      // https://en.cppreference.com/w/cpp/chrono/year_month_day/operator_days
      us = std::chrono::hours{hour} + std::chrono::minutes{minute} + std::chrono::seconds{second};
      input = new_input;
      return true;
    }

    inline bool parse_ss_fraction(std::chrono::microseconds& us, const char*& input)
    {
      auto new_input = input;
      if (parse_character(new_input, '.') == false)
      {
        return false;
      }
      int value = 0;
      int len_max = 6;
      int len_actual;
      for (len_actual = 0; (len_actual < len_max) && std::isdigit(*new_input); ++len_actual, ++new_input)
      {
        value = value * 10 + *new_input - '0';
      }
      if (len_actual == 0)
      {
        return false;
      }
      for (; len_actual < len_max; ++len_actual)
      {
        value *= 10;
      }
      us = std::chrono::microseconds{value};
      input = new_input;
      return true;
    }

    inline bool parse_tz(std::chrono::microseconds& offset, const char*& input)
    {
      auto new_input = input;
      int tz_sign;
      if (parse_character(new_input, '+'))
      {
        tz_sign = 1;
      }
      else if (parse_character(new_input, '-'))
      {
        tz_sign = -1;
      }
      else
      {
        return false;
      }
      int hour;
      if (parse_unsigned(hour, new_input, 2) == false)
      {
        return false;
      }
      offset = tz_sign * std::chrono::hours{hour};
      input = new_input;
      int minute;
      if ((parse_character(new_input, ':') == false) || (parse_unsigned(minute, new_input, 2) == false))
      {
        return true;
      }
      offset += tz_sign * std::chrono::minutes{minute};
      input = new_input;
      int second;
      if ((parse_character(new_input, ':') == false) || (parse_unsigned(second, new_input, 2) == false))
      {
        return true;
      }
      offset += tz_sign * std::chrono::seconds{second};
      input = new_input;
      return true;
    }

    inline bool parse_hh_mm_ss_us_tz(std::chrono::microseconds& us, const char*& input)
    {
      if (parse_hh_mm_ss(us, input) == false)
      {
        return false;
      }
      std::chrono::microseconds fraction;
      if (parse_ss_fraction(fraction, input))
      {
        us += fraction;
      }
      std::chrono::microseconds tz_offset;
      if (parse_tz(tz_offset, input))
      {
        us -= tz_offset;
      }
      return true;
    }

    // Parse timestamp formatted as YYYY-MM-DD HH:MM:SS.U+HH:MM:SS
    // The microseconds and timezone offset are optional
    //
    inline bool parse_timestamp(sqlpp::chrono::microsecond_point& tp, const char* date_time_string)
    {
      sqlpp::chrono::day_point parsed_ymd;
      std::chrono::microseconds parsed_tod;
      if ((parse_yyyy_mm_dd(parsed_ymd, date_time_string) == false) ||
          (parse_character(date_time_string, ' ') == false) ||
          (parse_hh_mm_ss_us_tz(parsed_tod, date_time_string) == false))
      {
        return false;
      }
      if (*date_time_string)
      {
        return false;
      }
      tp = parsed_ymd + parsed_tod;
      return true;
    }

    // Parse date string formatted as YYYY-MM-DD
    //
    inline bool parse_date(sqlpp::chrono::day_point& dp, const char* date_string)
    {
      if (parse_yyyy_mm_dd(dp, date_string) == false)
      {
        return false;
      }
      if (*date_string)
      {
        return false;
      }
      return true;
    }

    // Parse time string formatted as YYYY-MM-DD HH:MM:SS.U+HH:MM:SS
    // The time-of-day part is optional
    //
    inline bool parse_date_or_timestamp(sqlpp::chrono::microsecond_point& tp, const char* date_time_string)
    {
      sqlpp::chrono::day_point parsed_ymd;
      if (parse_yyyy_mm_dd(parsed_ymd, date_time_string) == false)
      {
        return false;
      }
      if (*date_time_string == 0)
      {
        tp = parsed_ymd;
        return true;
      }
      std::chrono::microseconds parsed_tod;
      if ((parse_character(date_time_string, ' ') == false) ||
          (parse_hh_mm_ss_us_tz(parsed_tod, date_time_string) == false))
      {
        return false;
      }
      if (*date_time_string == 0)
      {
        tp = parsed_ymd + parsed_tod;
        return true;
      }
      return false;
    }

    // Parse time of day string formatted as HH:MM:SS.U+HH:MM:SS
    // The microseconds and timezone offset are optional
    //
    inline bool parse_time_of_day(std::chrono::microseconds& us, const char* time_string)
    {
      if (parse_hh_mm_ss_us_tz(us, time_string) == false)
      {
        return false;
      }
      if (*time_string)
      {
        return false;
      }
      return true;
    }
  }  // namespace detail
}  // namespace sqlpp

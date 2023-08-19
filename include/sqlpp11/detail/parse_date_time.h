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

#include <regex>

namespace sqlpp
{
  namespace detail
  {
    // Parse a date string formatted as YYYY-MM-DD
    //
    inline bool parse_string_date(::sqlpp::chrono::day_point& value, const char* date_string)
    {
      static const std::regex rx{"(\\d{4})-(\\d{2})-(\\d{2})"};
      std::cmatch mr;
      if (std::regex_match(date_string, mr, rx) == false)
      {
        return false;
      }
      value = ::sqlpp::chrono::day_point{
        ::date::year{std::atoi(date_string + mr.position(1))} / // Year
        std::atoi(date_string + mr.position(2)) /               // Month
        std::atoi(date_string + mr.position(3))                 // Day of month
      };
      return true;
    }

    // Parse a date string formatted as YYYY-MM-DD HH:MM:SS.US TZ
    // .US are optional fractional seconds, up to 6 digits in length
    // TZ is an optional time zone offset formatted as +HH[:MM] or -HH[:MM]
    //
    inline bool parse_string_date_time(::sqlpp::chrono::microsecond_point& value, const char* date_time_string)
    {
      static const std::regex rx{
        "(\\d{4})-(\\d{2})-(\\d{2}) "
        "(\\d{2}):(\\d{2}):(\\d{2})(?:\\.(\\d{1,6}))?"
        "(?:([+-])(\\d{2})(?::(\\d{2})(?::(\\d{2}))?)?)?"
      };
      std::cmatch mr;
      if (std::regex_match(date_time_string, mr, rx) == false)
      {
        return false;
      }
      value =
        ::sqlpp::chrono::day_point{
          ::date::year{std::atoi(date_time_string + mr.position(1))} / // Year
          std::atoi(date_time_string + mr.position(2)) /               // Month
          std::atoi(date_time_string + mr.position(3))                 // Day of month
        } +
        std::chrono::hours{std::atoi(date_time_string + mr.position(4))} +     // Hour
        std::chrono::minutes{std::atoi(date_time_string + mr.position(5))} +   // Minute
        std::chrono::seconds{std::atoi(date_time_string + mr.position(6))} +   // Second
        ::std::chrono::microseconds{                                           // Second fraction
          mr[7].matched ? std::stoi((mr[7].str() + "000000").substr(0, 6)) : 0
        };
      if (mr[8].matched)
      {
        const auto tz_sign = (date_time_string[mr.position(8)] == '+') ? 1 : -1;
        const auto tz_offset =
          std::chrono::hours{std::atoi(date_time_string + mr.position(9))} +
          std::chrono::minutes{mr[10].matched ? std::atoi(date_time_string + mr.position(10)) : 0} +
          std::chrono::seconds{mr[11].matched ? std::atoi(date_time_string + mr.position(11)) : 0};
        value -= tz_sign * tz_offset;
      }
      return true;
    }

    // Parse a time string formatted as HH:MM:SS[.US][ TZ]
    // .US is up to 6 digits in length
    // TZ is an optional time zone offset formatted as +HH[:MM] or -HH[:MM]
    //
    inline bool parse_string_time_of_day(::std::chrono::microseconds& value, const char* time_string)
    {
      static const std::regex rx{
        "(\\d{2}):(\\d{2}):(\\d{2})(?:\\.(\\d{1,6}))?"
        "(?:([+-])(\\d{2})(?::(\\d{2})(?::(\\d{2}))?)?)?"
      };
      std::cmatch mr;
      if (std::regex_match (time_string, mr, rx) == false)
      {
        return false;
      }
      value =
        std::chrono::hours{std::atoi(time_string + mr.position(1))} +     // Hour
        std::chrono::minutes{std::atoi(time_string + mr.position(2))} +   // Minute
        std::chrono::seconds{std::atoi(time_string + mr.position(3))} +   // Second
        ::std::chrono::microseconds{                                      // Second fraction
          mr[4].matched ? std::stoi((mr[4].str() + "000000").substr(0, 6)) : 0
        };
      if (mr[5].matched)
      {
        const auto tz_sign = (time_string[mr.position(5)] == '+') ? 1 : -1;
        const auto tz_offset =
          std::chrono::hours{std::atoi(time_string + mr.position(6))} +
          std::chrono::minutes{mr[7].matched ? std::atoi(time_string + mr.position(7)) : 0} +
          std::chrono::seconds{mr[8].matched ? std::atoi(time_string + mr.position(8)) : 0};
        value -= tz_sign * tz_offset;
      }
      return true;
    }
  }  // namespace detail
}  // namespace sqlpp

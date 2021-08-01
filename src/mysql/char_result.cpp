/*
 * Copyright (c) 2013 - 2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "detail/result_handle.h"
#include <cctype>
#include <ciso646>
#include <date/date.h>
#include <iostream>
#include <sqlpp11/mysql/char_result.h>
#include <vector>

namespace sqlpp
{
  namespace mysql
  {
    char_result_t::char_result_t()
    {
    }

    char_result_t::char_result_t(std::unique_ptr<detail::result_handle>&& handle) : _handle(std::move(handle))
    {
      if (_invalid())
        throw sqlpp::exception("MySQL: Constructing char_result without valid handle");

      if (_handle->debug)
        std::cerr << "MySQL debug: Constructing result, using handle at " << _handle.get() << std::endl;
    }

    char_result_t::~char_result_t() = default;
    char_result_t::char_result_t(char_result_t&& rhs) = default;
    char_result_t& char_result_t::operator=(char_result_t&&) = default;

    namespace
    {
      const auto date_digits = std::vector<char>{1, 1, 1, 1, 0, 1, 1, 0, 1, 1};  // 2015-10-28
      const auto time_digits = std::vector<char>{0, 1, 1, 0, 1, 1, 0, 1, 1};     // T23:00:12

      auto check_digits(const char* text, const std::vector<char>& digitFlags) -> bool
      {
        for (const auto digitFlag : digitFlags)
        {
          if (digitFlag)
          {
            if (not std::isdigit(*text))
            {
              return false;
            }
          }
          else
          {
            if (std::isdigit(*text) or *text == '\0')
            {
              return false;
            }
          }
          ++text;
        }
        return true;
      }
    }

    bool char_result_t::_invalid() const
    {
      return !_handle or !*_handle;
    }

    void char_result_t::_bind_date_result(size_t index, ::sqlpp::chrono::day_point* value, bool* is_null)
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: parsing date result at index: " << index << std::endl;

      *is_null = (_char_result_row.data == nullptr or _char_result_row.data[index] == nullptr);
      if (*is_null)
      {
        *value = {};
        return;
      }

      const auto date_string = _char_result_row.data[index];
      if (_handle->debug)
        std::cerr << "MySQL debug: date string: " << date_string << std::endl;

      if (check_digits(date_string, date_digits))
      {
        const auto ymd = ::date::year(std::atoi(date_string)) / atoi(date_string + 5) / atoi(date_string + 8);
        *value = ::sqlpp::chrono::day_point(ymd);
      }
      else
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: invalid date result: " << date_string << std::endl;
        *value = {};
      }
    }

    void char_result_t::_bind_date_time_result(size_t index, ::sqlpp::chrono::microsecond_point* value, bool* is_null)
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: parsing date result at index: " << index << std::endl;

      *is_null = (_char_result_row.data == nullptr or _char_result_row.data[index] == nullptr);
      if (*is_null)
      {
        *value = {};
        return;
      }

      const auto date_time_string = _char_result_row.data[index];
      if (_handle->debug)
        std::cerr << "MySQL debug: date_time string: " << date_time_string << std::endl;

      if (check_digits(date_time_string, date_digits))
      {
        const auto ymd =
            ::date::year(std::atoi(date_time_string)) / atoi(date_time_string + 5) / atoi(date_time_string + 8);
        *value = ::sqlpp::chrono::day_point(ymd);
      }
      else
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: invalid date_time result: " << date_time_string << std::endl;
        *value = {};

        return;
      }

      const auto time_string = date_time_string + 10;
      if (check_digits(time_string, time_digits))
      {
        *value += ::std::chrono::hours(std::atoi(time_string + 1)) + std::chrono::minutes(std::atoi(time_string + 4)) +
                  std::chrono::seconds(std::atoi(time_string + 7));
      }
      else
      {
        return;
      }

      const auto mu_string = time_string + 9;
      if (mu_string[0] == '\0')
      {
        return;
      }
      auto factor = 100 * 1000;
      for (auto i = 1u; i <= 6u and std::isdigit(mu_string[i]); ++i, factor /= 10)
      {
        *value += ::std::chrono::microseconds(factor * (mu_string[i] - '0'));
      }
    }

    bool char_result_t::next_impl()
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: Accessing next row of handle at " << _handle.get() << std::endl;

      _char_result_row.data = const_cast<const char**>(mysql_fetch_row(_handle->mysql_res));
      _char_result_row.len = mysql_fetch_lengths(_handle->mysql_res);

      return _char_result_row.data;
    }
  }
}

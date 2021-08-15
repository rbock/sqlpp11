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

#ifndef SQLPP_MYSQL_CHAR_RESULT_H
#define SQLPP_MYSQL_CHAR_RESULT_H

#include <ciso646>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sqlpp11/chrono.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/mysql/sqlpp_mysql.h>
#include <sqlpp11/mysql/char_result_row.h>

namespace sqlpp
{
  namespace mysql
  {
    namespace detail
    {
      struct result_handle
      {
        MYSQL_RES* mysql_res;
        bool debug;

        result_handle(MYSQL_RES* res, bool debug_) : mysql_res(res), debug(debug_)
        {
        }

        result_handle(const result_handle&) = delete;
        result_handle(result_handle&&) = default;
        result_handle& operator=(const result_handle&) = delete;
        result_handle& operator=(result_handle&&) = default;

        ~result_handle()
        {
          if (mysql_res)
            mysql_free_result(mysql_res);
        }

        bool operator!() const
        {
          return !mysql_res;
        }
      };

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
    }  // namespace detail

    class char_result_t
    {
      std::unique_ptr<detail::result_handle> _handle;
      char_result_row_t _char_result_row;

    public:
      char_result_t() = default;
      char_result_t(std::unique_ptr<detail::result_handle>&& handle) : _handle(std::move(handle))
      {
        if (_invalid())
          throw sqlpp::exception("MySQL: Constructing char_result without valid handle");

        if (_handle->debug)
          std::cerr << "MySQL debug: Constructing result, using handle at " << _handle.get() << std::endl;
      }

      char_result_t(const char_result_t&) = delete;
      char_result_t(char_result_t&& rhs) = default;
      char_result_t& operator=(const char_result_t&) = delete;
      char_result_t& operator=(char_result_t&&) = default;
      ~char_result_t() = default;

      bool operator==(const char_result_t& rhs) const
      {
        return _handle == rhs._handle;
      }

      template <typename ResultRow>
      void next(ResultRow& result_row)
      {
        if (_invalid())
        {
          result_row._invalidate();
          return;
        }

        if (next_impl())
        {
          if (not result_row)
          {
            result_row._validate();
          }
          result_row._bind(*this);
        }
        else
        {
          if (result_row)
            result_row._invalidate();
        }
      }

      bool _invalid() const
      {
        return !_handle or !*_handle;
      }

      void _bind_boolean_result(size_t index, signed char* value, bool* is_null)
      {
        *is_null = (_char_result_row.data == nullptr or _char_result_row.data[index] == nullptr);
        *value =
            (*is_null ? false : (_char_result_row.data[index][0] == 't' or _char_result_row.data[index][0] == '1'));
      }

      void _bind_floating_point_result(size_t index, double* value, bool* is_null)
      {
        *is_null = (_char_result_row.data == nullptr or _char_result_row.data[index] == nullptr);
        *value = (*is_null ? 0 : std::strtod(_char_result_row.data[index], nullptr));
      }

      void _bind_integral_result(size_t index, int64_t* value, bool* is_null)
      {
        *is_null = (_char_result_row.data == nullptr or _char_result_row.data[index] == nullptr);
        *value = (*is_null ? 0 : std::strtoll(_char_result_row.data[index], nullptr, 10));
      }

      void _bind_unsigned_integral_result(size_t index, uint64_t* value, bool* is_null)
      {
        *is_null = (_char_result_row.data == nullptr or _char_result_row.data[index] == nullptr);
        *value = (*is_null ? 0 : std::strtoull(_char_result_row.data[index], nullptr, 10));
      }

      void _bind_blob_result(size_t index, const uint8_t** value, size_t* len)
      {
        bool is_null = (_char_result_row.data == nullptr or _char_result_row.data[index] == nullptr);
        *value = (uint8_t*)(is_null ? nullptr : _char_result_row.data[index]);
        *len = (is_null ? 0 : _char_result_row.len[index]);
      }

      void _bind_text_result(size_t index, const char** value, size_t* len)
      {
        bool is_null = (_char_result_row.data == nullptr or _char_result_row.data[index] == nullptr);
        *value = (is_null ? nullptr : _char_result_row.data[index]);
        *len = (is_null ? 0 : _char_result_row.len[index]);
      }

      void _bind_date_result(size_t index, ::sqlpp::chrono::day_point* value, bool* is_null)
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

        if (detail::check_digits(date_string, detail::date_digits))
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

      void _bind_date_time_result(size_t index, ::sqlpp::chrono::microsecond_point* value, bool* is_null)
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

        if (detail::check_digits(date_time_string, detail::date_digits))
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
        if (detail::check_digits(time_string, detail::time_digits))
        {
          *value += ::std::chrono::hours(std::atoi(time_string + 1)) +
                    std::chrono::minutes(std::atoi(time_string + 4)) + std::chrono::seconds(std::atoi(time_string + 7));
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

    private:
      bool next_impl()
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: Accessing next row of handle at " << _handle.get() << std::endl;

        _char_result_row.data = const_cast<const char**>(mysql_fetch_row(_handle->mysql_res));
        _char_result_row.len = mysql_fetch_lengths(_handle->mysql_res);

        return _char_result_row.data;
      }
    };
  }  // namespace mysql
}  // namespace sqlpp
#endif

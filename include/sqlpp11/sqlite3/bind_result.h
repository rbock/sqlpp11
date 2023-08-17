#pragma once

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

#include <memory>
#include <iostream>
#include <sqlpp11/chrono.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/sqlite3/detail/prepared_statement_handle.h>
#include <sqlpp11/sqlite3/export.h>

#ifdef _MSC_VER
#include <iso646.h>
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace sqlpp
{
  namespace sqlite3
  {
    namespace detail
    {
      inline auto check_first_digit(const char* text, bool digit_flag) -> bool
      {
        if (digit_flag)
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
        return true;
      }

      inline auto check_date_digits(const char* text) -> bool
      {
        for (const auto digit_flag : {true, true, true, true, false, true, true, false, true, true})  // YYYY-MM-DD
        {
          if (not check_first_digit(text, digit_flag))
            return false;
          ++text;
        }
        return true;
      }

      inline auto check_time_digits(const char* text) -> bool
      {
        for (const auto digit_flag : {true, true, false, true, true, false, true, true}) // hh:mm:ss
        {
          if (not check_first_digit(text, digit_flag))
            return false;
          ++text;
        }
        return true;
      }

      inline auto check_ms_digits(const char* text) -> bool
      {
        for (const auto digit_flag : {true, true, true})
        {
          if (not check_first_digit(text, digit_flag))
            return false;
          ++text;
        }
        return true;
      }
    }  // namespace detail

    class SQLPP11_SQLITE3_EXPORT bind_result_t
    {
      std::shared_ptr<detail::prepared_statement_handle_t> _handle;

    public:
      bind_result_t() = default;
      bind_result_t(const std::shared_ptr<detail::prepared_statement_handle_t>& handle) : _handle(handle)
      {
        if (_handle and _handle->debug)
          std::cerr << "Sqlite3 debug: Constructing bind result, using handle at " << _handle.get() << std::endl;
      }

      bind_result_t(const bind_result_t&) = delete;
      bind_result_t(bind_result_t&& rhs) = default;
      bind_result_t& operator=(const bind_result_t&) = delete;
      bind_result_t& operator=(bind_result_t&&) = default;
      ~bind_result_t() = default;

      bool operator==(const bind_result_t& rhs) const
      {
        return _handle == rhs._handle;
      }

      template <typename ResultRow>
      void next(ResultRow& result_row)
      {
        if (!_handle)
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

      void _bind_boolean_result(size_t index, signed char* value, bool* is_null)
      {
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: binding boolean result " << *value << " at index: " << index << std::endl;

        *value = static_cast<signed char>(sqlite3_column_int(_handle->sqlite_statement, static_cast<int>(index)));
        *is_null = sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)) == SQLITE_NULL;
      }

      void _bind_floating_point_result(size_t index, double* value, bool* is_null)
      {
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: binding floating_point result " << *value << " at index: " << index << std::endl;

        switch (sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)))
        {
          case (SQLITE3_TEXT):
            *value = atof(
                reinterpret_cast<const char*>(sqlite3_column_text(_handle->sqlite_statement, static_cast<int>(index))));
            break;
          default:
            *value = sqlite3_column_double(_handle->sqlite_statement, static_cast<int>(index));
        }
        *is_null = sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)) == SQLITE_NULL;
      }

      void _bind_integral_result(size_t index, int64_t* value, bool* is_null)
      {
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: binding integral result " << *value << " at index: " << index << std::endl;

        *value = sqlite3_column_int64(_handle->sqlite_statement, static_cast<int>(index));
        *is_null = sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)) == SQLITE_NULL;
      }

      void _bind_unsigned_integral_result(size_t index, uint64_t* value, bool* is_null)
      {
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: binding unsigned integral result " << *value << " at index: " << index
                    << std::endl;

        *value = static_cast<uint64_t>(sqlite3_column_int64(_handle->sqlite_statement, static_cast<int>(index)));
        *is_null = sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)) == SQLITE_NULL;
      }

      void _bind_text_result(size_t index, const char** value, size_t* len)
      {
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: binding text result at index: " << index << std::endl;

        *value =
            (reinterpret_cast<const char*>(sqlite3_column_text(_handle->sqlite_statement, static_cast<int>(index))));
        *len = static_cast<size_t>(sqlite3_column_bytes(_handle->sqlite_statement, static_cast<int>(index)));
      }

      void _bind_blob_result(size_t index, const uint8_t** value, size_t* len)
      {
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: binding text result at index: " << index << std::endl;

        *value =
            (reinterpret_cast<const uint8_t*>(sqlite3_column_blob(_handle->sqlite_statement, static_cast<int>(index))));
        *len = static_cast<size_t>(sqlite3_column_bytes(_handle->sqlite_statement, static_cast<int>(index)));
      }

      void _bind_date_result(size_t index, ::sqlpp::chrono::day_point* value, bool* is_null)
      {
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: binding date result at index: " << index << std::endl;

        *is_null = sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)) == SQLITE_NULL;
        if (*is_null)
        {
          *value = {};
          return;
        }

        const auto date_string =
            reinterpret_cast<const char*>(sqlite3_column_text(_handle->sqlite_statement, static_cast<int>(index)));
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: date string: " << date_string << std::endl;

        if (detail::check_date_digits(date_string))
        {
          const auto ymd = ::date::year(std::atoi(date_string)) / atoi(date_string + 5) / atoi(date_string + 8);
          *value = ::sqlpp::chrono::day_point(ymd);
        }
        else
        {
          if (_handle->debug)
            std::cerr << "Sqlite3 debug: invalid date result: " << date_string << std::endl;
          *value = {};
        }
      }

      void _bind_date_time_result(size_t index, ::sqlpp::chrono::microsecond_point* value, bool* is_null)
      {
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: binding date result at index: " << index << std::endl;

        *is_null = sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)) == SQLITE_NULL;
        if (*is_null)
        {
          *value = {};
          return;
        }

        const auto date_time_string =
            reinterpret_cast<const char*>(sqlite3_column_text(_handle->sqlite_statement, static_cast<int>(index)));
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: date_time string: " << date_time_string << std::endl;

        if (detail::check_date_digits(date_time_string))
        {
          const auto ymd =
              ::date::year(std::atoi(date_time_string)) / atoi(date_time_string + 5) / atoi(date_time_string + 8);
          *value = ::sqlpp::chrono::day_point(ymd);
        }
        else
        {
          if (_handle->debug)
            std::cerr << "Sqlite3 debug: invalid date_time result: " << date_time_string << std::endl;
          *value = {};

          return;
        }

        const auto time_string = date_time_string + 11; // YYYY-MM-DDT
        if (detail::check_time_digits(time_string))
        {
          *value += ::std::chrono::hours(std::atoi(time_string + 0)) +
                    std::chrono::minutes(std::atoi(time_string + 3)) + std::chrono::seconds(std::atoi(time_string + 6));
        }
        else
        {
          return;
        }
        const auto ms_string = time_string + 9; // hh:mm:ss.
        if (detail::check_ms_digits(ms_string) and ms_string[4] == '\0')
        {
          *value += ::std::chrono::milliseconds(std::atoi(ms_string));
        }
        else
        {
          return;
        }
      }

    private:
      bool next_impl()
      {
        if (_handle->debug)
          std::cerr << "Sqlite3 debug: Accessing next row of handle at " << _handle.get() << std::endl;

        auto rc = sqlite3_step(_handle->sqlite_statement);

        switch (rc)
        {
          case SQLITE_ROW:
            return true;
          case SQLITE_DONE:
            return false;
          default:
            throw sqlpp::exception("Sqlite3 error: Unexpected return value for sqlite3_step()");
        }
      }
    };
  }  // namespace sqlite3
}  // namespace sqlpp

#ifdef _MSC_VER
#pragma warning(pop)
#endif

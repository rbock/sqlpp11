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

#include <date/date.h>

#include <sqlpp11/mysql/sqlpp_mysql.h>
#include <sqlpp11/mysql/bind_result.h>
#include <memory>
#include <iostream>
#include <string>
#include <sqlpp11/chrono.h>

namespace sqlpp
{
  namespace mysql
  {
    class connection_base;

    class prepared_statement_t
    {
      friend ::sqlpp::mysql::connection_base;
      std::shared_ptr<detail::prepared_statement_handle_t> _handle;

    public:
      prepared_statement_t() = delete;
      prepared_statement_t(std::shared_ptr<detail::prepared_statement_handle_t>&& handle) : _handle{std::move(handle)}
      {
        if (_handle and _handle->debug)
          std::cerr << "MySQL debug: Constructing prepared_statement, using handle at " << _handle.get() << std::endl;
      }

      prepared_statement_t(const prepared_statement_t&) = delete;
      prepared_statement_t(prepared_statement_t&& rhs) = default;
      prepared_statement_t& operator=(const prepared_statement_t&) = delete;
      prepared_statement_t& operator=(prepared_statement_t&&) = default;
      ~prepared_statement_t() = default;

      bool operator==(const prepared_statement_t& rhs) const
      {
        return _handle == rhs._handle;
      }

      void _pre_bind();

      void _bind_boolean_parameter(size_t index, const signed char* value, bool is_null)
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: binding boolean parameter " << (*value ? "true" : "false")
                    << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
        _handle->stmt_param_is_null[index] = is_null;
        MYSQL_BIND& param{_handle->stmt_params[index]};
        param.buffer_type = MYSQL_TYPE_TINY;
        param.buffer = const_cast<signed char*>(value);
        param.buffer_length = sizeof(*value);
        param.length = &param.buffer_length;
        param.is_null = &_handle->stmt_param_is_null[index].value;
        param.is_unsigned = false;
        param.error = nullptr;
      }

      void _bind_integral_parameter(size_t index, const int64_t* value, bool is_null)
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: binding integral parameter " << *value << " at index: " << index << ", being "
                    << (is_null ? "" : "not ") << "null" << std::endl;
        _handle->stmt_param_is_null[index] = is_null;
        MYSQL_BIND& param{_handle->stmt_params[index]};
        param.buffer_type = MYSQL_TYPE_LONGLONG;
        param.buffer = const_cast<int64_t*>(value);
        param.buffer_length = sizeof(*value);
        param.length = &param.buffer_length;
        param.is_null = &_handle->stmt_param_is_null[index].value;
        param.is_unsigned = false;
        param.error = nullptr;
      }

      void _bind_unsigned_integral_parameter(size_t index, const uint64_t* value, bool is_null)
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: binding unsigned integral parameter " << *value << " at index: " << index
                    << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
        _handle->stmt_param_is_null[index] = is_null;
        MYSQL_BIND& param{_handle->stmt_params[index]};
        param.buffer_type = MYSQL_TYPE_LONGLONG;
        param.buffer = const_cast<uint64_t*>(value);
        param.buffer_length = sizeof(*value);
        param.length = &param.buffer_length;
        param.is_null = &_handle->stmt_param_is_null[index].value;
        param.is_unsigned = true;
        param.error = nullptr;
      }

      void _bind_floating_point_parameter(size_t index, const double* value, bool is_null)
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: binding floating_point parameter " << *value << " at index: " << index
                    << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
        _handle->stmt_param_is_null[index] = is_null;
        MYSQL_BIND& param{_handle->stmt_params[index]};
        param.buffer_type = MYSQL_TYPE_DOUBLE;
        param.buffer = const_cast<double*>(value);
        param.buffer_length = sizeof(*value);
        param.length = &param.buffer_length;
        param.is_null = &_handle->stmt_param_is_null[index].value;
        param.is_unsigned = false;
        param.error = nullptr;
      }

      void _bind_text_parameter(size_t index, const std::string* value, bool is_null)
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: binding text parameter " << *value << " at index: " << index << ", being "
                    << (is_null ? "" : "not ") << "null" << std::endl;
        _handle->stmt_param_is_null[index] = is_null;
        MYSQL_BIND& param{_handle->stmt_params[index]};
        param.buffer_type = MYSQL_TYPE_STRING;
        param.buffer = const_cast<char*>(value->data());
        param.buffer_length = value->size();
        param.length = &param.buffer_length;
        param.is_null = &_handle->stmt_param_is_null[index].value;
        param.is_unsigned = false;
        param.error = nullptr;
      }

      void _bind_date_parameter(size_t index, const ::sqlpp::chrono::day_point* value, bool is_null)
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: binding date parameter "
                    << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;

        auto& bound_time = _handle->stmt_date_time_param_buffer[index];
        if (not is_null)
        {
          const auto ymd = ::date::year_month_day{*value};
          bound_time.year = static_cast<unsigned>(std::abs(static_cast<int>(ymd.year())));
          bound_time.month = static_cast<unsigned>(ymd.month());
          bound_time.day = static_cast<unsigned>(ymd.day());
          bound_time.hour = 0u;
          bound_time.minute = 0u;
          bound_time.second = 0u;
          bound_time.second_part = 0u;
          if (_handle->debug)
            std::cerr << "bound values: " << bound_time.year << '-' << bound_time.month << '-' << bound_time.day << 'T'
                      << bound_time.hour << ':' << bound_time.minute << ':' << bound_time.second << std::endl;
        }

        _handle->stmt_param_is_null[index] = is_null;
        MYSQL_BIND& param{_handle->stmt_params[index]};
        param.buffer_type = MYSQL_TYPE_DATE;
        param.buffer = &bound_time;
        param.buffer_length = sizeof(MYSQL_TIME);
        param.length = &param.buffer_length;
        param.is_null = &_handle->stmt_param_is_null[index].value;
        param.is_unsigned = false;
        param.error = nullptr;
      }

      void _bind_date_time_parameter(size_t index, const ::sqlpp::chrono::microsecond_point* value, bool is_null)
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: binding date_time parameter "
                    << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;

        auto& bound_time = _handle->stmt_date_time_param_buffer[index];
        if (not is_null)
        {
          const auto dp = ::sqlpp::chrono::floor<::date::days>(*value);
          const auto time = ::date::make_time(*value - dp);
          const auto ymd = ::date::year_month_day{dp};
          bound_time.year = static_cast<unsigned>(std::abs(static_cast<int>(ymd.year())));
          bound_time.month = static_cast<unsigned>(ymd.month());
          bound_time.day = static_cast<unsigned>(ymd.day());
          bound_time.hour = static_cast<unsigned>(time.hours().count());
          bound_time.minute = static_cast<unsigned>(time.minutes().count());
          bound_time.second = static_cast<unsigned>(time.seconds().count());
          bound_time.second_part = static_cast<unsigned long>(time.subseconds().count());
          if (_handle->debug)
            std::cerr << "bound values: " << bound_time.year << '-' << bound_time.month << '-' << bound_time.day << 'T'
                      << bound_time.hour << ':' << bound_time.minute << ':' << bound_time.second << std::endl;
        }

        _handle->stmt_param_is_null[index] = is_null;
        MYSQL_BIND& param{_handle->stmt_params[index]};
        param.buffer_type = MYSQL_TYPE_DATETIME;
        param.buffer = &bound_time;
        param.buffer_length = sizeof(MYSQL_TIME);
        param.length = &param.buffer_length;
        param.is_null = &_handle->stmt_param_is_null[index].value;
        param.is_unsigned = false;
        param.error = nullptr;
      }

      void _bind_time_of_day_parameter(size_t index, const ::std::chrono::microseconds* value, bool is_null)
      {
        if (_handle->debug)
          std::cerr << "MySQL debug: binding time_of_day parameter "
                    << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;

        auto& bound_time = _handle->stmt_date_time_param_buffer[index];
        if (not is_null)
        {
          const auto time = ::date::make_time(*value);
          bound_time.year = 0u;
          bound_time.month = 0u;
          bound_time.day = 0u;
          bound_time.hour = static_cast<unsigned>(time.hours().count());
          bound_time.minute = static_cast<unsigned>(time.minutes().count());
          bound_time.second = static_cast<unsigned>(time.seconds().count());
          bound_time.second_part = static_cast<unsigned long>(time.subseconds().count());
          if (_handle->debug)
            std::cerr << "bound values: " << bound_time.hour << ':' << bound_time.minute << ':' << bound_time.second
                      << '.' << bound_time.second_part << std::endl;
        }

        _handle->stmt_param_is_null[index] = is_null;
        MYSQL_BIND& param{_handle->stmt_params[index]};
        param.buffer_type = MYSQL_TYPE_TIME;
        param.buffer = &bound_time;
        param.buffer_length = sizeof(MYSQL_TIME);
        param.length = &param.buffer_length;
        param.is_null = &_handle->stmt_param_is_null[index].value;
        param.is_unsigned = false;
        param.error = nullptr;
      }
    };
  }  // namespace mysql
}  // namespace sqlpp

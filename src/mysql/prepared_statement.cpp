/*
 * Copyright (c) 2013 - 2016, Roland Bock
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

#ifdef _WIN32
// It seems that Windows.h will be included directly and indirectly.
// These defines prevent min/max macros and a bunch of other stuff
// to be defined in that header.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#endif  // _WIN32

#include "detail/prepared_statement_handle.h"
#include <ciso646>
#include <date/date.h>
#include <iostream>
#include <sqlpp11/mysql/prepared_statement.h>
#include <string>

namespace sqlpp
{
  namespace mysql
  {
    prepared_statement_t::prepared_statement_t(std::shared_ptr<detail::prepared_statement_handle_t>&& handle)
        : _handle(std::move(handle))
    {
      if (_handle and _handle->debug)
        std::cerr << "MySQL debug: Constructing prepared_statement, using handle at " << _handle.get() << std::endl;
    }

    void prepared_statement_t::_bind_boolean_parameter(size_t index, const signed char* value, bool is_null)
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: binding boolean parameter " << (*value ? "true" : "false") << " at index: " << index
                  << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
      _handle->stmt_param_is_null[index] = is_null;
      MYSQL_BIND& param = _handle->stmt_params[index];
      param.buffer_type = MYSQL_TYPE_TINY;
      param.buffer = const_cast<signed char*>(value);
      param.buffer_length = sizeof(*value);
      param.length = &param.buffer_length;
      param.is_null = &_handle->stmt_param_is_null[index].value;
      param.is_unsigned = false;
      param.error = nullptr;
    }

    void prepared_statement_t::_bind_integral_parameter(size_t index, const int64_t* value, bool is_null)
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: binding integral parameter " << *value << " at index: " << index << ", being "
                  << (is_null ? "" : "not ") << "null" << std::endl;
      _handle->stmt_param_is_null[index] = is_null;
      MYSQL_BIND& param = _handle->stmt_params[index];
      param.buffer_type = MYSQL_TYPE_LONGLONG;
      param.buffer = const_cast<int64_t*>(value);
      param.buffer_length = sizeof(*value);
      param.length = &param.buffer_length;
      param.is_null = &_handle->stmt_param_is_null[index].value;
      param.is_unsigned = false;
      param.error = nullptr;
    }

    void prepared_statement_t::_bind_unsigned_integral_parameter(size_t index, const uint64_t* value, bool is_null)
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: binding unsigned integral parameter " << *value << " at index: " << index
                  << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
      _handle->stmt_param_is_null[index] = is_null;
      MYSQL_BIND& param = _handle->stmt_params[index];
      param.buffer_type = MYSQL_TYPE_LONGLONG;
      param.buffer = const_cast<uint64_t*>(value);
      param.buffer_length = sizeof(*value);
      param.length = &param.buffer_length;
      param.is_null = &_handle->stmt_param_is_null[index].value;
      param.is_unsigned = true;
      param.error = nullptr;
    }

    void prepared_statement_t::_bind_floating_point_parameter(size_t index, const double* value, bool is_null)
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: binding floating_point parameter " << *value << " at index: " << index << ", being "
                  << (is_null ? "" : "not ") << "null" << std::endl;
      _handle->stmt_param_is_null[index] = is_null;
      MYSQL_BIND& param = _handle->stmt_params[index];
      param.buffer_type = MYSQL_TYPE_DOUBLE;
      param.buffer = const_cast<double*>(value);
      param.buffer_length = sizeof(*value);
      param.length = &param.buffer_length;
      param.is_null = &_handle->stmt_param_is_null[index].value;
      param.is_unsigned = false;
      param.error = nullptr;
    }

    void prepared_statement_t::_bind_text_parameter(size_t index, const std::string* value, bool is_null)
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: binding text parameter " << *value << " at index: " << index << ", being "
                  << (is_null ? "" : "not ") << "null" << std::endl;
      _handle->stmt_param_is_null[index] = is_null;
      MYSQL_BIND& param = _handle->stmt_params[index];
      param.buffer_type = MYSQL_TYPE_STRING;
      param.buffer = const_cast<char*>(value->data());
      param.buffer_length = value->size();
      param.length = &param.buffer_length;
      param.is_null = &_handle->stmt_param_is_null[index].value;
      param.is_unsigned = false;
      param.error = nullptr;
    }

    void prepared_statement_t::_bind_date_parameter(size_t index, const ::sqlpp::chrono::day_point* value, bool is_null)
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: binding date parameter "
                  << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;

      auto& bound_time = _handle->stmt_date_time_param_buffer[index];
      if (not is_null)
      {
        const auto ymd = ::date::year_month_day{*value};
        bound_time.year = static_cast<int>(ymd.year());
        bound_time.month = static_cast<unsigned>(ymd.month());
        bound_time.day = static_cast<unsigned>(ymd.day());
        if (_handle->debug)
          std::cerr << "bound values: " << bound_time.year << '-' << bound_time.month << '-' << bound_time.day << 'T'
                    << bound_time.hour << ':' << bound_time.minute << ':' << bound_time.second << std::endl;
      }

      _handle->stmt_param_is_null[index] = is_null;
      MYSQL_BIND& param = _handle->stmt_params[index];
      param.buffer_type = MYSQL_TYPE_DATE;
      param.buffer = &bound_time;
      param.buffer_length = sizeof(MYSQL_TIME);
      param.length = &param.buffer_length;
      param.is_null = &_handle->stmt_param_is_null[index].value;
      param.is_unsigned = false;
      param.error = nullptr;
    }

    void prepared_statement_t::_bind_date_time_parameter(size_t index,
                                                         const ::sqlpp::chrono::microsecond_point* value,
                                                         bool is_null)
    {
      if (_handle->debug)
        std::cerr << "MySQL debug: binding date_time parameter "
                  << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;

      auto& bound_time = _handle->stmt_date_time_param_buffer[index];
      if (not is_null)
      {
        const auto dp = ::sqlpp::chrono::floor<::date::days>(*value);
        const auto time = date::make_time(*value - dp);
        const auto ymd = ::date::year_month_day{dp};
        bound_time.year = static_cast<int>(ymd.year());
        bound_time.month = static_cast<unsigned>(ymd.month());
        bound_time.day = static_cast<unsigned>(ymd.day());
        bound_time.hour = time.hours().count();
        bound_time.minute = time.minutes().count();
        bound_time.second = time.seconds().count();
        bound_time.second_part = time.subseconds().count();
        if (_handle->debug)
          std::cerr << "bound values: " << bound_time.year << '-' << bound_time.month << '-' << bound_time.day << 'T'
                    << bound_time.hour << ':' << bound_time.minute << ':' << bound_time.second << std::endl;
      }

      _handle->stmt_param_is_null[index] = is_null;
      MYSQL_BIND& param = _handle->stmt_params[index];
      param.buffer_type = MYSQL_TYPE_DATETIME;
      param.buffer = &bound_time;
      param.buffer_length = sizeof(MYSQL_TIME);
      param.length = &param.buffer_length;
      param.is_null = &_handle->stmt_param_is_null[index].value;
      param.is_unsigned = false;
      param.error = nullptr;
    }
  }
}

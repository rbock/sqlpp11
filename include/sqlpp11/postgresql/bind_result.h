/**
 * Copyright © 2014-2020, Matthijs Möhlmann
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

#ifndef SQLPP_POSTGRESQL_BIND_RESULT_H
#define SQLPP_POSTGRESQL_BIND_RESULT_H

#include <memory>
#include <sqlpp11/chrono.h>
#include <sqlpp11/data_types.h>

#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

#include "detail/prepared_statement_handle.h"

#ifdef _MSC_VER
#include <iso646.h>
#pragma warning(disable : 4800)  // int to bool
#endif
namespace sqlpp
{
  namespace postgresql
  {
    namespace detail
    {
      struct statement_handle_t;
    }

    class bind_result_t
    {
    private:
      std::shared_ptr<detail::statement_handle_t> _handle;

      bool next_impl();

    public:
      bind_result_t() = default;
      bind_result_t(const std::shared_ptr<detail::statement_handle_t>& handle);
      bind_result_t(const bind_result_t&) = delete;
      bind_result_t(bind_result_t&&) = default;
      bind_result_t& operator=(const bind_result_t&) = delete;
      bind_result_t& operator=(bind_result_t&&) = default;
      ~bind_result_t() = default;

      bool operator==(const bind_result_t& rhs) const
      {
        return (this->_handle == rhs._handle);
      }

      template <typename ResultRow>
      void next(ResultRow& result_row)
      {
        if (!this->_handle)
        {
          result_row._invalidate();
          return;
        }

        if (this->next_impl())
        {
          if (!result_row)
          {
            result_row._validate();
          }
          result_row._bind(*this);
        }
        else
        {
          if (result_row)
          {
            result_row._invalidate();
          }
        }
      }

      void _bind_boolean_result(size_t index, signed char* value, bool* is_null);
      void _bind_floating_point_result(size_t index, double* value, bool* is_null);
      void _bind_integral_result(size_t index, int64_t* value, bool* is_null);
      void _bind_unsigned_integral_result(size_t index, uint64_t* value, bool* is_null);
      void _bind_text_result(size_t index, const char** value, size_t* len);
      void _bind_date_result(size_t index, ::sqlpp::chrono::day_point* value, bool* is_null);
      void _bind_date_time_result(size_t index, ::sqlpp::chrono::microsecond_point* value, bool* is_null);
      void _bind_time_of_day_result(size_t index, ::std::chrono::microseconds* value, bool* is_null);
      void _bind_blob_result(size_t index, const uint8_t** value, size_t* len);

      int size() const;
    };

    inline bind_result_t::bind_result_t(const std::shared_ptr<detail::statement_handle_t>& handle) : _handle(handle)
    {
      if (this->_handle && this->_handle->debug())
      {
        // cerr
        std::cerr << "PostgreSQL debug: constructing bind result, using handle at: " << this->_handle.get()
                  << std::endl;
      }
    }

    inline bool bind_result_t::next_impl()
    {
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: accessing next row of handle at " << _handle.get() << std::endl;
      }

      // Fetch total amount
      if (_handle->totalCount == 0U)
      {
        _handle->totalCount = _handle->result.records_size();
        if (_handle->totalCount == 0U)
          return false;
      }
      else
      {
        // Next row
        if (_handle->count < (_handle->totalCount - 1))
        {
          _handle->count++;
        }
        else
        {
          return false;
        }
      }

      // Really needed?
      if (_handle->fields == 0U)
      {
        _handle->fields = _handle->result.field_count();
      }

      return true;
    }

    inline void bind_result_t::_bind_boolean_result(size_t _index, signed char* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding boolean result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);
      *value = _handle->result.getBoolValue(_handle->count, index);
    }

    inline void bind_result_t::_bind_floating_point_result(size_t _index, double* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding floating_point result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);
      *value = _handle->result.getDoubleValue(_handle->count, index);
    }

    inline void bind_result_t::_bind_integral_result(size_t _index, int64_t* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding integral result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);
      *value = _handle->result.getInt64Value(_handle->count, index);
    }

    inline void bind_result_t::_bind_unsigned_integral_result(size_t _index, uint64_t* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding unsigned integral result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);
      *value = _handle->result.getUInt64Value(_handle->count, index);
    }

    inline void bind_result_t::_bind_text_result(size_t _index, const char** value, size_t* len)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding text result at index: " << index << std::endl;
      }

      if (_handle->result.isNull(_handle->count, index))
      {
        *value = nullptr;
        *len = 0;
      }
      else
      {
        *value = _handle->result.getCharPtrValue(_handle->count, index);
        *len = static_cast<size_t>(_handle->result.length(_handle->count, index));
      }
    }

    // PostgreSQL will return one of those (using the default ISO client):
    //
    // 2010-10-11 01:02:03 - ISO timestamp without timezone
    // 2011-11-12 01:02:03.123456 - ISO timesapt with sub-second (microsecond) precision
    // 1997-12-17 07:37:16-08 - ISO timestamp with timezone
    // 1992-10-10 01:02:03-06:30 - for some timezones with non-hour offset
    // 1900-01-01 - date only
    // we do not support time-only values !

    inline void bind_result_t::_bind_date_result(size_t _index, ::sqlpp::chrono::day_point* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);

      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding date result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);

      if (!(*is_null))
      {
        const auto date_string = _handle->result.getCharPtrValue(_handle->count, index);

        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: date string: " << date_string << std::endl;
        }

        static const std::regex rx {"(\\d{4})-(\\d{2})-(\\d{2})"};
        std::cmatch mr;
        if (std::regex_match (date_string, mr, rx)) {
          *value =
            ::sqlpp::chrono::day_point{
              ::date::year{std::atoi(date_string + mr.position(1))} / // Year
              std::atoi(date_string + mr.position(2)) /               // Month
              std::atoi(date_string + mr.position(3))                 // Day of month
            };
        } else {
          if (_handle->debug()) {
            std::cerr << "PostgreSQL debug: got invalid date '" << date_string << "'" << std::endl;
          }
          *value = {};
        }
      }
      else
      {
        *value = {};
      }
    }

    // always returns UTC time for timestamp with time zone
    inline void bind_result_t::_bind_date_time_result(size_t _index, ::sqlpp::chrono::microsecond_point* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding date_time result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);

      if (!(*is_null))
      {
        const auto date_string = _handle->result.getCharPtrValue(_handle->count, index);

        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: got date_time string: " << date_string << std::endl;
        }

        static const std::regex rx {
          "(\\d{4})-(\\d{2})-(\\d{2}) "
          "(\\d{2}):(\\d{2}):(\\d{2})(?:\\.(\\d{1,6}))?"
          "(?:([+-])(\\d{2})(?::(\\d{2})(?::(\\d{2}))?)?)?"
        };
        std::cmatch mr;
        if (std::regex_match (date_string, mr, rx)) {
          *value =
            ::sqlpp::chrono::day_point{
              ::date::year{std::atoi(date_string + mr.position(1))} / // Year
              std::atoi(date_string + mr.position(2)) /               // Month
              std::atoi(date_string + mr.position(3))                 // Day of month
            } +
            std::chrono::hours{std::atoi(date_string + mr.position(4))} +     // Hour
            std::chrono::minutes{std::atoi(date_string + mr.position(5))} +   // Minute
            std::chrono::seconds{std::atoi(date_string + mr.position(6))} +   // Second
            ::std::chrono::microseconds{                                      // Microsecond
              mr[7].matched ? std::stoi((mr[7].str() + "000000").substr(0, 6)) : 0
            };
            if (mr[8].matched) {
              const auto tz_sign = (date_string[mr.position(8)] == '+') ? 1 : -1;
              const auto tz_offset =
                std::chrono::hours{std::atoi(date_string + mr.position(9))} +
                std::chrono::minutes{mr[10].matched ? std::atoi(date_string + mr.position(10)) : 0} +
                std::chrono::seconds{mr[11].matched ? std::atoi(date_string + mr.position(11)) : 0};
              *value -= tz_sign * tz_offset;
            }
        } else {
          if (_handle->debug()) {
            std::cerr << "PostgreSQL debug: got invalid date_time '" << date_string << "'" << std::endl;
          }
          *value = {};
        }
      }
    }

    // always returns UTC time for time with time zone
    inline void bind_result_t::_bind_time_of_day_result(size_t _index, ::std::chrono::microseconds* value, bool* is_null)
    {
      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding time result at index: " << index << std::endl;
      }

      *is_null = _handle->result.isNull(_handle->count, index);

      if (!(*is_null))
      {
        const auto time_string = _handle->result.getCharPtrValue(_handle->count, index);

        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: got time string: " << time_string << std::endl;
        }

        static const std::regex rx {
          "(\\d{2}):(\\d{2}):(\\d{2})(?:\\.(\\d{1,6}))?"
          "(?:([+-])(\\d{2})(?::(\\d{2})(?::(\\d{2}))?)?)?"
        };
        std::cmatch mr;
        if (std::regex_match (time_string, mr, rx)) {
          *value =
            std::chrono::hours{std::atoi(time_string + mr.position(1))} +     // Hour
            std::chrono::minutes{std::atoi(time_string + mr.position(2))} +   // Minute
            std::chrono::seconds{std::atoi(time_string + mr.position(3))} +   // Second
            ::std::chrono::microseconds{                                      // Microsecond
              mr[4].matched ? std::stoi((mr[4].str() + "000000").substr(0, 6)) : 0
            };
            if (mr[5].matched) {
              const auto tz_sign = (time_string[mr.position(5)] == '+') ? 1 : -1;
              const auto tz_offset =
                std::chrono::hours{std::atoi(time_string + mr.position(6))} +
                std::chrono::minutes{mr[7].matched ? std::atoi(time_string + mr.position(7)) : 0} +
                std::chrono::seconds{mr[8].matched ? std::atoi(time_string + mr.position(8)) : 0};
              *value -= tz_sign * tz_offset;
            }
        } else {
          if (_handle->debug()) {
            std::cerr << "PostgreSQL debug: got invalid time '" << time_string << "'" << std::endl;
          }
          *value = {};
        }
      }
    }

    inline void bind_result_t::_bind_blob_result(size_t _index, const uint8_t** value, size_t* len)
    {

      auto index = static_cast<int>(_index);
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding blob result at index: " << index << std::endl;
      }

      if (_handle->result.isNull(_handle->count, index))
      {
        *value = nullptr;
        *len = 0;
      }
      else
      {
        *value = _handle->result.getBlobValue(_handle->count, index);
        *len   = static_cast<size_t>(_handle->result.length(_handle->count, index));
      }
    }

    inline int bind_result_t::size() const
    {
      return _handle->result.records_size();
    }
  }  // namespace postgresql
}  // namespace sqlpp

#endif

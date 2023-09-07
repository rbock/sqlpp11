#pragma once

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

#include <sqlpp11/chrono.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/detail/parse_date_time.h>

#include <iomanip>
#include <iostream>
#include <memory>
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

      bool next_impl()
      {
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: accessing next row of handle at " << _handle.get() << std::endl;
        }

        // Fetch total amount
        if (_handle->total_count == 0U)
        {
          _handle->total_count = _handle->result.records_size();
          if (_handle->total_count == 0U)
            return false;
        }
        else
        {
          // Next row
          if (_handle->count < (_handle->total_count - 1))
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

    public:
      bind_result_t() = default;

      bind_result_t(const std::shared_ptr<detail::statement_handle_t>& handle) : _handle(handle)
      {
        if (this->_handle && this->_handle->debug())
        {
          // cerr
          std::cerr << "PostgreSQL debug: constructing bind result, using handle at: " << this->_handle.get()
                    << std::endl;
        }
      }

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

      void _bind_boolean_result(size_t _index, signed char* value, bool* is_null)
      {
        auto index = static_cast<int>(_index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding boolean result at index: " << index << std::endl;
        }

        *is_null = _handle->result.is_null(_handle->count, index);
        *value = _handle->result.get_bool_value(_handle->count, index);
      }

      void _bind_floating_point_result(size_t _index, double* value, bool* is_null)
      {
        auto index = static_cast<int>(_index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding floating_point result at index: " << index << std::endl;
        }

        *is_null = _handle->result.is_null(_handle->count, index);
        *value = _handle->result.get_double_value(_handle->count, index);
      }

      void _bind_integral_result(size_t _index, int64_t* value, bool* is_null)
      {
        auto index = static_cast<int>(_index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding integral result at index: " << index << std::endl;
        }

        *is_null = _handle->result.is_null(_handle->count, index);
        *value = _handle->result.get_int64_value(_handle->count, index);
      }

      void _bind_unsigned_integral_result(size_t _index, uint64_t* value, bool* is_null)
      {
        auto index = static_cast<int>(_index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding unsigned integral result at index: " << index << std::endl;
        }

        *is_null = _handle->result.is_null(_handle->count, index);
        *value = _handle->result.get_uint64_value(_handle->count, index);
      }

      void _bind_text_result(size_t _index, const char** value, size_t* len)
      {
        auto index = static_cast<int>(_index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding text result at index: " << index << std::endl;
        }

        if (_handle->result.is_null(_handle->count, index))
        {
          *value = nullptr;
          *len = 0;
        }
        else
        {
          *value = _handle->result.get_char_ptr_value(_handle->count, index);
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
      void _bind_date_result(size_t _index, ::sqlpp::chrono::day_point* value, bool* is_null)
      {
        auto index = static_cast<int>(_index);

        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding date result at index: " << index << std::endl;
        }

        *value = {};
        *is_null = _handle->result.is_null(_handle->count, index);
        if (*is_null)
        {
          return;
        }

        const auto date_string = _handle->result.get_char_ptr_value(_handle->count, index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: date string: " << date_string << std::endl;
        }
        if (::sqlpp::detail::parse_date(*value, date_string) == false)
        {
          if (_handle->debug())
          {
            std::cerr << "PostgreSQL debug: got invalid date '" << date_string << "'" << std::endl;
          }
        }
      }

      // always returns UTC time for timestamp with time zone
      void _bind_date_time_result(size_t _index, ::sqlpp::chrono::microsecond_point* value, bool* is_null)
      {
        auto index = static_cast<int>(_index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding date_time result at index: " << index << std::endl;
        }

        *value = {};
        *is_null = _handle->result.is_null(_handle->count, index);
        if (*is_null)
        {
          return;
        }

        const auto date_string = _handle->result.get_char_ptr_value(_handle->count, index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: got date_time string: " << date_string << std::endl;
        }
        if (::sqlpp::detail::parse_timestamp(*value, date_string) == false)
        {
          if (_handle->debug())
          {
            std::cerr << "PostgreSQL debug: got invalid date_time '" << date_string << "'" << std::endl;
          }
        }
      }

      // always returns UTC time for time with time zone
      void _bind_time_of_day_result(size_t _index, ::std::chrono::microseconds* value, bool* is_null)
      {
        auto index = static_cast<int>(_index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding time result at index: " << index << std::endl;
        }

        *value = {};
        *is_null = _handle->result.is_null(_handle->count, index);
        if (*is_null)
        {
          return;
        }

        const auto time_string = _handle->result.get_char_ptr_value(_handle->count, index);

        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: got time string: " << time_string << std::endl;
        }

        if (::sqlpp::detail::parse_time_of_day(*value, time_string) == false)
        {
          if (_handle->debug()) {
            std::cerr << "PostgreSQL debug: got invalid time '" << time_string << "'" << std::endl;
          }
        }
      }

      void _bind_blob_result(size_t _index, const uint8_t** value, size_t* len)
      {
        auto index = static_cast<int>(_index);
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding blob result at index: " << index << std::endl;
        }

        if (_handle->result.is_null(_handle->count, index))
        {
          *value = nullptr;
          *len = 0;
        }
        else
        {
          *value = _handle->result.get_blob_value(_handle->count, index);
          *len   = static_cast<size_t>(_handle->result.length(_handle->count, index));
        }
      }

      int size() const
      {
        return _handle->result.records_size();
      }
    };
  }  // namespace postgresql
}  // namespace sqlpp

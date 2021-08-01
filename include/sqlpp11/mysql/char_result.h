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
#include <memory>
#include <sqlpp11/chrono.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/mysql/char_result_row.h>

namespace sqlpp
{
  namespace mysql
  {
    namespace detail
    {
      struct result_handle;
    }

    class char_result_t
    {
      std::unique_ptr<detail::result_handle> _handle;
      char_result_row_t _char_result_row;

    public:
      char_result_t();
      char_result_t(std::unique_ptr<detail::result_handle>&& handle);
      char_result_t(const char_result_t&) = delete;
      char_result_t(char_result_t&& rhs);
      char_result_t& operator=(const char_result_t&) = delete;
      char_result_t& operator=(char_result_t&&);
      ~char_result_t();

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

      bool _invalid() const;

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

      void _bind_date_result(size_t index, ::sqlpp::chrono::day_point* value, bool* is_null);
      void _bind_date_time_result(size_t index, ::sqlpp::chrono::microsecond_point* value, bool* is_null);

    private:
      bool next_impl();
    };
  }
}
#endif

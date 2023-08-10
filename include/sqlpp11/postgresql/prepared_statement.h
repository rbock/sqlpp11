#pragma once

/**
 * Copyright © 2014-2015, Matthijs Möhlmann
 * Copyright © 2021-2021, Roland Bock
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

#include <memory>
#include <string>
#include <sqlpp11/chrono.h>
#include <sqlpp11/detail/float_safe_ostringstream.h>
#include <sqlpp11/postgresql/exception.h>

namespace sqlpp
{
  namespace postgresql
  {
#ifdef SQLPP_DYNAMIC_LOADING
    using namespace dynamic;
#endif

    // Forward declaration
    class connection_base;

    // Detail namespace
    namespace detail
    {
      struct prepared_statement_handle_t;
    }

    class prepared_statement_t
    {
    private:
      friend class sqlpp::postgresql::connection_base;

      std::shared_ptr<detail::prepared_statement_handle_t> _handle;

    public:
      prepared_statement_t() = default;

      // ctor
      prepared_statement_t(std::shared_ptr<detail::prepared_statement_handle_t>&& handle)
          : _handle{handle}
      {
        if (_handle && _handle->debug())
        {
          std::cerr << "PostgreSQL debug: constructing prepared_statement, using handle at: " << _handle.get()
                    << std::endl;
        }
      }

      prepared_statement_t(const prepared_statement_t&) = delete;
      prepared_statement_t(prepared_statement_t&&) = default;
      prepared_statement_t& operator=(const prepared_statement_t&) = delete;
      prepared_statement_t& operator=(prepared_statement_t&&) = default;
      ~prepared_statement_t() = default;

      bool operator==(const prepared_statement_t& rhs)
      {
        return (this->_handle == rhs._handle);
      }

      void _bind_boolean_parameter(size_t index, const signed char* value, bool is_null)
      {
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding boolean parameter " << (*value ? "true" : "false")
                    << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
        }

        _handle->null_values[index] = is_null;
        if (!is_null)
        {
          if (*value)
          {
            _handle->param_values[index] = "TRUE";
          }
          else
          {
            _handle->param_values[index] = "FALSE";
          }
        }
      }

      void _bind_floating_point_parameter(size_t index, const double* value, bool is_null)
      {
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding floating_point parameter " << *value << " at index: " << index
                    << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
        }

        _handle->null_values[index] = is_null;
        if (!is_null)
        {
          sqlpp::detail::float_safe_ostringstream out;
          out << *value;
          _handle->param_values[index] = out.str();
        }
      }

      void _bind_integral_parameter(size_t index, const int64_t* value, bool is_null)
      {
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding integral parameter " << *value << " at index: " << index << ", being "
                    << (is_null ? "" : "not ") << "null" << std::endl;
        }

        // Assign values
        _handle->null_values[index] = is_null;
        if (!is_null)
        {
          _handle->param_values[index] = std::to_string(*value);
        }
      }

      void _bind_text_parameter(size_t index, const std::string* value, bool is_null)
      {
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding text parameter " << *value << " at index: " << index << ", being "
                    << (is_null ? "" : "not ") << "null" << std::endl;
        }

        // Assign values
        _handle->null_values[index] = is_null;
        if (!is_null)
        {
          _handle->param_values[index] = *value;
        }
      }

      void _bind_date_parameter(size_t index, const ::sqlpp::chrono::day_point* value, bool is_null)
      {
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding date parameter at index "
                    << index << ", being " << (is_null ? "" : "not ") << "null" <<  std::endl;
        }
        _handle->null_values[index] = is_null;
        if (not is_null)
        {
          const auto ymd = ::date::year_month_day{*value};
          std::ostringstream os;
          os << ymd;
          _handle->param_values[index] = os.str();

          if (_handle->debug())
          {
            std::cerr << "PostgreSQL debug: binding date parameter string: " << _handle->param_values[index] << std::endl;
          }
        }
      }

      void _bind_time_of_day_parameter(size_t index, const ::std::chrono::microseconds* value, bool is_null)
      {
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding time parameter at index "
                    << index << ", being " << (is_null ? "" : "not ") << "null" <<  std::endl;
        }
        _handle->null_values[index] = is_null;
        if (not is_null)
        {
          const auto time = ::date::make_time(*value) ;

          // Timezone handling - always treat the local value as UTC.
          std::ostringstream os;
          os << time << "+00";
          _handle->param_values[index] = os.str();
          if (_handle->debug())
          {
            std::cerr << "PostgreSQL debug: binding time parameter string: " << _handle->param_values[index] << std::endl;
          }
        }
      }

      void _bind_date_time_parameter(size_t index, const ::sqlpp::chrono::microsecond_point* value, bool is_null)
      {
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding date_time parameter at index "
            << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
        }
        _handle->null_values[index] = is_null;
        if (not is_null)
        {
          const auto dp = ::sqlpp::chrono::floor<::date::days>(*value);
          const auto time = ::date::make_time(::sqlpp::chrono::floor<::std::chrono::microseconds>(*value - dp));
          const auto ymd = ::date::year_month_day{dp};

          // Timezone handling - always treat the local value as UTC.
          std::ostringstream os;
          os << ymd << ' ' << time << "+00";
          _handle->param_values[index] = os.str();
          if (_handle->debug())
          {
            std::cerr << "PostgreSQL debug: binding date_time parameter string: " << _handle->param_values[index] << std::endl;
          }
        }
      }

      void _bind_blob_parameter(size_t index, const std::vector<unsigned char>* value, bool is_null)
      {
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding blob parameter at index "
            << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
        }
        _handle->null_values[index] = is_null;
        if (not is_null)
        {
          constexpr char hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
          auto param = std::string(value->size() * 2 + 2, '\0');  // ()-init for correct constructor
          param[0] = '\\';
          param[1] = 'x';
          auto i = size_t{1};
          for (const auto c : *value)
          {
            param[++i] = hex_chars[c >> 4];
            param[++i] = hex_chars[c & 0x0F];
          }
          _handle->param_values[index] = std::move(param);
          if (_handle->debug())
          {
            std::cerr << "PostgreSQL debug: binding blob parameter string (up to 100 chars): " << _handle->param_values[index].substr(0, 100) << std::endl;
          }
        }
      }
    };
  }
}

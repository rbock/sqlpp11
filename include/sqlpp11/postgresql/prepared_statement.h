/**
 * Copyright © 2014-2015, Matthijs Möhlmann
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

#ifndef SQLPP_POSTGRESQL_PREPARED_STATEMENT_H
#define SQLPP_POSTGRESQL_PREPARED_STATEMENT_H

#include <memory>
#include <string>
#include <sqlpp11/chrono.h>

#include <sqlpp11/postgresql/exception.h>

namespace sqlpp
{
  namespace postgresql
  {
#ifdef SQLPP_DYNAMIC_LOADING
    using namespace dynamic;
#endif

    // Forward declaration
    class connection;

    // Detail namespace
    namespace detail
    {
      struct prepared_statement_handle_t;
    }

    class prepared_statement_t
    {
      friend sqlpp::postgresql::connection;

    private:
      std::shared_ptr<detail::prepared_statement_handle_t> _handle;

    public:
      prepared_statement_t() = default;
      prepared_statement_t(std::shared_ptr<detail::prepared_statement_handle_t>&& handle);
      prepared_statement_t(const prepared_statement_t&) = delete;
      prepared_statement_t(prepared_statement_t&&) = default;
      prepared_statement_t& operator=(const prepared_statement_t&) = delete;
      prepared_statement_t& operator=(prepared_statement_t&&) = default;
      ~prepared_statement_t() = default;

      bool operator==(const prepared_statement_t& rhs)
      {
        return (this->_handle == rhs._handle);
      }

      void _bind_boolean_parameter(size_t index, const signed char* value, bool is_null);
      void _bind_floating_point_parameter(size_t index, const double* value, bool is_null);
      void _bind_integral_parameter(size_t index, const int64_t* value, bool is_null);
      void _bind_text_parameter(size_t index, const std::string* value, bool is_null);
      void _bind_date_parameter(size_t index, const ::sqlpp::chrono::day_point* value, bool is_null);
      void _bind_date_time_parameter(size_t index, const ::sqlpp::chrono::microsecond_point* value, bool is_null);
    };

    // ctor
    inline prepared_statement_t::prepared_statement_t(std::shared_ptr<detail::prepared_statement_handle_t>&& handle)
        : _handle{handle}
    {
      if (_handle && _handle->debug())
      {
        std::cerr << "PostgreSQL debug: constructing prepared_statement, using handle at: " << _handle.get()
                  << std::endl;
      }
    }

    inline void prepared_statement_t::_bind_boolean_parameter(size_t index, const signed char* value, bool is_null)
    {
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding boolean parameter " << (*value ? "true" : "false")
                  << " at index: " << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
      }

      _handle->nullValues[index] = is_null;
      if (!is_null)
      {
        if (*value)
        {
          _handle->paramValues[index] = "TRUE";
        }
        else
        {
          _handle->paramValues[index] = "FALSE";
        }
      }
    }

    inline void prepared_statement_t::_bind_floating_point_parameter(size_t index, const double* value, bool is_null)
    {
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding floating_point parameter " << *value << " at index: " << index
                  << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
      }

      _handle->nullValues[index] = is_null;
      if (!is_null)
      {
        std::ostringstream out;
        out.precision(std::numeric_limits<double>::digits10);
        out << std::fixed << *value;
        _handle->paramValues[index] = out.str();
      }
    }

    inline void prepared_statement_t::_bind_integral_parameter(size_t index, const int64_t* value, bool is_null)
    {
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding integral parameter " << *value << " at index: " << index << ", being "
                  << (is_null ? "" : "not ") << "null" << std::endl;
      }

      // Assign values
      _handle->nullValues[index] = is_null;
      if (!is_null)
      {
        _handle->paramValues[index] = std::to_string(*value);
      }
    }

    inline void prepared_statement_t::_bind_text_parameter(size_t index, const std::string* value, bool is_null)
    {
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding text parameter " << *value << " at index: " << index << ", being "
                  << (is_null ? "" : "not ") << "null" << std::endl;
      }

      // Assign values
      _handle->nullValues[index] = is_null;
      if (!is_null)
      {
        _handle->paramValues[index] = *value;
      }
    }

    inline void prepared_statement_t::_bind_date_parameter(size_t index, const ::sqlpp::chrono::day_point* value, bool is_null)
    {
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding date parameter at index "
                  << index << ", being " << (is_null ? "" : "not ") << "null" <<  std::endl;
      }
      _handle->nullValues[index] = is_null;
      if (not is_null)
      {
        const auto ymd = ::date::year_month_day{*value};
        std::ostringstream os;
        os << ymd;
        _handle->paramValues[index] = os.str();

        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding date parameter string: " << _handle->paramValues[index] << std::endl;
        }
      }
    }

    inline void prepared_statement_t::_bind_date_time_parameter(size_t index, const ::sqlpp::chrono::microsecond_point* value, bool is_null)
    {
      if (_handle->debug())
      {
        std::cerr << "PostgreSQL debug: binding date_time parameter at index "
          << index << ", being " << (is_null ? "" : "not ") << "null" << std::endl;
      }
      _handle->nullValues[index] = is_null;
      if (not is_null)
      {
        const auto dp = ::sqlpp::chrono::floor<::date::days>(*value);
        const auto time = ::date::make_time(::sqlpp::chrono::floor<::std::chrono::microseconds>(*value - dp));
        const auto ymd = ::date::year_month_day{dp};

        // Timezone handling - always treat the value as UTC.
        // It is assumed that the database timezone is set to UTC, too.
        std::ostringstream os;
        os << ymd << ' ' << time;
        _handle->paramValues[index] = os.str();
        if (_handle->debug())
        {
          std::cerr << "PostgreSQL debug: binding date_time parameter string: " << _handle->paramValues[index] << std::endl;
        }
      }
    }
  }
}

#endif

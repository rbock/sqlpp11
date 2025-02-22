#pragma once

/*
 * Copyright (c) 2013 - 2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
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

#include <chrono>
#include <ciso646>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include <sqlpp23/core/chrono.h>
#include <sqlpp23/core/database/exception.h>
#include <sqlpp23/sqlite3/export.h>

#include <sqlpp23/sqlite3/detail/prepared_statement_handle.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace sqlpp {
namespace sqlite3 {
// Forward declaration
class connection_base;

namespace detail {
inline void check_bind_result(int result, const char *const type) {
  switch (result) {
  case SQLITE_OK:
    return;
  case SQLITE_RANGE:
    throw sqlpp::exception{"Sqlite3 error: " + std::string(type) +
                           " bind value out of range"};
  case SQLITE_NOMEM:
    throw sqlpp::exception{"Sqlite3 error: " + std::string(type) +
                           " bind out of memory"};
  case SQLITE_TOOBIG:
    throw sqlpp::exception{"Sqlite3 error: " + std::string(type) +
                           " bind too big"};
  default:
    throw sqlpp::exception{
        "Sqlite3 error: " + std::string(type) +
        " bind returned unexpected value: " + std::to_string(result)};
  }
}
} // namespace detail

class SQLPP11_SQLITE3_EXPORT prepared_statement_t {
  friend class ::sqlpp::sqlite3::connection_base;
  std::shared_ptr<detail::prepared_statement_handle_t> _handle;

public:
  prepared_statement_t() = default;
  prepared_statement_t(
      std::shared_ptr<detail::prepared_statement_handle_t> &&handle)
      : _handle(std::move(handle)) {
    if (_handle and _handle->debug) {
      std::cerr << "Sqlite3 debug: Constructing prepared_statement, "
                   "clause/using.handle at "
                << _handle.get() << std::endl;
    }
  }
  prepared_statement_t(const prepared_statement_t &) = delete;
  prepared_statement_t(prepared_statement_t &&rhs) = default;
  prepared_statement_t &operator=(const prepared_statement_t &) = delete;
  prepared_statement_t &operator=(prepared_statement_t &&) = default;
  ~prepared_statement_t() = default;

  bool operator==(const prepared_statement_t &rhs) const {
    return _handle == rhs._handle;
  }

  void _reset() {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: resetting prepared statement" << std::endl;
    }
    sqlite3_reset(_handle->sqlite_statement);
  }

  void _bind_parameter(size_t index, const bool &value) {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding boolean parameter "
                << (value ? "true" : "false") << " at index: " << index
                << std::endl;
    }

    const int result = sqlite3_bind_int(_handle->sqlite_statement,
                                        static_cast<int>(index + 1), value);
    detail::check_bind_result(result, "boolean");
  }

  void _bind_parameter(size_t index, const double &value) {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding floating_point parameter " << value
                << " at index: " << index << std::endl;
    }

    int result;
    if (std::isnan(value))
      result = sqlite3_bind_text(_handle->sqlite_statement,
                                 static_cast<int>(index + 1), "NaN", 3,
                                 SQLITE_STATIC);
    else if (std::isinf(value)) {
      if (value > std::numeric_limits<double>::max())
        result = sqlite3_bind_text(_handle->sqlite_statement,
                                   static_cast<int>(index + 1), "Inf", 3,
                                   SQLITE_STATIC);
      else
        result = sqlite3_bind_text(_handle->sqlite_statement,
                                   static_cast<int>(index + 1), "-Inf", 4,
                                   SQLITE_STATIC);
    } else
      result = sqlite3_bind_double(_handle->sqlite_statement,
                                   static_cast<int>(index + 1), value);
    detail::check_bind_result(result, "floating_point");
  }

  void _bind_parameter(size_t index, const int64_t &value) {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding integral parameter " << value
                << " at index: " << index << std::endl;
    }

    const int result = sqlite3_bind_int64(_handle->sqlite_statement,
                                          static_cast<int>(index + 1), value);
    detail::check_bind_result(result, "integral");
  }

  void _bind_parameter(size_t index, const uint64_t &value) {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding unsigned integral parameter "
                << value << " at index: " << index << std::endl;
    }

    const int result = sqlite3_bind_int64(_handle->sqlite_statement,
                                          static_cast<int>(index + 1),
                                          static_cast<int64_t>(value));
    detail::check_bind_result(result, "integral");
  }

  void _bind_parameter(size_t index, const std::string &value) {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding text parameter " << value
                << " at index: " << index << std::endl;
    }

    const int result = sqlite3_bind_text(
        _handle->sqlite_statement, static_cast<int>(index + 1), value.data(),
        static_cast<int>(value.size()), SQLITE_STATIC);
    detail::check_bind_result(result, "text");
  }

  void _bind_parameter(size_t index, const std::chrono::microseconds &value) {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding time of day parameter "
                << " at index: " << index << std::endl;
    }

    const auto text = std::format("{0:%H:%M:%S}", value);
    const int result = sqlite3_bind_text(
        _handle->sqlite_statement, static_cast<int>(index + 1), text.data(),
        static_cast<int>(text.size()), SQLITE_TRANSIENT);
    detail::check_bind_result(result, "time_of_day");
  }

  void _bind_parameter(size_t index, const ::sqlpp::chrono::day_point &value) {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding date parameter "
                << " at index: " << index << std::endl;
    }

    const auto text = std::format("{0:%Y-%m-%d}", value);
    const int result = sqlite3_bind_text(
        _handle->sqlite_statement, static_cast<int>(index + 1), text.data(),
        static_cast<int>(text.size()), SQLITE_TRANSIENT);
    detail::check_bind_result(result, "date");
  }

  void _bind_parameter(size_t index,
                       const ::sqlpp::chrono::microsecond_point &value) {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding date_time parameter "
                << " at index: " << index << std::endl;
    }

    const auto text = std::format("{0:%Y-%m-%d %H:%M:%S}", value);
    const int result = sqlite3_bind_text(
        _handle->sqlite_statement, static_cast<int>(index + 1), text.data(),
        static_cast<int>(text.size()), SQLITE_TRANSIENT);
    detail::check_bind_result(result, "date_time");
  }

  void _bind_parameter(size_t index, const std::vector<uint8_t> &value) {
    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding vector parameter size of "
                << value.size() << " at index: " << index << std::endl;
    }

    const int result = sqlite3_bind_blob(
        _handle->sqlite_statement, static_cast<int>(index + 1), value.data(),
        static_cast<int>(value.size()), SQLITE_STATIC);
    detail::check_bind_result(result, "blob");
  }

  template <typename Parameter>
  void _bind_parameter(size_t index,
                       const std::optional<Parameter> &parameter) {
    if (parameter.has_value()) {
      _bind_parameter(index, parameter.value());
      return;
    }

    if (_handle->debug) {
      std::cerr << "Sqlite3 debug: binding NULL parameter at index: " << index
                << std::endl;
    }

    const int result = sqlite3_bind_null(_handle->sqlite_statement,
                                         static_cast<int>(index + 1));
    detail::check_bind_result(result, "NULL");
  }
};
} // namespace sqlite3
} // namespace sqlpp

#ifdef _MSC_VER
#pragma warning(pop)
#endif

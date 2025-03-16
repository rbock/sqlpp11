#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include <utility>

#include <sqlpp23/core/operator/enable_as.h>
#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename ValueType>
struct verbatim_t : public enable_as<verbatim_t<ValueType>> {
  verbatim_t(std::string verbatim) : _verbatim(std::move(verbatim)) {}
  verbatim_t(const verbatim_t&) = default;
  verbatim_t(verbatim_t&&) = default;
  verbatim_t& operator=(const verbatim_t&) = default;
  verbatim_t& operator=(verbatim_t&&) = default;
  ~verbatim_t() = default;

  std::string _verbatim;
};

template <typename ValueType>
struct is_clause<verbatim_t<ValueType>> : public std::true_type {};

template <typename Statement, typename ValueType>
struct consistency_check<Statement, verbatim_t<ValueType>> {
  using type = consistent_t;
};
template <typename ValueType>
struct value_type_of<verbatim_t<ValueType>> {
  // Since we do not know what's going on inside the verbatim, we assume it can
  // be null.
  using type = sqlpp::force_optional_t<ValueType>;
};

template <typename Context, typename ValueType>
auto to_sql_string(Context&, const verbatim_t<ValueType>& t) -> std::string {
  return t._verbatim;
}

template <typename ValueType, typename StringType>
auto verbatim(StringType s) -> verbatim_t<ValueType> {
  return {s};
}

inline auto verbatim(std::string s) -> verbatim_t<no_value_t> {
  return {std::move(s)};
}
}  // namespace sqlpp

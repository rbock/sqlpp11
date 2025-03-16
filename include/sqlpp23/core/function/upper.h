#pragma once

/*
 * Copyright (c) 2023, Roland Bock
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

#include <sqlpp23/core/operator/enable_as.h>
#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename Expr>
struct upper_t : public enable_as<upper_t<Expr>> {
  upper_t(const Expr expr) : _expr(expr) {}

  upper_t(const upper_t&) = default;
  upper_t(upper_t&&) = default;
  upper_t& operator=(const upper_t&) = default;
  upper_t& operator=(upper_t&&) = default;
  ~upper_t() = default;

  Expr _expr;
};

template <typename Expr>
struct value_type_of<upper_t<Expr>> : public value_type_of<Expr> {};

template <typename Expr>
struct nodes_of<upper_t<Expr>> {
  using type = detail::type_vector<Expr>;
};

template <typename Context, typename Expr>
auto to_sql_string(Context& context, const upper_t<Expr>& t) -> std::string {
  return "UPPER(" + to_sql_string(context, t._expr) + ")";
}

template <typename T>
using check_upper_args = std::enable_if_t<is_text<T>::value>;

template <typename T, typename = check_upper_args<T>>
auto upper(T t) -> upper_t<T> {
  return {std::move(t)};
}

}  // namespace sqlpp

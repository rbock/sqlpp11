#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/core/operator/enable_as.h>
#include <sqlpp11/core/type_traits/group_by_column.h>
#include <sqlpp11/core/type_traits/value_type.h>

namespace sqlpp {
// `group_by` takes columns as parameters.
// If you really, really want to, you can pass something else, but you have to
// tell the library that you are really certain by wrapping it in
// `declare_group_by_column`.

template <typename Expr>
struct group_by_column : public enable_as<group_by_column<Expr>> {
  group_by_column(Expr expr) : _expr(std::move(expr)) {}
  group_by_column(const group_by_column &) = default;
  group_by_column(group_by_column &&) = default;
  group_by_column &operator=(const group_by_column &) = default;
  group_by_column &operator=(group_by_column &&) = default;
  ~group_by_column() = default;

  Expr _expr;
};

template <typename KnownAggregateColumns, typename Expr>
struct is_aggregate_expression<KnownAggregateColumns, group_by_column<Expr>>
    : public std::true_type {};

template <typename Expr> struct raw_group_by_column {
  using type = Expr;
};

template <typename Expr> struct raw_group_by_column<group_by_column<Expr>> {
  using type = Expr;
};

template <typename Expr>
using raw_group_by_column_t = typename raw_group_by_column<Expr>::type;

template <typename Expr>
struct is_group_by_column<group_by_column<Expr>> : public std::true_type {};

template <typename Expr>
struct requires_parentheses<group_by_column<Expr>>
    : public requires_parentheses<Expr> {};

template <typename Expr>
struct value_type_of<group_by_column<Expr>> : public value_type_of<Expr> {};

template <typename Context, typename Expr>
auto to_sql_string(Context &context, const group_by_column<Expr> &t)
    -> std::string {
  return to_sql_string(context, t._expr);
}

template <typename Expr>
using check_declare_group_by_column_args =
    std::enable_if_t<has_value_type<Expr>::value and
                     not is_group_by_column<Expr>::value>;

template <typename Expr, typename = check_declare_group_by_column_args<Expr>>
auto declare_group_by_column(Expr expr) -> group_by_column<Expr> {
  return {std::move(expr)};
}

} // namespace sqlpp

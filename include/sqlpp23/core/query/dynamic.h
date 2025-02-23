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

#include <sqlpp23/core/concepts.h>
#include <sqlpp23/core/operator/assign_expression.h>
#include <sqlpp23/core/operator/enable_as.h>
#include <sqlpp23/core/operator/as_expression_fwd.h>
#include <sqlpp23/core/operator/sort_order_expression.h>
#include <sqlpp23/core/query/dynamic_fwd.h>
#include <sqlpp23/core/query/statement_fwd.h>
#include <sqlpp23/core/static_assert.h>
#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename Expr> struct dynamic_t : public enable_as<dynamic_t<Expr>> {
  dynamic_t(bool condition, Expr expr)
      : _condition(condition), _expr(std::move(expr)) {
    SQLPP_STATIC_ASSERT(
        parameters_of_t<Expr>::empty(),
        "dynamic expressions must not contain query parameters");
  }

  template <typename OtherExpr>
  dynamic_t(const dynamic_t<OtherExpr> &d)
      : _condition(d._condition), _expr(d._expr) {}
  template <typename OtherExpr>
  dynamic_t(dynamic_t<OtherExpr> &&d)
      : _condition(d._condition), _expr(std::move(d._expr)) {}
  template <typename OtherExpr>
  dynamic_t &operator=(const dynamic_t<OtherExpr> &d) {
    _condition = d._condition;
    _expr = Expr{d._expr};
  }
  template <typename OtherExpr> dynamic_t &operator=(dynamic_t<OtherExpr> &&d) {
    _condition = d._condition;
    _expr = Expr{std::move(d._expr)};
  }

  dynamic_t(const dynamic_t &) = default;
  dynamic_t(dynamic_t &&) = default;
  dynamic_t &operator=(const dynamic_t &) = default;
  dynamic_t &operator=(dynamic_t &&) = default;
  ~dynamic_t() = default;

  bool _condition;
  Expr _expr;
};

// No value_type_of or name_tag_of defined for dynamic_t, to prevent its usage
// outside of select columns or similar explicitly allowed areas.

template <typename Expr> struct nodes_of<dynamic_t<Expr>> {
  using type = detail::type_vector<Expr>;
};

// No to_sql_string:
//
// Dynamic expressions need to handle dynamic parts in their own fashion, e.g.
//
// * a.join(dynamic(false, b)).on(a.id == b.id)
///  --> "a"
// * select(dynamic(maybe, b))
//   --> "NULL as b"

template <typename Expr>
using check_dynamic_args = std::enable_if_t<has_value_type<Expr>::value or
                                            is_select_flag<Expr>::value>;

template <typename Expr, typename = check_dynamic_args<Expr>>
auto dynamic(bool condition, Expr t) -> dynamic_t<Expr> {
  return {condition, std::move(t)};
}

template <StaticTable _Table>
auto dynamic(bool condition, _Table t) -> dynamic_t<_Table> {
  return {condition, std::move(t)};
}

template <typename Expr, typename NameTag>
  requires(has_value_type_v<Expr>)
auto dynamic(bool condition, as_expression<Expr, NameTag> t)
    -> dynamic_t<as_expression<Expr, NameTag>> {
  return {condition, std::move(t)};
}

template <typename L, typename Operator, typename R,
          typename = check_dynamic_args<L>>
auto dynamic(bool condition, assign_expression<L, Operator, R> t)
    -> dynamic_t<assign_expression<L, Operator, R>> {
  return {condition, std::move(t)};
}

template <typename Expr, typename = check_dynamic_args<Expr>>
auto dynamic(bool condition, sort_order_expression<Expr> t)
    -> dynamic_t<sort_order_expression<Expr>> {
  return {condition, std::move(t)};
}

template <typename... Clauses>
auto dynamic(bool condition, statement_t<Clauses...> t)
    -> dynamic_t<statement_t<Clauses...>> {
  return {condition, std::move(t)};
}

} // namespace sqlpp

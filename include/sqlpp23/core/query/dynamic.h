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

#include <optional>

#include <sqlpp23/core/concepts.h>
#include <sqlpp23/core/operator/as_expression_fwd.h>
#include <sqlpp23/core/operator/assign_expression.h>
#include <sqlpp23/core/operator/enable_as.h>
#include <sqlpp23/core/operator/sort_order_expression.h>
#include <sqlpp23/core/query/dynamic_fwd.h>
#include <sqlpp23/core/query/statement_fwd.h>
#include <sqlpp23/core/static_assert.h>
#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename Expr>
struct dynamic_t : public enable_as<dynamic_t<Expr>> {
  dynamic_t(std::optional<Expr> expr) : _expr(std::move(expr)) {
    SQLPP_STATIC_ASSERT(
        parameters_of_t<Expr>::empty(),
        "dynamic expressions must not contain query parameters");
  }

  dynamic_t(const dynamic_t&) = default;
  dynamic_t(dynamic_t&&) = default;
  dynamic_t& operator=(const dynamic_t&) = default;
  dynamic_t& operator=(dynamic_t&&) = default;
  ~dynamic_t() = default;

  template <typename OtherExpr>
  dynamic_t(const dynamic_t<OtherExpr>& d) : _expr(d._expr) {}
  template <typename OtherExpr>
  dynamic_t(dynamic_t<OtherExpr>&& d) : _expr(std::move(d._expr)) {}
  template <typename OtherExpr>
  dynamic_t& operator=(const dynamic_t<OtherExpr>& d) {
    _expr = Expr{d._expr};
  }
  template <typename OtherExpr>
  dynamic_t& operator=(dynamic_t<OtherExpr>&& d) {
    _expr = Expr{std::move(d._expr)};
  }

  auto has_value() const -> bool { return _expr.has_value(); }
  auto value() const -> const Expr& { return _expr.value(); }

 private:
  template <typename OtherExpr>
  friend struct dynamic_t;
  std::optional<Expr> _expr;
};

// No value_type_of or name_tag_of defined for dynamic_t, to prevent its usage
// outside of select columns or similar explicitly allowed areas.

template <typename Expr>
struct nodes_of<dynamic_t<Expr>> {
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

// Constructing from optional
template <typename Expr>
  requires(has_value_type<Expr>::value or is_select_flag<Expr>::value or
           is_as_expression<Expr>::value or is_assignment<Expr>::value or
           is_table<Expr>::value or is_sort_order<Expr>::value or
           is_statement<Expr>::value)
auto dynamic(std::optional<Expr> t) -> dynamic_t<Expr> {
  return {std::move(t)};
}

// Constructing from condition and value
template <typename Expr>
  requires(has_value_type<Expr>::value or is_select_flag<Expr>::value or
           is_as_expression<Expr>::value or is_assignment<Expr>::value or
           is_table<Expr>::value or is_sort_order<Expr>::value or
           is_statement<Expr>::value)
auto dynamic(bool condition, Expr t) -> dynamic_t<Expr> {
  if (condition) {
    return {std::move(t)};
  }
  return {std::nullopt};
}

}  // namespace sqlpp

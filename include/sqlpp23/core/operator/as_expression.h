#pragma once

/*
Copyright (c) 2024, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <optional>
#include <type_traits>

#include <sqlpp23/core/operator/as_expression_fwd.h>
#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename Expression, typename NameTag> struct as_expression {
  constexpr as_expression(Expression expression)
      : _expression(std::move(expression)) {}
  as_expression(const as_expression &) = default;
  as_expression(as_expression &&) = default;
  as_expression &operator=(const as_expression &) = default;
  as_expression &operator=(as_expression &&) = default;
  ~as_expression() = default;

  Expression _expression;
};

template <typename Expression, typename NameTag>
struct name_tag_of<as_expression<Expression, NameTag>> {
  using type = NameTag;
};

// No value_type_of defined for as_expression to prevent its usage outside of
// select columns.

template <typename Expression, typename NameTag>
struct nodes_of<as_expression<Expression, NameTag>> {
  using type = detail::type_vector<Expression>;
};

template <typename Expression, typename NameTag>
struct is_as_expression<as_expression<Expression, NameTag>>
    : public std::true_type {};

template <typename Context, typename Expression, typename NameTag>
auto to_sql_string(Context &context,
                   const as_expression<Expression, NameTag> &t) -> std::string {
  return operand_to_sql_string(context, t._expression) + " AS " +
         name_to_sql_string(context, NameTag{});
}

template <typename Expr, typename NameTagProvider>
  requires(has_value_type_v<Expr> and not is_dynamic<Expr>::value and
           not is_as_expression<Expr>::value and
           has_name_tag_v<NameTagProvider>)
constexpr auto as(Expr expr, const NameTagProvider &)
    -> as_expression<Expr, name_tag_of_t<NameTagProvider>> {
  return {std::move(expr)};
}

template <typename NameTagProvider>
constexpr auto as(std::nullopt_t expr, const NameTagProvider &)
    -> as_expression<std::nullopt_t, name_tag_of_t<NameTagProvider>> {
  return {std::move(expr)};
}

} // namespace sqlpp

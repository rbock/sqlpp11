#pragma once

/*
Copyright (c) 2024, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or
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

#include <type_traits>

#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/to_sql_string.h>

namespace sqlpp
{
#warning: need type tests
#warning: Should only use NameTag, not NameTagProvider here!
  template <typename Expression, typename NameTagProvider>
  struct expression_as
  {
    using _traits = make_traits<value_type_of_t<Expression>, tag::is_selectable, tag::is_alias>;

#warning Maybe make constructor of expressions private to force construction in the respective functions?
    constexpr expression_as(Expression expression) : _expression(std::move(expression))
    {
    }
    expression_as(const expression_as&) = default;
    expression_as(expression_as&&) = default;
    expression_as& operator=(const expression_as&) = default;
    expression_as& operator=(expression_as&&) = default;
    ~expression_as() = default;

    Expression _expression;
  };

  // No value_type_of or name_tag_of defined for expression_as, to prevent its usage outside of select columns.

  template <typename T>
  struct remove_as
  {
    using type = T;
  };

  template <typename Expression, typename NameTagProvider>
  struct remove_as<expression_as<Expression, NameTagProvider>>
  {
    using type = Expression;
  };

  template <typename T>
  using remove_as_t = typename remove_as<T>::type;

  template <typename Expression, typename NameTagProvider>
  struct nodes_of<expression_as<Expression, NameTagProvider>>
  {
    using type = detail::type_vector<Expression>;
  };

  template <typename Context, typename Expression, typename NameTagProvider>
  auto to_sql_string(Context& context, const expression_as<Expression, NameTagProvider>& t) -> std::string
  {
    return operand_to_sql_string(context, t._expression) + " AS " + name_to_sql_string(context, name_tag_of_t<NameTagProvider>::name);
  }

  template <typename Expr, typename NameTagProvider>
  using check_as_args = ::sqlpp::enable_if_t<
  has_value_type<Expr>::value and not is_alias_t<Expr>::value and has_name<NameTagProvider>::value
  >;

  template <typename Expr, typename NameTagProvider, typename = check_as_args<Expr, NameTagProvider>>
  constexpr auto as(Expr expr, const NameTagProvider&) -> expression_as<Expr, NameTagProvider>
  {
      return {std::move(expr)};
  }

  template <typename Expr>
  struct dynamic_t;

  template <typename Expr, typename NameTagProvider, typename = check_as_args<Expr, NameTagProvider>>
  constexpr auto as(dynamic_t<Expr> expr, const NameTagProvider&) -> expression_as<dynamic_t<Expr>, NameTagProvider>
  {
      return {std::move(expr)};
  }

  template <typename NameTagProvider>
  constexpr auto as(sqlpp::nullopt_t expr, const NameTagProvider&) -> expression_as<nullopt_t, NameTagProvider>
  {
      return {std::move(expr)};
  }

}  // namespace sqlpp

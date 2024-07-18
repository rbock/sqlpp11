#pragma once

/*
Copyright (c) 2017, Roland Bock
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

#include <sqlpp11/operator/as_expression.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  template <typename Expression, typename AliasProvider>
  struct as_expression
  {
    using _traits = make_traits<value_type_of_t<Expression>, tag::is_selectable, tag::is_alias>;
    using _nodes = detail::type_vector<Expression>;

#warning Maybe make constructor of expressions private to force construction in the respective functions?
    /*
    static_assert(is_expression_t<Expression>::value, "invalid argument for an expression alias");
    static_assert(not is_alias_t<Expression>::value, "cannot create an alias of an alias");
    */

    using _alias_t = typename AliasProvider::_alias_t;

    as_expression(Expression expression) : _expression(expression)
    {
    }

    as_expression(const as_expression&) = default;
    as_expression(as_expression&&) = default;
    as_expression& operator=(const as_expression&) = default;
    as_expression& operator=(as_expression&&) = default;
    ~as_expression() = default;

    Expression _expression;
  };

  template <typename Expression, typename AliasProvider>
  struct value_type_of<as_expression<Expression, AliasProvider>>
  {
    using type = value_type_of_t<Expression>;
  };

  template <typename Expression, typename AliasProvider>
  struct has_name<as_expression<Expression, AliasProvider>> : std::true_type
  {
  };

  template <typename Context, typename Expression, typename AliasProvider>
  Context& serialize(Context& context, const as_expression<Expression, AliasProvider>& t)
  {
    serialize_operand(context, t._expression);
    context << " AS ";
    context << name_of<as_expression<Expression, AliasProvider>>::template char_ptr<Context>();
    return context;
  }
  template <typename Expr, typename AliasProvider>
  using check_as_args = std::enable_if_t<
  has_value_type<Expr>::value and not is_alias_t<Expr>::value and has_name<AliasProvider>::value
  >;

  template <typename Expr, typename AliasProvider, typename = check_as_args<Expr, AliasProvider>>
  constexpr auto as(Expr expr, const AliasProvider&) -> as_expression<Expr, AliasProvider>
  {
      return {std::move(expr)};
  }

}  // namespace sqlpp

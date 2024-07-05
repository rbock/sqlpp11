#pragma once

/*
Copyright (c) 2018, Roland Bock
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

#include <sqlpp11/alias.h>
//#include <sqlpp11/bad_expression.h>
//#include <sqlpp11/embrace.h>
//#include <sqlpp11/to_sql_string.h>
#include <sqlpp11/type_traits.h>
//#include <sqlpp11/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename L, typename Operator, typename R>
  struct logical_expression
  {
    logical_expression() = delete;
    constexpr logical_expression(L l, R r) : _l(l), _r(r)
    {
    }
    logical_expression(const logical_expression&) = default;
    logical_expression(logical_expression&&) = default;
    logical_expression& operator=(const logical_expression&) = default;
    logical_expression& operator=(logical_expression&&) = default;
    ~logical_expression() = default;

    template <typename alias_provider>
    expression_alias_t<logical_expression, alias_provider> as(const alias_provider& /*unused*/) const
    {
      return {*this};
    }

    L _l;
    R _r;
  };

  template <typename Operator, typename R>
  struct unary_logical_expression
  {
    unary_logical_expression() = delete;
    constexpr unary_logical_expression(R r) : _r(r)
    {
    }
    unary_logical_expression(const unary_logical_expression&) = default;
    unary_logical_expression(unary_logical_expression&&) = default;
    unary_logical_expression& operator=(const unary_logical_expression&) = default;
    unary_logical_expression& operator=(unary_logical_expression&&) = default;
    ~unary_logical_expression() = default;

    template <typename alias_provider>
    expression_alias_t<unary_logical_expression, alias_provider> as(const alias_provider& /*unused*/) const
    {
      return {*this};
    }

    R _r;
  };

  template <typename L, typename R>
  using check_logical_args = std::enable_if_t<has_boolean_value<L>::value and has_boolean_value<R>::value>;

  /*
  template <typename L, typename Operator, typename R>
  struct nodes_of<logical_binary_expression<L, Operator, R>>
  {
    using type = type_vector<L, R>;
  };

  template <typename L, typename Operator, typename R>
  struct value_type_of_t<logical_binary_expression<L, Operator, R>>
  {
    using type = bool;
  };

  template <typename L, typename Operator, typename R>
  constexpr auto requires_braces_v<logical_binary_expression<L, Operator, R>> = true;

  template <typename Context, typename L, typename Operator, typename R>
  [[nodiscard]] auto to_sql_string(Context& context, const logical_binary_expression<L, Operator, R>& t)
  {
    return to_sql_string(context, embrace(t._l)) + Operator::symbol + to_sql_string(context, embrace(t._r));
  }

  template <typename Context, typename Operator, typename R>
  [[nodiscard]] auto to_sql_string(Context& context, const logical_binary_expression<none_t, Operator, R>& t)
  {
    return Operator::symbol + to_sql_string(context, embrace(t._r));
  }

  template <typename Context, typename L1, typename Operator, typename R1, typename R2>
  [[nodiscard]] auto to_sql_string(Context& context, const logical_binary_expression<logical_binary_expression<L1, Operator, R1>, Operator, R2>& t)
  {
    return to_sql_string(context, t._l) + Operator::symbol + to_sql_string(context, embrace(t._r));
  }
  */

}  // namespace sqlpp

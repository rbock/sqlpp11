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

#include <sqlpp11/core/operator/enable_as.h>
#include <sqlpp11/core/query/dynamic.h>
#include <sqlpp11/core/noop.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  struct logical_and
  {
    static constexpr auto symbol = " AND ";
  };

  struct logical_or
  {
    static constexpr auto symbol = " OR ";
  };

  template <typename L, typename Operator, typename R>
  struct logical_expression : public enable_as<logical_expression<L, Operator, R>>
  {
    logical_expression() = delete;
    constexpr logical_expression(L l, R r) : _l(std::move(l)), _r(std::move(r))
    {
    }
    logical_expression(const logical_expression&) = default;
    logical_expression(logical_expression&&) = default;
    logical_expression& operator=(const logical_expression&) = default;
    logical_expression& operator=(logical_expression&&) = default;
    ~logical_expression() = default;

    L _l;
    R _r;
  };

  template <typename L, typename R>
  using check_logical_args = ::sqlpp::enable_if_t<is_boolean<L>::value and is_boolean<R>::value>;

  template <typename L, typename Operator, typename R>
  struct value_type_of<logical_expression<L, Operator, R>>
      : std::conditional<sqlpp::is_optional<value_type_of_t<L>>::value or sqlpp::is_optional<value_type_of_t<remove_dynamic_t<R>>>::value,
                         ::sqlpp::optional<boolean>,
                         boolean>
  {
  };

  template <typename L, typename Operator, typename R>
  struct nodes_of<logical_expression<L, Operator, R>>
  {
    using type = detail::type_vector<L, R>;
  };

  template <typename L, typename Operator, typename R>
  struct requires_parentheses<logical_expression<L, Operator, R>> : public std::true_type{};

  template <typename Context, typename L, typename Operator, typename R>
  auto to_sql_string_impl(Context& context, const logical_expression<L, Operator, R>& t) -> std::string
  {
    return operand_to_sql_string(context, t._l) + Operator::symbol + operand_to_sql_string(context, t._r);
  }

  template <typename Context, typename L, typename Operator, typename R>
  auto to_sql_string(Context& context, const logical_expression<L, Operator, R>& t) -> std::string
  {
    return to_sql_string_impl(context, t);
  }

  template <typename Context, typename L, typename Operator, typename R1, typename R2>
  auto to_sql_string(Context& context,
                 const logical_expression<logical_expression<L, Operator, R1>, Operator, R2>& t) -> std::string
  {
    return to_sql_string(context, t._l) + Operator::symbol + operand_to_sql_string(context, t._r);
  }

  template <typename Context, typename L, typename Operator, typename R1, typename R2>
  auto to_sql_string(Context& context,
                 const logical_expression<logical_expression<L, Operator, R1>, Operator, dynamic_t<R2>>& t) -> std::string
  {
    if (t._r._condition)
    {
      return to_sql_string(context, t._l) + Operator::symbol + operand_to_sql_string(context, t._r);
    }

    // If the dynamic part is inactive ignore it.
    return to_sql_string(context, t._l);
  }

  template <typename Context, typename L, typename Operator, typename R>
  auto to_sql_string(Context& context, const logical_expression<L, Operator, dynamic_t<R>>& t) -> std::string
  {
    if (t._r._condition)
    {
      return to_sql_string_impl(context, t);
    }

    // If the dynamic part is inactive ignore it.
    return to_sql_string(context, t._l);
  }

  template <typename L, typename R, typename = check_logical_args<L, remove_dynamic_t<R>>>
  constexpr auto operator and(L l, R r) -> logical_expression<L, logical_and, R>
  {
    return {std::move(l), std::move(r)};
  }

  template <typename L, typename R, typename = check_logical_args<L, remove_dynamic_t<R>>>
  constexpr auto operator||(L l, R r) -> logical_expression<L, logical_or, R>
  {
    return {std::move(l), std::move(r)};
  }

  struct logical_not
  {
    static constexpr auto symbol = "NOT ";
  };

  template <typename R, typename = check_logical_args<R, R>>
  constexpr auto operator!(R r) -> logical_expression<noop, logical_not, R>
  {
    return {{}, std::move(r)};
  }

}  // namespace sqlpp

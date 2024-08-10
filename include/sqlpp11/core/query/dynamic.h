#pragma once

/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/operator/sort_order_expression.h>
#include <sqlpp11/core/operator/enable_as.h>
#include <sqlpp11/core/to_sql_string.h>

namespace sqlpp
{
  template <typename Expr>
  struct dynamic_t: public enable_as<dynamic_t<Expr>>
  {
    using _traits = make_traits<value_type_of_t<Expr>, tag::is_multi_expression>;

    dynamic_t(bool condition, Expr expr) : _condition(condition), _expr(expr)
    {
    }

    dynamic_t(const dynamic_t&) = default;
    dynamic_t(dynamic_t&&) = default;
    dynamic_t& operator=(const dynamic_t&) = default;
    dynamic_t& operator=(dynamic_t&&) = default;
    ~dynamic_t() = default;

    bool _condition;
    Expr _expr;
  };

  // No value_type_of or name_tag_of defined for dynamic_t, to prevent its usage outside of select columns.

  template <typename Expr>
  struct nodes_of<dynamic_t<Expr>> : public nodes_of<Expr>
  {
  };

  template <typename T>
  struct remove_dynamic
  {
    using type = T;
  };

  template <typename Expr>
  struct remove_dynamic<dynamic_t<Expr>>
  {
    using type = Expr;
  };

  template <typename T>
  using remove_dynamic_t = typename remove_dynamic<T>::type;

  template <typename Context, typename Select>
  auto to_sql_string(Context& context, const dynamic_t<Select>& t) -> std::string
  {
    if (t._condition)
    {
      to_sql_string(context, t._expr);
    }
    else
    {
      to_sql_string(context, ::sqlpp::nullopt);
    }
    return context;
  }

  template <typename Context, typename Select>
  Context& operand_to_sql_string(Context& context, const dynamic_t<Select>& t)
  {
    if (t._condition)
    {
      operand_to_sql_string(context, t._expr);
    }
    else
    {
      to_sql_string(context, ::sqlpp::nullopt);
    }
    return context;
  }

  template <typename Expr>
  using check_dynamic_args = ::sqlpp::enable_if_t<has_value_type<Expr>::value>;

  template <typename Expr, typename = check_dynamic_args<Expr>>
  auto dynamic(bool condition, Expr t) -> dynamic_t<Expr>
  {
    return {condition, std::move(t)};
  }

  template <typename L, typename Operator, typename R, typename = check_dynamic_args<L>>
  auto dynamic(bool condition, assign_expression<L, Operator, R> t) -> dynamic_t<assign_expression<L, Operator, R>>
  {
    return {condition, std::move(t)};
  }

  template <typename Expr, typename = check_dynamic_args<Expr>>
  auto dynamic(bool condition, sort_order_expression<Expr> t) -> dynamic_t<sort_order_expression<Expr>>
  {
    return {condition, std::move(t)};
  }
}  // namespace sqlpp

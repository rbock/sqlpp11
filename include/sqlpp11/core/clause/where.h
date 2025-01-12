#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/query/dynamic.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/query/statement_fwd.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/unconditional.h>
#include <sqlpp11/core/basic/value.h>

namespace sqlpp
{
  template <typename Expression>
  struct where_t
  {
    Expression _expression;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_no_unknown_tables_in_where_t,
      "at least one expression in where() requires a table which is otherwise not known in the statement");

  template <typename Expression>
  struct is_clause<where_t<Expression>> : public std::true_type
  {
  };

  template <typename Statement, typename Expression>
  struct consistency_check<Statement, where_t<Expression>>
  {
    using type = consistent_t;
  };

  template <typename Statement, typename Expression>
  struct prepare_check<Statement, where_t<Expression>>
  {
      using type = typename std::conditional<Statement::template _no_unknown_tables<where_t<Expression>>,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_where_t>::type;
  };

  template <typename Expression>
  struct nodes_of<where_t<Expression>>
  {
    using type = detail::type_vector<Expression>;
  };

  template <>
  struct where_t<unconditional_t>
  {
  };

  // NO WHERE YET
  struct no_where_t
  {
  };

  template <typename Statement>
  struct clause_base<no_where_t, Statement> : public clause_data<no_where_t, Statement>
  {
    using clause_data<no_where_t, Statement>::clause_data;

    auto unconditionally() const -> decltype(new_statement(*this, where_t<unconditional_t>{}))
    {
      return new_statement(*this, where_t<unconditional_t>{});
    }

    template <typename Expression, typename = sqlpp::enable_if_t<is_boolean<remove_dynamic_t<Expression>>::value>>
    auto where(Expression expression) const -> decltype(new_statement(*this, where_t<Expression>{expression}))
    {
      SQLPP_STATIC_ASSERT(not contains_aggregate_function<Expression>::value,
                          "where() must not contain aggregate functions");

      return new_statement(*this, where_t<Expression>{expression});
    }
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_where_or_unconditionally_called_t,
                               "calling where() or unconditionally() required");

  template <typename Statement>
  struct consistency_check<Statement, no_where_t>
  {
    using type = typename std::
        conditional<is_where_required<Statement>::value, assert_where_or_unconditionally_called_t, consistent_t>::type;
  };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const no_where_t&) -> std::string
  {
    return "";
  }

  template <typename Context, typename Expression>
  auto to_sql_string(Context& context, const where_t<Expression>& t) -> std::string
  {
    return  " WHERE " + to_sql_string(context, t._expression);
  }

  template <typename Context, typename Expression>
  auto to_sql_string(Context& context, const where_t<dynamic_t<Expression>>& t) -> std::string
  {
    if (t._expression._condition)
    {
      return " WHERE " + to_sql_string(context, t._expression._expr);
    }
    return "";
  }

  template <typename Context>
  auto to_sql_string(Context& , const where_t<unconditional_t>&) -> std::string
  {
    return {};
  }

  template <typename T>
  auto where(T&& t) -> decltype(statement_t<no_where_t>().where(std::forward<T>(t)))
  {
    return statement_t<no_where_t>().where(std::forward<T>(t));
  }

  inline auto unconditionally() -> decltype(statement_t<no_where_t>().unconditionally())
  {
    return statement_t<no_where_t>().unconditionally();
  }
}  // namespace sqlpp

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
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/basic/value.h>

namespace sqlpp
{
  template <typename Expression>
  struct having_t
  {
    having_t(Expression expression) : _expression(expression)
    {
    }

    having_t(const having_t&) = default;
    having_t(having_t&&) = default;
    having_t& operator=(const having_t&) = default;
    having_t& operator=(having_t&&) = default;
    ~having_t() = default;

    Expression _expression;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_having_no_unknown_tables_t,
      "at least one having-expression requires a table which is otherwise not known in the statement");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_having_all_aggregates_t,
                               "having expression not built out of aggregate expressions");

  template <typename Expression>
  struct is_clause<having_t<Expression>> : public std::true_type
  {
  };

  template <typename Statement, typename Expression>
  struct consistency_check<Statement, having_t<Expression>>
  {
    using type = static_combined_check_t<
          static_check_t<Statement::template _no_unknown_tables<having_t<Expression>>,
                         assert_having_no_unknown_tables_t>,
          static_check_t<is_aggregate_expression<typename Statement::_all_provided_aggregates, Expression>::value,
                         assert_having_all_aggregates_t>>;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_having_boolean_expression_t,
                               "having() argument has to be an sqlpp boolean expression.");

  template <typename Expression>
  struct check_having
  {
    using type =
        static_combined_check_t<static_check_t<is_boolean<Expression>::value, assert_having_boolean_expression_t>>;
  };

  template <typename Expression>
  using check_having_t = typename check_having<remove_dynamic_t<Expression>>::type;

  template <typename... Exprs>
  constexpr auto are_all_parameters_expressions() -> bool
  {
    return logic::all<is_expression_t<Exprs>::value...>::value;
  }

  // NO HAVING YET
  struct no_having_t
  {
  };

  template <typename Statement>
  struct clause_base<no_having_t, Statement> : public clause_data<no_having_t, Statement>
  {
    using clause_data<no_having_t, Statement>::clause_data;

#warning : reactivate check_having_t
    template <typename Expression>
    auto having(Expression expression) const -> decltype(new_statement(*this, having_t<Expression>{expression}))
    {
      return new_statement(*this, having_t<Expression>{expression});
    }
  };

  template <typename Statement>
  struct consistency_check<Statement, no_having_t>
  {
    using type = consistent_t;
  };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const no_having_t&) -> std::string
  {
    return "";
  }

  template <typename Context, typename Expression>
  auto to_sql_string(Context& context, const having_t<Expression>& t) -> std::string
  {
    return " HAVING " + to_sql_string(context, t._expression);
  }

  template <typename Context, typename Expression>
  auto to_sql_string(Context& context, const having_t<dynamic_t<Expression>>& t) -> std::string
  {
    if (t._expression._condition)
    {
      return " HAVING " + to_sql_string(context, t._expression._expr);
    }
    return "";
  }

  template <typename T>
  auto having(T&& t) -> decltype(statement_t<no_having_t>().having(std::forward<T>(t)))
  {
    return statement_t<no_having_t>().having(std::forward<T>(t));
  }

}  // namespace sqlpp

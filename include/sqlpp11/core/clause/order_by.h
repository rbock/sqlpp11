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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/type_traits.h>
#include <tuple>

namespace sqlpp
{
  template <typename... Expressions>
  struct order_by_t
  {
    order_by_t(Expressions... expressions) : _expressions(expressions...)
    {
    }

    order_by_t(const order_by_t&) = default;
    order_by_t(order_by_t&&) = default;
    order_by_t& operator=(const order_by_t&) = default;
    order_by_t& operator=(order_by_t&&) = default;
    ~order_by_t() = default;

    std::tuple<Expressions...> _expressions;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_order_by_t,
      "at least one order-by expression requires a table which is otherwise not known in the statement");

  template <typename... Expressions>
  struct is_clause<order_by_t<Expressions...>> : public std::true_type
  {
  };

  template <typename Statement, typename... Expressions>
  struct consistency_check<Statement, order_by_t<Expressions...>>
  {
    using type = consistent_t;
  };

  template <typename Statement, typename... Expressions>
  struct prepare_check<Statement, order_by_t<Expressions...>>
  {
    using type = typename std::conditional<Statement::template _no_unknown_tables<order_by_t<Expressions...>>,
                                           consistent_t,
                                           assert_no_unknown_tables_in_order_by_t>::type;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_order_by_args_are_sort_order_expressions_t,
                               "arguments for order_by() must be sort order expressions");
  template <typename... Exprs>
  struct check_order_by
  {
    using type = static_combined_check_t<static_check_t<logic::all<is_sort_order<Exprs>::value...>::value,
                                                        assert_order_by_args_are_sort_order_expressions_t>>;
  };
  template <typename... Exprs>
  using check_order_by_t = typename check_order_by<remove_dynamic_t<Exprs>...>::type;

  // NO ORDER BY YET
  struct no_order_by_t
  {
  };

  template <typename Statement>
  struct clause_base<no_order_by_t, Statement> : public clause_data<no_order_by_t, Statement>
  {
    using clause_data<no_order_by_t, Statement>::clause_data;

#warning : reactivate check_order_by
    template <typename... Expressions>
    auto order_by(Expressions... expressions) const
        -> decltype(new_statement(*this, order_by_t<Expressions...>{std::move(expressions)...}))
    {
      static_assert(sizeof...(Expressions), "at least one expression (e.g. a column) required in order_by()");

      static_assert(not detail::has_duplicates<Expressions...>::value,
                    "at least one duplicate argument detected in order_by()");

      return new_statement(*this, order_by_t<Expressions...>{std::move(expressions)...});
    }
  };

  template <typename Statement>
  struct consistency_check<Statement, no_order_by_t>
  {
    using type = consistent_t;
  };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const no_order_by_t&) -> std::string
  {
    return "";
  }

  template <typename Context, typename... Expressions>
  auto to_sql_string(Context& context, const order_by_t<Expressions...>& t) -> std::string
  {
    const auto columns = tuple_to_sql_string(context, t._expressions, tuple_operand_no_dynamic{", "});

    if (columns.empty()) {
      return "";
    }

    return " ORDER BY " + columns;
  }

  template <typename... T>
  auto order_by(T... t) -> decltype(statement_t<no_order_by_t>().order_by(std::forward<T>(t)...))
  {
    return statement_t<no_order_by_t>().order_by(std::move(t)...);
  }

}  // namespace sqlpp

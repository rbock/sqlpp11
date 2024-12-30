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
#include <sqlpp11/core/group_by_column.h>
#include <tuple>

namespace sqlpp
{
  template <typename... Columns>
  struct group_by_t
  {
    group_by_t(Columns... columns) : _columns(columns...)
    {
    }

    group_by_t(const group_by_t&) = default;
    group_by_t(group_by_t&&) = default;
    group_by_t& operator=(const group_by_t&) = default;
    group_by_t& operator=(group_by_t&&) = default;
    ~group_by_t() = default;

    std::tuple<Columns...> _columns;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_no_unknown_tables_in_group_by_t,
      "at least one group-by expression requires a table which is otherwise not known in the statement");

  template <typename... Columns>
  struct is_clause<group_by_t<Columns...>> : public std::true_type
  {
  };

 template <typename Statement, typename... Columns>
  struct consistency_check<Statement, group_by_t<Columns...>>
  {
    using type = consistent_t;
  };

 template <typename Statement, typename... Columns>
  struct prepare_check<Statement, group_by_t<Columns...>>
  {
    using type = typename std::conditional<Statement::template _no_unknown_tables<group_by_t<Columns...>>,
                                           consistent_t,
                                           assert_no_unknown_tables_in_group_by_t>::type;
  };

  template <typename... Columns>
  struct known_aggregate_columns_of<group_by_t<Columns...>>
  {
    using type = detail::type_set<raw_group_by_column_t<remove_dynamic_t<Columns>>...>;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_group_by_args_are_columns_t, "all arguments for group_by() must be columns");

  template <typename... Columns>
  struct check_group_by
  {
    using type = static_combined_check_t<
        static_check_t<logic::all<is_group_by_column<remove_dynamic_t<Columns>>::value...>::value, assert_group_by_args_are_columns_t>>;
  };
  template <typename... Columns>
  using check_group_by_t = typename check_group_by<remove_dynamic_t<Columns>...>::type;

  // NO GROUP BY YET
  struct no_group_by_t
  {
  };

  template <typename Statement>
  struct clause_base<no_group_by_t, Statement> : public clause_data<no_group_by_t, Statement>
  {
    using clause_data<no_group_by_t, Statement>::clause_data;

#warning: reactive check_group_by_t<Columns...>

    template <typename... Columns>
    auto group_by(Columns... columns) const -> decltype(new_statement(*this, group_by_t<Columns...>{columns...}))
    {
      static_assert(sizeof...(Columns), "at least one column required in group_by()");

      return new_statement(*this, group_by_t<Columns...>{columns...});
    }
  };

  template <typename Statement>
  struct consistency_check<Statement, no_group_by_t>
  {
    using type = consistent_t;
  };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const no_group_by_t&) -> std::string
  {
    return "";
  }

  template <typename Context, typename... Columns>
  auto to_sql_string(Context& context, const group_by_t<Columns...>& t) -> std::string
  {
    const auto columns = tuple_to_sql_string(context, t._columns, tuple_operand_no_dynamic{", "});

    if (columns.empty()) {
      return "";
    }

    return " GROUP BY " + columns;
  }

  template <typename... T>
  auto group_by(T... t) -> decltype(statement_t<no_group_by_t>().group_by(std::move(t)...))
  {
    return statement_t<no_group_by_t>().group_by(std::move(t)...);
  }

}  // namespace sqlpp

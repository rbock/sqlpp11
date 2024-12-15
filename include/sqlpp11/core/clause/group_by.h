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
  // GROUP BY DATA
  template <typename... Columns>
  struct group_by_data_t
  {
    group_by_data_t(Columns... columns) : _columns(columns...)
    {
    }

    group_by_data_t(const group_by_data_t&) = default;
    group_by_data_t(group_by_data_t&&) = default;
    group_by_data_t& operator=(const group_by_data_t&) = default;
    group_by_data_t& operator=(group_by_data_t&&) = default;
    ~group_by_data_t() = default;

    std::tuple<Columns...> _columns;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_group_by_t,
      "at least one group-by expression requires a table which is otherwise not known in the statement");

  // GROUP BY
  template <typename... Columns>
  struct group_by_t
  {
    using _traits = make_traits<no_value_t, tag::is_group_by>;
    using _nodes = detail::type_vector<Columns...>;

   using _data_t = group_by_data_t<Columns...>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<group_by_t>,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_group_by_t>::type;
    };
  };

  template <typename... Columns>
  struct known_aggregate_columns_of<group_by_t<Columns...>>
  {
    using type = detail::type_set<raw_group_by_column_t<Columns>...>;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_group_by_args_are_columns_t, "all arguments for group_by() must be columns");

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
    // Data
    using _data_t = no_data_t;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t() = default;
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_group_by_t, T>;

      using _consistency_check = consistent_t;

      template <typename... Columns>
      auto group_by(Columns... columns) const
          -> _new_statement_t<check_group_by_t<Columns...>, group_by_t<Columns...>>
      {
        static_assert(sizeof...(Columns), "at least one column required in group_by()");

        return _group_by_impl(check_group_by_t<Columns...>{}, columns...);
      }

    private:
      template <typename Check, typename... Columns>
      auto _group_by_impl(Check, Columns... columns) const -> inconsistent<Check>;

      template <typename... Columns>
      auto _group_by_impl(consistent_t /*unused*/, Columns... columns) const
          -> _new_statement_t<consistent_t, group_by_t<Columns...>>
      {
        static_assert(not detail::has_duplicates<Columns...>::value,
                      "at least one duplicate argument detected in group_by()");

        return {static_cast<const derived_statement_t<Policies>&>(*this),
                group_by_data_t<Columns...>{columns...}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename... Columns>
  auto to_sql_string(Context& context, const group_by_data_t<Columns...>& t) -> std::string
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

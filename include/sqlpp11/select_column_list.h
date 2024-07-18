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

#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/dynamic.h>
#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/field_spec.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/result_row.h>
#include <sqlpp11/select_pseudo_table.h>
#include <sqlpp11/table.h>
#include <tuple>

namespace sqlpp
{
  namespace detail
  {
    template <typename... Columns>
    struct select_traits
    {
      using _traits = make_traits<no_value_t, tag::is_select_column_list, tag::is_return_value>;
      struct _alias_t
      {
      };
    };

    template <typename Column>
    struct select_traits<Column>
    {
      using _traits = make_traits<value_type_of_t<Column>,
                                  tag::is_select_column_list,
                                  tag::is_return_value,
                                  tag::is_expression,
                                  tag::is_selectable>;
      using _alias_t = typename remove_optional_t<Column>::_alias_t;
    };
  }  // namespace detail

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_selected_columns_t,
      "at least one selected column requires a table which is otherwise not known in the statement");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_no_aggregate_mix_t,
                               "selected columns contain a mix of aggregates and non-aggregates");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_no_unknown_aggregates_t,
                               "not all selected columns are made of aggregates, despite group_by or similar");

  // SELECTED COLUMNS
  template <typename... Columns>
  struct select_column_list_t
  {
    using _traits = typename detail::select_traits<Columns...>::_traits;
    using _nodes = detail::type_vector<Columns...>;

    using _alias_t = typename detail::select_traits<Columns...>::_alias_t;

    using _data_t = std::tuple<Columns...>;

    struct _column_type
    {
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      const _base_t& get_selected_columns() const
      {
        return *this;
      }

      using _table_check = typename std::conditional<Policies::template _no_unknown_tables<select_column_list_t>::value,
                                                     consistent_t,
                                                     assert_no_unknown_tables_in_selected_columns_t>::type;

      using _unknown_aggregate_check =
          typename std::conditional<Policies::template _no_unknown_aggregates<Columns...>::value,
                                    consistent_t,
                                    assert_no_unknown_aggregates_t>::type;

      using _no_aggregate_mix_check =
          typename std::conditional<Policies::template _all_aggregates<Columns...>::value ||
                                        Policies::template _no_aggregates<Columns...>::value,
                                    consistent_t,
                                    assert_no_aggregate_mix_t>::type;

      using _consistency_check = detail::
          get_first_if<is_inconsistent_t, consistent_t, _table_check, _no_aggregate_mix_check, _unknown_aggregate_check>;
    };

    // Result methods
    template <typename Statement>
    struct _result_methods_t
    {
      using _statement_t = Statement;

      const _statement_t& _get_statement() const
      {
        return static_cast<const _statement_t&>(*this);
      }

      template <typename Db, typename Column>
      struct _deferred_field_t
      {
        using type = make_field_spec_t<_statement_t, Column>;
      };

      template <typename Db, typename Column>
      using _field_t = typename _deferred_field_t<Db, Column>::type;

      template <typename Db>
      using _result_row_t = result_row_t<Db, _field_t<Db, dynamic_to_optional_t<Columns>>...>;

      template <typename AliasProvider>
      struct _deferred_table_t
      {
        using table = select_pseudo_table_t<_statement_t, Columns...>;
        using alias = typename table::template _alias_t<AliasProvider>;
      };

      template <typename AliasProvider>
      using _table_t = typename _deferred_table_t<AliasProvider>::table;

      template <typename AliasProvider>
      using _alias_t = typename _deferred_table_t<AliasProvider>::alias;

      template <typename AliasProvider>
      _alias_t<AliasProvider> as(const AliasProvider& aliasProvider) const
      {
        consistency_check_t<_statement_t>::verify();
        static_assert(_statement_t::_can_be_used_as_table(),
                      "statement cannot be used as table, e.g. due to missing tables");
        return _table_t<AliasProvider>(_get_statement()).as(aliasProvider);
      }

      size_t get_no_of_result_columns() const
      {
        return sizeof...(Columns);
      }

      // Execute
      template <typename Db, typename Composite>
      auto _run(Db& db, const Composite& composite) const -> result_t<decltype(db.select(composite)), _result_row_t<Db>>
      {
        return {db.select(composite)};
      }

      template <typename Db>
      auto _run(Db& db) const -> result_t<decltype(db.select(std::declval<_statement_t>())), _result_row_t<Db>>
      {
        return {db.select(_get_statement())};
      }

      // Prepare
      template <typename Db, typename Composite>
      auto _prepare(Db& db, const Composite& composite) const -> prepared_select_t<Db, _statement_t, Composite>
      {
        return {make_parameter_list_t<Composite>{}, db.prepare_select(composite)};
      }

      template <typename Db>
      auto _prepare(Db& db) const -> prepared_select_t<Db, _statement_t>
      {
        return {make_parameter_list_t<_statement_t>{}, db.prepare_select(_get_statement())};
      }
    };
  };

  template <typename Column>
    struct value_type_of<select_column_list_t<Column>> : public value_type_of<Column> {};

  SQLPP_PORTABLE_STATIC_ASSERT(assert_selected_colums_are_selectable_t, "selected columns must be selectable");
  template <typename... T>
  struct check_selected_columns
  {
    using type =
        static_combined_check_t<static_check_t<logic::all_t<is_selectable_t<T>::value...>::value,
                                               assert_selected_colums_are_selectable_t>>;
  };
  template <typename... T>
  using check_selected_columns_t = typename check_selected_columns<T...>::type;

  struct no_select_column_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop, tag::is_missing>;
    using _nodes = detail::type_vector<>;

    struct _alias_t
    {
    };

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
      using _new_statement_t = new_statement_t<Check, Policies, no_select_column_list_t, T>;

      using _consistency_check = consistent_t;

      template <typename... Args>
      auto columns(Args... args) const
          -> _new_statement_t<check_selected_columns_t<remove_dynamic_t<Args>...>,
                              select_column_list_t<Args...>>
      {
        static_assert(sizeof...(Args), "at least one selectable expression (e.g. a column) required in columns()");
        using check = check_selected_columns_t<remove_dynamic_t<Args>...>;
        static_assert(check::value,
                      "at least one argument is not a selectable expression in columns()");

        return _columns_impl(check{}, std::make_tuple(std::move(args)...));
      }

    private:
      template <typename Check, typename... Args>
      auto _columns_impl(Check, std::tuple<Args...> args) const -> inconsistent<Check>;

      template <typename... Args>
      auto _columns_impl(consistent_t /*unused*/, std::tuple<Args...> args) const
          -> _new_statement_t<consistent_t, select_column_list_t<Args...>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                typename select_column_list_t<Args...>::_data_t{std::move(args)}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename... Columns>
  Context& serialize(const std::tuple<Columns...>& t, Context& context)
  {
    interpret_tuple(t, ',', context);
    return context;
  }

  template <typename... T>
  auto select_columns(T&&... t) -> decltype(statement_t<no_select_column_list_t>().columns(std::forward<T>(t)...))
  {
    return statement_t<no_select_column_list_t>().columns(std::forward<T>(t)...);
  }

}  // namespace sqlpp

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
#include <sqlpp11/core/operator/as_expression.h>
#include <sqlpp11/core/query/dynamic.h>
#include <sqlpp11/core/field_spec.h>
#include <sqlpp11/core/interpret_tuple.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/query/result_row.h>
#include <sqlpp11/core/clause/select_as.h>
#include <sqlpp11/core/clause/select_column_traits.h>
#include <sqlpp11/core/basic/table.h>
#include <tuple>

namespace sqlpp
{
  namespace detail
  {
    template<typename T>
    auto tupelize(T t) -> std::tuple<T>
    {
      return std::make_tuple(std::move(t));
    }

    template<typename... Args>
    auto tupelize(std::tuple<Args...> t) -> std::tuple<Args...>
    {
      return t;
    }

    template<typename... Args>
    struct flat_tuple
    {
      using type = decltype(std::tuple_cat(tupelize(std::declval<Args>())...));
    };

    template<typename... Args>
    using flat_tuple_t = typename flat_tuple<Args...>::type;

    template <typename... Columns>
    struct select_traits
    {
      using _traits = make_traits<no_value_t, tag::is_select_column_list, tag::is_return_value>;
    };

    template <typename Column>
    struct select_traits<Column>
    {
      using _traits = make_traits<value_type_of_t<Column>,
                                  tag::is_select_column_list,
                                  tag::is_return_value,
                                  tag::is_expression,
                                  tag::is_selectable>;
    };
  }  // namespace detail

  // FIXME: We might use field specs here (same as with cte)
  //
  // provide type information for sub-selects that are used as named expressions or tables
  template <typename Select, typename Column>
  struct select_column_spec_t: public name_tag_base
  {
    using _alias_t = select_column_name_tag_of_t<Column>;

#warning: Need to test this!
    static constexpr bool _depends_on_outer_table =
        detail::make_intersect_set_t<required_tables_of_t<Column>, typename Select::_used_outer_tables>::size::value >
        0;
  };
  template <typename Select, typename Column>
    struct value_type_of<select_column_spec_t<Select, Column>> : public select_column_value_type_of<Column> {};

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
      using _result_row_t = result_row_t<Db, _field_t<Db, Columns>...>;

      template <typename AliasProvider>
      auto as(const AliasProvider&) const
          -> select_as_t<_statement_t, AliasProvider, select_column_spec_t<_statement_t, Columns>...>
      {
        consistency_check_t<_statement_t>::verify();
        using table = select_as_t<_statement_t, AliasProvider, select_column_spec_t<_statement_t, Columns>...>;
        return table(_get_statement());
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
        return {{}, db.prepare_select(composite)};
      }

      template <typename Db>
      auto _prepare(Db& db) const -> prepared_select_t<Db, _statement_t>
      {
        return {{}, db.prepare_select(_get_statement())};
      }
    };
  };
  template <typename Column>
  struct value_type_of<select_column_list_t<Column>> : public select_column_value_type_of<Column>
  {
  };

  template <typename Column>
  struct name_tag_of<select_column_list_t<Column>> : public name_tag_of<Column>
  {
  };

  template <typename... Columns>
  struct nodes_of<select_column_list_t<Columns...>>
  {
    using type = detail::type_vector<Columns...>;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_selected_colums_are_selectable_t, "selected columns must be selectable");

  template <typename T>
  struct check_selected_tuple;
  template <typename... T>
  struct check_selected_tuple<std::tuple<T...>>
  {
    using type = static_combined_check_t<
        static_check_t<logic::all_t<(select_column_has_value_type<T>::value and select_column_has_name<T>::value)...>::value,
                       assert_selected_colums_are_selectable_t>>;
  };
  template <typename T>
  using check_selected_tuple_t = typename check_selected_tuple<T>::type;

  template <typename T>
  struct make_select_column_list;
  template <typename... T>
  struct make_select_column_list<std::tuple<T...>>
  {
    using type = select_column_list_t<T...>;
  };
  template <typename T>
  using make_select_column_list_t = typename make_select_column_list<T>::type;

  struct no_select_column_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop, tag::is_missing>;

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
      auto columns(Args... args) const -> _new_statement_t<check_selected_tuple_t<detail::flat_tuple_t<Args...>>,
                                                           make_select_column_list_t<detail::flat_tuple_t<Args...>>>
      {
        static_assert(sizeof...(Args), "at least one selectable expression (e.g. a column) required in columns()");
        using check = check_selected_tuple_t<detail::flat_tuple_t<Args...>>;
        static_assert(check::value,
                      "at least one argument is not a selectable expression in columns()");

        return _columns_impl(check{}, std::tuple_cat(detail::tupelize(std::move(args))...));
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
  Context& serialize(Context& context, const std::tuple<Columns...>& t)
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

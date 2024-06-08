#pragma once

/**
 * Copyright © 2014-2015, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/data_types/no_value.h>
#include <sqlpp11/detail/column_tuple_merge.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/field_spec.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/operand_check.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/result_row.h>
#include <sqlpp11/select_pseudo_table.h>
#include <sqlpp11/table.h>
#include <tuple>

namespace sqlpp
{
  SQLPP_VALUE_TRAIT_GENERATOR(is_returning_column_list)

  namespace postgresql
  {
    namespace detail
    {
      template <typename... Columns>
      struct returning_traits
      {
        using _traits = make_traits<no_value_t, tag::is_returning_column_list, tag::is_return_value>;
        struct _alias_t
        {
        };
      };

      template <typename Column>
      struct returning_traits<Column>
      {
        using _traits = make_traits<value_type_of<Column>,
                                    tag::is_returning_column_list,
                                    tag::is_return_value,
                                    tag::is_expression,
                                    tag::is_selectable>;  // TODO: Is this correct?
        using _alias_t = typename Column::_alias_t;
      };
    }  // namespace detail

    template <typename... Columns>
    struct returning_column_list_data_t
    {
      returning_column_list_data_t(Columns... columns) : _columns(columns...)
      {
      }
      returning_column_list_data_t(std::tuple<Columns...> columns) : _columns(columns)
      {
      }
      returning_column_list_data_t(const returning_column_list_data_t&) = default;
      returning_column_list_data_t(returning_column_list_data_t&&) = default;
      returning_column_list_data_t& operator=(const returning_column_list_data_t&) = default;
      returning_column_list_data_t& operator=(returning_column_list_data_t&&) = default;

      std::tuple<Columns...> _columns;
    };

    // static asserts...
    SQLPP_PORTABLE_STATIC_ASSERT(
        assert_no_unknown_tables_in_returning_columns_t,
        "at least one returning column requires a table which is otherwise not known in the statement");

    // Columns in returning list
    template <typename... Columns>
    struct returning_column_list_t
    {
      using _traits = typename detail::returning_traits<Columns...>::_traits;
      using _nodes = ::sqlpp::detail::type_vector<Columns...>;
      using _alias_t = typename detail::returning_traits<Columns...>::_alias_t;

      struct _column_type
      {
      };

      // Data
      using _data_t = returning_column_list_data_t<Columns...>;

      // Base template to be inherited by the statement
      template <typename Policies>
      struct _base_t
      {
        _base_t(_data_t data) : _data{std::move(data)}
        {
        }

        _data_t _data;

        // Checks
        using _table_check =
            typename std::conditional<Policies::template _no_unknown_tables<returning_column_list_t>::value,
                                      consistent_t,
                                      assert_no_unknown_tables_in_returning_columns_t>::type;

        using _consistency_check = ::sqlpp::detail::get_first_if<is_inconsistent_t, consistent_t, _table_check>;
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
          consistency_check_t<_statement_t>::_();
          static_assert(_statement_t::_can_be_used_as_table(),
                        "statement cannot be used as table, e.g. due to missing tables");
          return _table_t<AliasProvider>(_get_statement()).as(aliasProvider);
        }

        size_t get_no_of_result_columns() const
        {
          return sizeof...(Columns);
        }

        // auto ..
        template <typename Db, typename Composite>
        auto _run(Db& db, const Composite& composite) const
            -> result_t<decltype(db.select(composite)), _result_row_t<Db>>
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

    namespace detail
    {
      template <typename... Columns>
      returning_column_list_t<Columns...> make_returning_column_list(std::tuple<Columns...> columns);
    }

    struct no_returning_column_list_t
    {
      using _traits = make_traits<no_value_t, tag::is_noop, tag::is_missing>;
      using _nodes = ::sqlpp::detail::type_vector<>;

      struct _alias_t
      {
      };

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

        template <typename... T>
        struct _check : logic::all_t<is_selectable_t<T>::value...>
        {
        };

        template <typename... T>
        static constexpr auto _check_tuple(std::tuple<T...>) -> _check<T...>
        {
          return {};
        }

        template <typename... T>
        static constexpr auto _check_args(T... args)
            -> decltype(_check_tuple(sqlpp::detail::column_tuple_merge(args...)))
        {
          return _check_tuple(sqlpp::detail::column_tuple_merge(args...));
        }

        template <typename Check, typename T>
        using _new_statement_t = new_statement_t<Check, Policies, no_returning_column_list_t, T>;

        using _consistency_check = consistent_t;

        template <typename... Args>
        auto returning(Args... args) const -> _new_statement_t<
            decltype(_check_args(args...)),
            decltype(detail::make_returning_column_list(::sqlpp::detail::column_tuple_merge(args...)))>
        {
          static_assert(sizeof...(Args), "at least one selectable expression (e.g. a column) required in returning()");
          static_assert(decltype(_check_args(args...))::value,
                        "at least one argument is not a selectable expression in returning()");

          return _returning_impl(decltype(_check_args(args...)){}, ::sqlpp::detail::column_tuple_merge(args...));
        }

      private:
        template <typename Check, typename... Args>
        auto _returning_impl(const std::false_type&, std::tuple<Args...> args) const -> inconsistent<Check>;

        template <typename... Args>
        auto _returning_impl(consistent_t, std::tuple<Args...> args) const
            -> _new_statement_t<consistent_t, returning_column_list_t<Args...>>
        {
          static_assert(not::sqlpp::detail::has_duplicates<Args...>::value, "at least one duplicate argument detected");
          static_assert(not::sqlpp::detail::has_duplicates<typename Args::_alias_t...>::value,
                        "at least one duplicate name detected");

          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  typename returning_column_list_t<Args...>::_data_t{args}};
        }
      };
    };

    // Serialization
    template <typename... Columns>
    postgresql::context_t& serialize(const postgresql::returning_column_list_data_t<Columns...>& t,
                                     postgresql::context_t& context)
    {
      context << " RETURNING ";
      interpret_tuple(t._columns, ',', context);
      return context;
    }
  }
}  // namespace sqlpp

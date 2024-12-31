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

#include <sqlpp11/core/noop.h>
#warning : It would be nicer to take the required templates out of there
#include <sqlpp11/core/clause/select_column_list.h>
#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/field_spec.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/query/result_row.h>
#include <sqlpp11/core/basic/table.h>
#include <tuple>

namespace sqlpp
{
  namespace postgresql
  {
    SQLPP_WRAPPED_STATIC_ASSERT(
        assert_no_unknown_tables_in_returning_columns_t,
        "at least one returning column requires a table which is otherwise not known in the statement");

  SQLPP_WRAPPED_STATIC_ASSERT(assert_correct_returning_column_aggregates_t,
                               "select must not contain a mix of aggregates and non-aggregates");

    template <typename... Columns>
    struct returning_column_list_t
    {
      returning_column_list_t(Columns... columns) : _columns(columns...)
      {
      }
      returning_column_list_t(std::tuple<Columns...> columns) : _columns(columns)
      {
      }
      returning_column_list_t(const returning_column_list_t&) = default;
      returning_column_list_t(returning_column_list_t&&) = default;
      returning_column_list_t& operator=(const returning_column_list_t&) = default;
      returning_column_list_t& operator=(returning_column_list_t&&) = default;

      std::tuple<Columns...> _columns;

      // Result methods
      template <typename Statement>
      struct _result_methods_t
      {
        using _statement_t = Statement;

        const _statement_t& _get_statement() const
        {
          return static_cast<const _statement_t&>(*this);
        }

        using _result_row_t = result_row_t<make_field_spec_t<_statement_t, Columns>...>;

        template <typename NameTagProvider>
        auto as(const NameTagProvider&) const
            -> select_as_t<_statement_t, name_tag_of_t<NameTagProvider>, make_field_spec_t<_statement_t, Columns>...>
        {
          statement_consistency_check_t<_statement_t>::verify();
          using table =
              select_as_t<_statement_t, name_tag_of_t<NameTagProvider>, make_field_spec_t<_statement_t, Columns>...>;
          return table(_get_statement());
        }

        size_t get_no_of_result_columns() const
        {
          return sizeof...(Columns);
        }

        // Execute
        template <typename Db>
        auto _run(Db& db) const -> result_t<decltype(db.select(std::declval<_statement_t>())), _result_row_t>
        {
          return {db.select(_get_statement())};
        }

        // Prepare
        template <typename Db>
        auto _prepare(Db& db) const -> prepared_select_t<Db, _statement_t>
        {
          return {make_parameter_list_t<_statement_t>{}, db.prepare_select(_get_statement())};
        }
      };
    };
  }  // namespace postgresql

  template <typename... Columns>
  struct has_result_row<postgresql::returning_column_list_t<Columns...>> : public std::true_type
  {
  };

  template <typename... Columns>
  struct is_clause<postgresql::returning_column_list_t<Columns...>> : public std::true_type
  {
  };

  template <typename Statement, typename... Columns>
  struct consistency_check<Statement, postgresql::returning_column_list_t<Columns...>>
  {
    using type = static_check_t<has_correct_aggregates<typename Statement::_all_provided_aggregates,
                                                       postgresql::returning_column_list_t<Columns...>>::value,
                                postgresql::assert_correct_returning_column_aggregates_t>;
  };

  template <typename Statement, typename... Columns>
  struct prepare_check<Statement, postgresql::returning_column_list_t<Columns...>>
  {
    using type = static_check_t<Statement::template _no_unknown_tables<postgresql::returning_column_list_t<Columns...>>,
                                postgresql::assert_no_unknown_tables_in_returning_columns_t>;
  };

  template <typename Column>
  struct value_type_of<postgresql::returning_column_list_t<Column>> : public value_type_of<Column>
  {
  };

  template <typename Column>
  struct name_tag_of<postgresql::returning_column_list_t<Column>> : public name_tag_of<Column>
  {
  };

  template <typename... Column>
  struct is_result_clause<postgresql::returning_column_list_t<Column...>> : public std::true_type
  {
  };

  namespace postgresql
  {

    template <typename ColumnTuple>
    struct make_returning_column_list;

    template <typename... Columns>
    struct make_returning_column_list<std::tuple<Columns...>>
    {
      using type = returning_column_list_t<Columns...>;
    };

    template <typename... Columns>
    using make_returning_column_list_t =
        typename make_returning_column_list<sqlpp::detail::flat_tuple_t<Columns...>>::type;

    struct no_returning_column_list_t
    {
    };

    // Serialization
    inline auto to_sql_string(postgresql::context_t&, const postgresql::no_returning_column_list_t&) -> std::string
    {
      return "";
    }

    template <typename... Columns>
    auto to_sql_string(postgresql::context_t& context, const postgresql::returning_column_list_t<Columns...>& t)
        -> std::string
    {
      return " RETURNING " + tuple_to_sql_string(context, t._columns, tuple_operand{", "});
    }
  }  // namespace postgresql

  template <typename Statement>
  struct consistency_check<Statement, postgresql::no_returning_column_list_t>
  {
    using type = consistent_t;
  };

  template <typename Statement>
  struct clause_base<postgresql::no_returning_column_list_t, Statement>
      : public clause_data<postgresql::no_returning_column_list_t, Statement>
  {
    using clause_data<postgresql::no_returning_column_list_t, Statement>::clause_data;

    template <typename... Columns, typename = sqlpp::enable_if_t<select_columns_have_values<Columns...>::value>>
    auto returning(Columns... columns) const
        -> decltype(new_statement(*this,
                                  postgresql::make_returning_column_list_t<Columns...>{
                                      std::tuple_cat(sqlpp::detail::tupelize(std::move(columns))...)}))
    {
      SQLPP_STATIC_ASSERT(sizeof...(Columns), "at least one return column required");
      SQLPP_STATIC_ASSERT(select_columns_have_names<Columns...>::value, "each return column must have a name");

      return new_statement(*this, postgresql::make_returning_column_list_t<Columns...>{
                                      std::tuple_cat(sqlpp::detail::tupelize(std::move(columns))...)});
    }
  };

}  // namespace sqlpp

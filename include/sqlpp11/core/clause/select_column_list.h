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

#include <sqlpp11/core/detail/flat_tuple.h>
#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/operator/expression_as.h>
#include <sqlpp11/core/query/dynamic.h>
#include <sqlpp11/core/field_spec.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/query/result_row.h>
#include <sqlpp11/core/clause/select_as.h>
#include <sqlpp11/core/clause/select_column_traits.h>
#include <sqlpp11/core/clause/select_columns_aggregate_check.h>
#include <sqlpp11/core/group_by_column.h>
#include <sqlpp11/core/basic/table.h>
#include <tuple>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_no_unknown_tables_in_selected_columns_t,
      "at least one selected column requires a table which is otherwise not known in the statement");

  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_no_unknown_static_tables_in_selected_columns_t,
      "at least one selected column statically requires a table which is otherwise not known dynamically in the statement");

  // SELECTED COLUMNS
  template <typename... Columns>
  struct select_column_list_t
  {
    select_column_list_t(std::tuple<Columns...> columns) : _columns(std::move(columns))
    {
    }

    select_column_list_t(const select_column_list_t&) = default;
    select_column_list_t(select_column_list_t&&) = default;
    select_column_list_t& operator=(const select_column_list_t&) = default;
    select_column_list_t& operator=(select_column_list_t&&) = default;
    ~select_column_list_t() = default;

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
        // This ensures that the sub select is free of table/CTE dependencies and consistent (e.g. not missing where
        // condition).
        statement_prepare_check_t<_statement_t>::verify();

        using table = select_as_t<_statement_t, name_tag_of_t<NameTagProvider>, make_field_spec_t<_statement_t, Columns>...>;
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
        return {{}, db.prepare_select(_get_statement())};
      }
    };
  };

  template <typename... Columns>
  struct is_clause<select_column_list_t<Columns...>> : public std::true_type
  {
  };

  template <typename... Columns>
  struct has_result_row<select_column_list_t<Columns...>> : public std::true_type
  {
  };

  namespace detail {
    // If a column is statically selected, the respective table needs to be statically provided, too.
    // Note that we are giving up analysis if a sub select uses tables from the enclosing query.
    template<typename Statement, typename Column>
      struct select_columns_dynamic_check
      {
        static constexpr bool uses_external_tables = not Statement::template _no_unknown_tables<Column>;
        using type = static_check_t<uses_external_tables or Statement::template _no_unknown_static_tables<Column>,
              assert_no_unknown_static_tables_in_selected_columns_t
          >;
      };

    template<typename Statement, typename Column>
      using select_columns_dynamic_check_t = typename select_columns_dynamic_check<Statement, Column>::type;
  }

  template <typename Statement, typename... Columns>
  struct consistency_check<Statement, select_column_list_t<Columns...>>
  {
    using AC = typename Statement::_all_provided_aggregates;
    static constexpr bool has_group_by = not AC::empty();

    using type = static_combined_check_t<
        detail::select_columns_aggregate_check_t<has_group_by,
                                                 Statement,
                                                 detail::remove_as_from_select_column_t<Columns>...>,
        detail::select_columns_dynamic_check_t<Statement, detail::remove_as_from_select_column_t<Columns>>...>;
  };

  template <typename Statement, typename... Columns>
  struct prepare_check<Statement, select_column_list_t<Columns...>>
  {
    using type = static_combined_check_t<
      static_check_t<Statement::template _no_unknown_tables<select_column_list_t<Columns...>>,
                                assert_no_unknown_tables_in_selected_columns_t>,
      static_check_t<Statement::template _no_unknown_static_tables<select_column_list_t<Columns...>>,
                                assert_no_unknown_static_tables_in_selected_columns_t>
                                  >;
  };

  template <typename Column>
  struct value_type_of<select_column_list_t<Column>> : public select_column_value_type_of<Column>
  {
  };

  template <typename... Columns>
  struct is_result_clause<select_column_list_t<Columns...>> : public std::true_type
  {
  };

  template <typename... Columns>
  struct nodes_of<select_column_list_t<Columns...>>
  {
    using type = detail::type_vector<Columns...>;
  };

  template <typename ColumnTuple>
  struct make_select_column_list;
  template <typename... Columns>
  struct make_select_column_list<std::tuple<Columns...>>
  {
    using type = select_column_list_t<Columns...>;
  };
  template <typename... Columns>
  using make_select_column_list_t = typename make_select_column_list<detail::flat_tuple_t<Columns...>>::type;

  SQLPP_WRAPPED_STATIC_ASSERT(assert_columns_selected_t, "selecting columns required");

  struct no_select_column_list_t
  {
  };

  template <typename Statement>
  struct clause_base<no_select_column_list_t, Statement> : public clause_data<no_select_column_list_t, Statement>
  {
    using clause_data<no_select_column_list_t, Statement>::clause_data;

    template <typename... Columns, typename = sqlpp::enable_if_t<select_columns_have_values<Columns...>::value>>
    auto columns(Columns... args) const
        -> decltype(new_statement(*this,
                                  make_select_column_list_t<Columns...>{
                                      std::tuple_cat(detail::tupelize(std::move(args))...)}))
    {
      SQLPP_STATIC_ASSERT(sizeof...(Columns), "at least one selected column required");
      SQLPP_STATIC_ASSERT(select_columns_have_names<Columns...>::value, "each selected column must have a name");

      return new_statement(*this,
              make_select_column_list_t<Columns...>{
                  std::tuple_cat(detail::tupelize(std::move(args))...)});
    }
  };

  template <typename Statement>
  struct consistency_check<Statement, no_select_column_list_t>
  {
    using type = assert_columns_selected_t;
  };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const no_select_column_list_t&) -> std::string
  {
    return "";
  }

  template <typename Context, typename... Columns>
  auto to_sql_string(Context& context, const select_column_list_t<Columns...>& t) -> std::string
  {
    //dynamic(false, foo.id) -> NULL as id
    //dynamic(false, foo.id).as(cheesecake) -> NULL AS cheesecake
    //max(something) -> max(something) as _max
    //max(something.as(cheesecake) -> max(something) AS cheesecake
    return tuple_to_sql_string(context, t._columns, tuple_operand_select_column{", "});
  }

  template <typename... T>
  auto select_columns(T&&... t) -> decltype(statement_t<no_select_column_list_t>().columns(std::forward<T>(t)...))
  {
    return statement_t<no_select_column_list_t>().columns(std::forward<T>(t)...);
  }

}  // namespace sqlpp

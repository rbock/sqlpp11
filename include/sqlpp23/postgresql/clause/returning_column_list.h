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

#include <sqlpp23/core/basic/table.h>
#include <sqlpp23/core/clause/select_as.h>
#include <sqlpp23/core/clause/select_column_traits.h>
#include <sqlpp23/core/detail/flat_tuple.h>
#include <sqlpp23/core/detail/type_set.h>
#include <sqlpp23/core/field_spec.h>
#include <sqlpp23/core/noop.h>
#include <sqlpp23/core/query/result_row.h>
#include <sqlpp23/core/tuple_to_sql_string.h>
#include <sqlpp23/postgresql/database/serializer_context.h>
#include <tuple>

namespace sqlpp {
namespace postgresql {
SQLPP_WRAPPED_STATIC_ASSERT(assert_no_unknown_tables_in_returning_columns_t,
                            "at least one returning column requires a table "
                            "which is otherwise not known in the statement");

SQLPP_WRAPPED_STATIC_ASSERT(
    assert_returning_columns_contain_no_aggregates_t,
    "returning columns must not contain aggregate functions");

template <typename... Columns> struct returning_column_list_t {
  std::tuple<Columns...> _columns;
};

template <typename... Columns> struct returning_column_list_result_methods_t {
  template <typename Statement, typename NameTagProvider>
  auto as(this Statement &&statement, const NameTagProvider &)
      -> select_as_t<std::decay_t<Statement>, name_tag_of_t<NameTagProvider>,
                     make_field_spec_t<std::decay_t<Statement>, Columns>...> {
    statement_consistency_check_t<std::decay_t<Statement>>::verify();
    using table =
        select_as_t<std::decay_t<Statement>, name_tag_of_t<NameTagProvider>,
                    make_field_spec_t<std::decay_t<Statement>, Columns>...>;
    return table(std::forward<Statement>(statement));
  }

  constexpr size_t get_no_of_result_columns() const {
    return sizeof...(Columns);
  }

  // Execute
  template <typename Statement, typename Db>
  auto _run(this Statement &&statement, Db &db) -> result_t<
      decltype(db.select(std::declval<std::decay_t<Statement>>())),
      result_row_t<make_field_spec_t<std::decay_t<Statement>, Columns>...>> {
    return {db.select(std::forward<Statement>(statement))};
  }

  // Prepare
  template <typename Statement, typename Db>
  auto _prepare(this Statement &&statement, Db &db)
      -> prepared_select_t<Db, std::decay_t<Statement>> {
    return {make_parameter_list_t<std::decay_t<Statement>>{},
            db.prepare_select(std::forward<Statement>(statement))};
  }
};
} // namespace postgresql

template <typename... Columns>
struct has_result_row<postgresql::returning_column_list_t<Columns...>>
    : public std::true_type {};

template <typename Statement, typename... Columns>
struct result_row_of<Statement,
                     postgresql::returning_column_list_t<Columns...>> {
  using type = result_row_t<make_field_spec_t<Statement, Columns>...>;
};

template <typename... Columns>
struct result_methods_of<postgresql::returning_column_list_t<Columns...>> {
  using type = postgresql::returning_column_list_result_methods_t<Columns...>;
};

template <typename... Columns>
struct nodes_of<postgresql::returning_column_list_t<Columns...>> {
  using type = detail::type_vector<Columns...>;
};

template <typename... Columns>
struct is_clause<postgresql::returning_column_list_t<Columns...>>
    : public std::true_type {};

template <typename Statement, typename... Columns>
struct consistency_check<Statement,
                         postgresql::returning_column_list_t<Columns...>> {
  using type = static_check_t<
      not contains_aggregate_function<
          postgresql::returning_column_list_t<Columns...>>::value,
      postgresql::assert_returning_columns_contain_no_aggregates_t>;
};

template <typename Statement, typename... Columns>
struct prepare_check<Statement,
                     postgresql::returning_column_list_t<Columns...>> {
  using type = static_check_t<
      Statement::template _no_unknown_tables<
          postgresql::returning_column_list_t<Columns...>>,
      postgresql::assert_no_unknown_tables_in_returning_columns_t>;
};

template <typename Column>
struct value_type_of<postgresql::returning_column_list_t<Column>>
    : public value_type_of<Column> {};

template <typename Column>
struct name_tag_of<postgresql::returning_column_list_t<Column>>
    : public name_tag_of<Column> {};

template <typename... Column>
struct is_result_clause<postgresql::returning_column_list_t<Column...>>
    : public std::true_type {};

namespace postgresql {

template <typename ColumnTuple> struct make_returning_column_list;

template <typename... Columns>
struct make_returning_column_list<std::tuple<Columns...>> {
  using type = returning_column_list_t<Columns...>;
};

template <typename... Columns>
using make_returning_column_list_t = typename make_returning_column_list<
    sqlpp::detail::flat_tuple_t<Columns...>>::type;

struct no_returning_column_list_t {
  template <typename Statement, typename... Columns>
    requires(select_columns_have_values<Columns...>::value)
  auto returning(this Statement &&statement, Columns... columns) {
    SQLPP_STATIC_ASSERT(sizeof...(Columns),
                        "at least one return column required");
    SQLPP_STATIC_ASSERT(select_columns_have_names<Columns...>::value,
                        "each return column must have a name");

    return new_statement<no_returning_column_list_t>(
        std::forward<Statement>(statement),
        make_returning_column_list_t<Columns...>{
            std::tuple_cat(sqlpp::detail::tupelize(std::move(columns))...)});
  }
};

// Serialization
inline auto to_sql_string(postgresql::context_t &,
                          const postgresql::no_returning_column_list_t &)
    -> std::string {
  return "";
}

template <typename... Columns>
auto to_sql_string(postgresql::context_t &context,
                   const postgresql::returning_column_list_t<Columns...> &t)
    -> std::string {
  return " RETURNING " +
         tuple_to_sql_string(context, t._columns, tuple_operand{", "});
}
} // namespace postgresql

template <typename Statement>
struct consistency_check<Statement, postgresql::no_returning_column_list_t> {
  using type = consistent_t;
};

namespace postgresql {
template <typename... Columns>
auto returning_columns(Columns... columns)
    -> decltype(statement_t<no_returning_column_list_t>{}.returning(
        std::move(columns)...)) {
  return statement_t<no_returning_column_list_t>{}.returning(
      std::move(columns)...);
}
} // namespace postgresql

} // namespace sqlpp

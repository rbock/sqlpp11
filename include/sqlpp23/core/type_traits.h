#pragma once

/*
 * Copyright (c) 2013, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
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

#include <optional>
#include <span>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#include <sqlpp23/core/consistent.h>
#include <sqlpp23/core/detail/get_first.h>
#include <sqlpp23/core/detail/type_set.h>
#include <sqlpp23/core/detail/type_vector.h>
#include <sqlpp23/core/name/name_tag.h>
#include <sqlpp23/core/operator/as_expression_fwd.h>
#include <sqlpp23/core/query/dynamic_fwd.h>
#include <sqlpp23/core/type_traits/aggregates.h>
#include <sqlpp23/core/type_traits/ctes_of.h>
#include <sqlpp23/core/type_traits/group_by_column.h>
#include <sqlpp23/core/type_traits/nodes_of.h>
#include <sqlpp23/core/type_traits/optional.h>
#include <sqlpp23/core/type_traits/tables_of.h>
#include <sqlpp23/core/type_traits/value_type.h>
#include <sqlpp23/core/wrapped_static_assert.h>

namespace sqlpp {
template <typename T> const T &get_value(const T &t) { return t; }

template <typename T> const T &get_value(const std::optional<T> &t) {
  return t.value();
}

template <typename T> auto has_value(const T &) -> bool { return true; }

template <typename T> auto has_value(const std::optional<T> &t) -> bool {
  return t.has_value();
}

template <typename T> struct has_default : public std::false_type {};

template <typename T>
struct can_be_null : public is_optional<value_type_of_t<T>> {};

template <> struct can_be_null<std::nullopt_t> : public std::true_type {};

template <typename T> struct dynamic_t;

template <typename T> struct is_dynamic : public std::false_type {};

template <typename T>
struct is_dynamic<dynamic_t<T>> : public std::true_type {};

template <typename L, typename R>
struct values_are_comparable
    : public std::integral_constant<
          bool, (is_blob<L>::value and is_blob<R>::value) or
                    (is_boolean<L>::value and is_boolean<R>::value) or
                    (is_numeric<L>::value and is_numeric<R>::value) or
                    (is_text<L>::value and is_text<R>::value) or
                    (is_day_or_time_point<L>::value and
                     is_day_or_time_point<R>::value) or
                    (is_time_of_day<L>::value and is_time_of_day<R>::value)> {};

template <typename T> struct result_value {};

template <typename T> struct result_value<std::optional<T>> {
  using type = std::optional<typename result_value<T>::type>;
};

template <typename T> using result_value_t = typename result_value<T>::type;

template <> struct result_value<blob> {
  using type = std::span<const uint8_t>;
};

template <> struct result_value<boolean> {
  using type = bool;
};

template <> struct result_value<integral> {
  using type = int64_t;
};

template <> struct result_value<unsigned_integral> {
  using type = uint64_t;
};

template <> struct result_value<floating_point> {
  using type = double;
};

template <> struct result_value<text> {
  using type = std::string_view;
};

template <> struct result_value<day_point> {
  using type =
      std::chrono::time_point<std::chrono::system_clock, std::chrono::days>;
};
template <> struct result_value<time_of_day> {
  using type = std::chrono::microseconds;
};

template <> struct result_value<time_point> {
  using type = std::chrono::time_point<std::chrono::system_clock,
                                       std::chrono::microseconds>;
};

template <typename T> struct parameter_value {};

template <typename T> struct parameter_value<std::optional<T>> {
  using type = std::optional<typename parameter_value<T>::type>;
};

template <typename T>
using parameter_value_t = typename parameter_value<T>::type;

template <> struct parameter_value<blob> {
  using type = std::vector<uint8_t>;
};

template <> struct parameter_value<boolean> {
  using type = bool;
};

template <> struct parameter_value<integral> {
  using type = int64_t;
};

template <> struct parameter_value<unsigned_integral> {
  using type = uint64_t;
};

template <> struct parameter_value<floating_point> {
  using type = double;
};

template <> struct parameter_value<text> {
  using type = std::string;
};

template <> struct parameter_value<day_point> {
  using type =
      std::chrono::time_point<std::chrono::system_clock, std::chrono::days>;
};
template <> struct parameter_value<time_of_day> {
  using type = std::chrono::microseconds;
};

template <> struct parameter_value<time_point> {
  using type = std::chrono::time_point<std::chrono::system_clock,
                                       std::chrono::microseconds>;
};

template <typename T> struct is_assignment : public std::false_type {};

template <typename T>
static inline constexpr bool is_assignment_v = is_assignment<T>::value;

template <typename T> struct lhs {
  using type = void;
};

template <typename T> struct lhs<dynamic_t<T>> {
  using type = dynamic_t<typename lhs<T>::type>;
};

template <typename T> using lhs_t = typename lhs<T>::type;

template <typename T> struct rhs {
  using type = void;
};

template <typename T> struct rhs<dynamic_t<T>> {
  using type = dynamic_t<typename rhs<T>::type>;
};

template <typename T> using rhs_t = typename rhs<T>::type;

template <typename T> struct parameters_of {
  using type = typename parameters_of<nodes_of_t<T>>::type;
};

template <typename... T> struct parameters_of<detail::type_vector<T...>> {
  using type = detail::type_vector_cat_t<typename parameters_of<T>::type...>;
};

template <typename T> using parameters_of_t = typename parameters_of<T>::type;

// Something that can be used as a table
template <typename T> struct is_table : public std::false_type {};

template <typename T>
static inline constexpr bool is_table_v = is_table<T>::value;

// Really a table, not a `table AS ...`, `JOIN` or `CTE` or `SELECT ... AS`
template <typename T> struct is_raw_table : public std::false_type {};

template <typename T>
static inline constexpr bool is_raw_table_v = is_raw_table<T>::value;

template <typename T> struct is_column : public std::false_type {};

template <typename T>
static inline constexpr bool is_column_v = is_column<T>::value;

template <typename NameTagProvider, typename Member>
using member_t =
    typename name_tag_of_t<NameTagProvider>::template _member_t<Member>;

template <typename Clauses>
using derived_statement_t = typename Clauses::_statement_t;

template <typename T>
using is_inconsistent_t =
    std::conditional_t<std::is_same<consistent_t, T>::value, std::false_type,
                       std::true_type>;

SQLPP_WRAPPED_STATIC_ASSERT(assert_run_statement_or_prepared_t,
                            "connection cannot run something that is neither "
                            "statement nor prepared statement");
SQLPP_WRAPPED_STATIC_ASSERT(
    assert_prepare_statement_t,
    "connection cannot prepare something that is not a statement");

template <typename T> struct is_statement : public std::false_type {};

// Checks whether a statement has a result row (i.e. select or union).
// Note: It does not check if the statement is actually consistent, preparable,
// or runnable.
template <typename T> struct has_result_row : public std::false_type {};

template <typename T> struct get_result_row {
  using type = void;
};

template <typename T> using get_result_row_t = typename get_result_row<T>::type;

template <typename T> struct requires_parentheses : public std::false_type {};

template <typename T> struct table_ref {
  using type = T;
};

template <typename T> using table_ref_t = typename table_ref<T>::type;

template <typename T> struct is_select_flag : public std::false_type {};

template <typename T> struct is_sort_order : public std::false_type {};

template <typename T>
static inline constexpr bool is_sort_order_v = is_sort_order<T>::value;

template <typename T> struct is_result_clause : public std::false_type {};

template <typename T> struct is_cte : public std::false_type {};

template <typename T> static inline constexpr bool is_cte_v = is_cte<T>::value;

template <typename T> struct is_as_expression : public std::false_type {};

template <typename T> struct is_recursive_cte : public std::false_type {};

template <typename T> struct is_pre_join : public std::false_type {};

template <typename T> struct required_insert_columns_of {
  using type = detail::type_set<>;
};
template <typename T>
using required_insert_columns_of_t =
    typename required_insert_columns_of<T>::type;

template <typename T> struct is_where_required : public std::false_type {};

template <typename T> struct is_clause : public std::false_type {};

// Check if a clause makes sense in the context of the whole statement.
// Note: This should /not/ be checking for missing tables as the statement might
// be used as a sub-select that /might/ be using columns from the enclosing
// statement.
//
// Note: This has no default implementation to ensure implementation for every
// clause.
template <typename Statement, typename Clause> struct consistency_check;

template <typename Statement, typename Clause>
using consistency_check_t = typename consistency_check<Statement, Clause>::type;

// Check if a clause within a statement is ready to be used in a prepared
// statement. This used in addition to the `consistency_check`.
//
// Implementation is optional for clauses, but it might be useful to check for
// missing tables.
template <typename Statement, typename Clause> struct prepare_check {
  using type = consistent_t;
};

template <typename Statement, typename Clause>
using prepare_check_t = typename prepare_check<Statement, Clause>::type;

// Check if a clause within a statement is ready to be run by the connection.
// This used in addition to the `consistency_check`.
//
// Implementation is optional for clauses, but it might be useful to check for
// missing tables.
template <typename Statement, typename Clause> struct run_check {
  using type = consistent_t;
};

template <typename Statement, typename Clause>
using run_check_t = typename run_check<Statement, Clause>::type;

// Not implemented to ensure implementation for statement_t
template <typename Statement> struct statement_consistency_check;

template <typename Statement>
using statement_consistency_check_t =
    typename statement_consistency_check<Statement>::type;

template <typename Statement> struct statement_prepare_check {
  using type = assert_prepare_statement_t;
};

template <typename Statement>
using statement_prepare_check_t =
    typename statement_prepare_check<Statement>::type;

template <typename Statement> struct statement_run_check {
  using type = assert_run_statement_or_prepared_t;
};

template <typename Statement>
using statement_run_check_t = typename statement_run_check<Statement>::type;

template <typename Clause> struct result_methods_of {};

template <typename Clause>
using result_methods_of_t = typename result_methods_of<Clause>::type;

template <typename Statement, typename Clause> struct result_row_of {
  using type = void;
};

template <typename Statement, typename Clause>
using result_row_of_t = typename result_row_of<Statement, Clause>::type;

template <typename T> struct is_select_column {
  static constexpr bool value =
      has_value_type_v<remove_as_t<remove_dynamic_t<T>>> and
      has_name_tag_v<remove_dynamic_t<T>>;
};

template <typename T>
static inline constexpr bool is_select_column_v = is_select_column<T>::value;

template <typename... T> struct is_select_column<std::tuple<T...>> {
  static constexpr bool value = (true and ... and is_select_column_v<T>);
};

template <typename Statement>
struct can_be_used_as_table : public std::false_type{};

template <typename StatementOrClause>
struct no_of_result_columns
{
  static constexpr size_t value = 0;
};

} // namespace sqlpp

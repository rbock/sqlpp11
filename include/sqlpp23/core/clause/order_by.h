#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <tuple>

#include <sqlpp23/core/clause/expression_static_check.h>
#include <sqlpp23/core/concepts.h>
#include <sqlpp23/core/detail/type_set.h>
#include <sqlpp23/core/logic.h>
#include <sqlpp23/core/query/statement.h>
#include <sqlpp23/core/reader.h>
#include <sqlpp23/core/tuple_to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
SQLPP_WRAPPED_STATIC_ASSERT(assert_no_unknown_tables_in_order_by_t,
                            "at least one order-by expression requires a table "
                            "which is otherwise not known in the statement");

SQLPP_WRAPPED_STATIC_ASSERT(
    assert_no_unknown_static_tables_in_order_by_t,
    "at least one order-by expression statically requires a table which is "
    "only known dynamically in the statement");

template <typename... Expressions>
struct order_by_t {
  order_by_t(Expressions... expressions) : _expressions(expressions...) {}

  order_by_t(const order_by_t&) = default;
  order_by_t(order_by_t&&) = default;
  order_by_t& operator=(const order_by_t&) = default;
  order_by_t& operator=(order_by_t&&) = default;
  ~order_by_t() = default;

 private:
  friend reader_t;
  std::tuple<Expressions...> _expressions;
};

template <typename Context, typename... Expressions>
auto to_sql_string(Context& context, const order_by_t<Expressions...>& t)
    -> std::string {
  return dynamic_tuple_clause_to_sql_string(context, "ORDER BY",
                                            read.expressions(t));
}

template <typename... Expressions>
struct is_clause<order_by_t<Expressions...>> : public std::true_type {};

SQLPP_WRAPPED_STATIC_ASSERT(
    assert_correct_order_by_aggregates_t,
    "order_by (without group by) must not contain any aggregates");

SQLPP_WRAPPED_STATIC_ASSERT(
    assert_correct_order_by_aggregates_with_group_by_t,
    "order_by (with group by) must contain aggregates only");

SQLPP_WRAPPED_STATIC_ASSERT(
    assert_correct_static_order_by_aggregates_with_group_by_t,
    "order_by statically contains aggregates that are only dynamically defined "
    "in group_by");

namespace detail {
template <typename ProvidedAggregates,
          typename ProvidedStaticAggregates,
          typename... Expressions>
struct check_order_by_aggregates;

template <typename ProvidedAggregates,
          typename ProvidedStaticAggregates,
          typename... Expressions>
using check_order_by_aggregates_t =
    typename check_order_by_aggregates<ProvidedAggregates,
                                       ProvidedStaticAggregates,
                                       Expressions...>::type;

// In case of provided aggregates all of the order by expressions have to be
// aggregates.
template <typename ProvidedAggregates,
          typename ProvidedStaticAggregates,
          typename... Expressions>
struct check_order_by_aggregates {
  using type = static_combined_check_t<
      static_check_t<
          logic::all<is_aggregate_expression<ProvidedAggregates,
                                             Expressions>::value...>::value,
          assert_correct_order_by_aggregates_with_group_by_t>,
      static_check_t<
          logic::all<static_part_is_aggregate_expression<
              ProvidedStaticAggregates,
              Expressions>::value...>::value,
          assert_correct_static_order_by_aggregates_with_group_by_t>>;
};

// In case of no provided aggregates all of the order by expressions have to be
// non-aggregates.
template <typename... Expressions>
struct check_order_by_aggregates<detail::type_set<>,
                                 detail::type_set<>,
                                 Expressions...> {
  using type = static_check_t<
      logic::all<is_non_aggregate_expression<detail::type_set<>,
                                             Expressions>::value...>::value,
      assert_correct_order_by_aggregates_t>;
};
}  // namespace detail

template <typename Statement, typename... Expressions>
struct consistency_check<Statement, order_by_t<Expressions...>> {
  using PA = typename Statement::_all_provided_aggregates;
  using PSA = typename Statement::_all_provided_static_aggregates;

  using type = static_combined_check_t<
      detail::check_order_by_aggregates_t<PA, PSA, Expressions...>,
      detail::expression_static_check_t<
          Statement,
          Expressions,
          assert_no_unknown_static_tables_in_order_by_t>...>;
};

template <typename Statement, typename... Expressions>
struct prepare_check<Statement, order_by_t<Expressions...>> {
  using type = static_combined_check_t<
      static_check_t<
          Statement::template _no_unknown_tables<order_by_t<Expressions...>>,
          assert_no_unknown_tables_in_order_by_t>,
      static_check_t<Statement::template _no_unknown_static_tables<
                         order_by_t<Expressions...>>,
                     assert_no_unknown_static_tables_in_order_by_t>>;
};

template <typename... Expressions>
struct nodes_of<order_by_t<Expressions...>> {
  using type = detail::type_vector<Expressions...>;
};

// NO ORDER BY YET
struct no_order_by_t {
  template <typename Statement, DynamicSortOrder... Expressions>
  auto order_by(this Statement&& statement, Expressions... expressions) {
    SQLPP_STATIC_ASSERT(sizeof...(Expressions),
                        "at least one sort-order expression (e.g. "
                        "column.asc()) required in order_by()");

    SQLPP_STATIC_ASSERT(
        not detail::has_duplicates<remove_dynamic_t<Expressions>...>::value,
        "at least one duplicate argument detected in order_by()");

    return new_statement<no_order_by_t>(
        std::forward<Statement>(statement),
        order_by_t<Expressions...>{std::move(expressions)...});
  }
};

template <typename Context>
auto to_sql_string(Context&, const no_order_by_t&) -> std::string {
  return "";
}

template <typename Statement>
struct consistency_check<Statement, no_order_by_t> {
  using type = consistent_t;
};

template <DynamicSortOrder... Expressions>
auto order_by(Expressions... expressions) {
  return statement_t<no_order_by_t>().order_by(std::move(expressions)...);
}

}  // namespace sqlpp

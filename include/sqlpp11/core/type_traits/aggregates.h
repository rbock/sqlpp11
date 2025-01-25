#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/core/detail/type_vector.h>
#include <sqlpp11/core/type_traits/nodes_of.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/query/dynamic_fwd.h>

namespace sqlpp
{
  // We don't want to mix aggregate and non-aggregate expressions as the results are unspecified.
  // Aggregates are either results of aggregate functions or GROUP BY columns.
  // Non-aggregates are columns (unless they are aggregate columns).
  // Constant values are neutral.

  template <typename T>
  struct is_aggregate_function : public std::false_type
  {
  };

  // Finds calls to aggregate functions (avg, count, max, min, sum) in expressions.
  // This is important as aggregated functions must not be nested.
  template <typename T>
  struct contains_aggregate_function
      : public std::integral_constant<bool,
                                      is_aggregate_function<T>::value or
                                          contains_aggregate_function<nodes_of_t<T>>::value>
  {
  };

  template <typename... T>
  struct contains_aggregate_function<detail::type_vector<T...>>
      : public std::integral_constant<
            bool,
            logic::any<(is_aggregate_function<T>::value or contains_aggregate_function<T>::value)...>::value>
  {
  };

  // Obtain known aggregate columns, i.e. GROUP BY columns.
  template <typename T>
  struct known_aggregate_columns_of
  {
    using type = detail::type_set<>;
  };

  template <typename T>
  using known_aggregate_columns_of_t = typename known_aggregate_columns_of<T>::type;

  template <typename T>
  struct known_static_aggregate_columns_of
  {
    using type = detail::type_set<>;
  };

  template <typename T>
  using known_static_aggregate_columns_of_t = typename known_static_aggregate_columns_of<T>::type;

  template <typename T>
  struct is_aggregate_neutral : public std::true_type
  {
  };

  // Checks if T is an aggregate expression, i.e. either
  //  - T is an aggregate function,
  //  - T is a known aggregate, or
  //  - T is aggregate-neutral, or
  //  - T exclusively exists of aggregate expressions.
  // @KnownAggregateColumns: type_set as obtained through known_aggregate_columns_of_t
  template <typename KnownAggregateColumns, typename T>
  struct is_aggregate_expression
      : public std::integral_constant<
            bool,
            is_aggregate_function<T>::value or KnownAggregateColumns::template contains<T>() or
                (not nodes_of_t<T>::empty() and is_aggregate_expression<KnownAggregateColumns, nodes_of_t<T>>::value) or
                (nodes_of_t<T>::empty() and is_aggregate_neutral<T>::value)>
  {
  };

  template <typename KnownAggregateColumns, typename... T>
  struct is_aggregate_expression<KnownAggregateColumns, detail::type_vector<T...>>
  {
    static constexpr bool value = logic::all<is_aggregate_expression<KnownAggregateColumns, T>::value...>::value;
  };

  // Checks if the static part of T is an aggregate expression, see above.
  // @KnownStaticAggregateColumns: type_set as obtained through known_static_aggregate_columns_of_t
  template <typename KnownStaticAggregateColumns, typename T>
  struct static_part_is_aggregate_expression
      : public std::integral_constant<
            bool,
            is_aggregate_function<T>::value or KnownStaticAggregateColumns::template contains<T>() or
                (not nodes_of_t<T>::empty() and
                 static_part_is_aggregate_expression<KnownStaticAggregateColumns, nodes_of_t<T>>::value) or
                (nodes_of_t<T>::empty() and is_aggregate_neutral<T>::value)>
  {
  };

  template <typename KnownStaticAggregateColumns, typename T>
  struct static_part_is_aggregate_expression<KnownStaticAggregateColumns, dynamic_t<T>> : public std::true_type{};

  template <typename KnownStaticAggregateColumns, typename... T>
  struct static_part_is_aggregate_expression<KnownStaticAggregateColumns, detail::type_vector<T...>>
  {
    static constexpr bool value =
        logic::all<static_part_is_aggregate_expression<KnownStaticAggregateColumns, T>::value...>::value;
  };

  // Checks if T is an non-aggregate expression, i.e. 
  //  - T is not an aggregate function, and
  //  - T is not a known aggregate, and
  //  - T exclusively exists of non-aggregate expressions, or
  //  - T is aggregate-neutral
  // @KnownAggregateColumns: type_set as obtained through known_aggregate_columns_of_t
  template <typename KnownAggregateColumns, typename T>
  struct is_non_aggregate_expression
      : public std::integral_constant<bool,
                                      (not is_aggregate_function<T>::value and
                                       not KnownAggregateColumns::template contains<T>() and
                                       is_non_aggregate_expression<KnownAggregateColumns, nodes_of_t<T>>::value) or
                                          (nodes_of_t<T>::empty() and is_aggregate_neutral<T>::value)>
  {
  };


  template <typename KnownAggregateColumns, typename... T>
  struct is_non_aggregate_expression<KnownAggregateColumns, detail::type_vector<T...>>
  {
    static constexpr bool value = logic::all<is_non_aggregate_expression<KnownAggregateColumns, T>::value...>::value;
  };

  // If a GROUP BY clause defines known aggregate columns or the SELECT columns contain an aggregate function then ALL
  // columns need to be aggregate.
  // SELECT-like clauses will need to specialize this.
  template <typename KnownAggregateColumns, typename T>
  struct has_correct_aggregates : public std::true_type {};

}  // namespace sqlpp11


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
    using type = detail::type_vector<>;
  };

  template <typename T>
  using known_aggregate_columns_of_t = typename known_aggregate_columns_of<T>::type;

  // Checks if T is an aggregate expression (either an aggregate function or a known aggregate).
  // @KnownAggregateColumns: type_vector as obtained through known_aggregate_columns_of_t
  template <typename KnownAggregateColumns, typename T>
  struct is_aggregate_expression
      : public std::integral_constant<bool,
                                      is_aggregate_function<T>::value or
                                          KnownAggregateColumns::template contains<T>()>
  {
  };

  // If a GROUP BY clause defines known aggregate columns or the SELECT columns contain an aggregate function then ALL
  // columns need to be aggregate.
  // SELECT-like clauses will need to specialize this.
  template <typename KnownAggregateColumns, typename T>
  struct has_correct_aggregates : public std::true_type {};

}  // namespace sqlpp11


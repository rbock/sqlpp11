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
  // Finds calls to aggregate functions (avg, count, max, min, sum).
  template <typename T>
  struct contains_aggregate_function : public std::integral_constant<bool, contains_aggregate_function<nodes_of_t<T>>::value>
  {
  };

  template <typename... T>
  struct contains_aggregate_function<detail::type_vector<T...>>
      : public std::integral_constant<bool, logic::any_t<contains_aggregate_function<T>::value...>::value>
  {
  };

  // Finds group_by expression.
  // @GroupByExpressions: type_vector
  template <typename GroupByExpressions, typename T>
  struct contains_aggregate_expression
      : public std::integral_constant<bool,
                                      GroupByExpressions::template contains<T>::value or
                                          contains_aggregate_expression<GroupByExpressions, nodes_of_t<T>>::value>
  {
  };

  template <typename GroupByExpressions, typename... T>
  struct contains_aggregate_expression<GroupByExpressions, detail::type_vector<T...>>
      : public std::integral_constant<
            bool,
            logic::any_t<(GroupByExpressions::template contains<T>::value or
                         contains_aggregate_expression<GroupByExpressions, T>::value)...>::value>
  {
  };

  // Finds columns.
  // Note that explicit values like `value(7)` are compatible with both aggregate and non-aggregate.
  template <typename T>
  struct contains_non_aggregate : public std::integral_constant<bool, contains_non_aggregate<nodes_of_t<T>>::value>
  {
  };

  template <typename... T>
  struct contains_non_aggregate<detail::type_vector<T...>>
      : public std::integral_constant<bool, logic::any_t<contains_non_aggregate<T>::value...>::value>
  {
  };

}  // namespace sqlpp11


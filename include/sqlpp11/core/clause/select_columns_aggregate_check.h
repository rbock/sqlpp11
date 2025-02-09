#pragma once

/*
 * Copyright (c) 2025, Roland Bock
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

#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_with_group_by_are_aggregates_t,
                               "with group_by, selected columns must be aggregate expressions");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_with_group_by_match_static_aggregates_t,
                               "with group_by, static parts of selected columns must match static group_by columns");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_all_aggregates_t,
                               "without group_by, selected columns must not be a mix of aggregate and non-aggregate expressions");

  namespace detail
  {
    // Columns can be
    // - Column
    // - dynamic_t<Column>
    // - expression_as_t<Column, NameTag>
    // - dynamic_t<expression_as_t<Column, NameTag>>
    //
    // In order to analyse aggregate expressions, we want to ignore the expression_as aspects, but preserve the dynamic nature.
    template <typename Column>
    struct remove_as_from_select_column
    {
      using type = Column;
    };
    template <typename Column, typename NameTag>
    struct remove_as_from_select_column<expression_as<Column, NameTag>>
    {
      using type = Column;
    };
    template <typename Column, typename NameTag>
    struct remove_as_from_select_column<dynamic_t<expression_as<Column, NameTag>>>
    {
      using type = dynamic_t<Column>;
    };

    template <typename Column>
    using remove_as_from_select_column_t = typename remove_as_from_select_column<Column>::type;

    // Select columns have different constraints, depending on whether a group by is present or not.
    template <bool WithGroupBy, typename Statement, typename... Columns>
    struct select_columns_aggregate_check;

    // In the presence of group_by aggregates, all select columns have to be aggregate expressions.
    template <typename Statement, typename... Columns>
    struct select_columns_aggregate_check<true, Statement, Columns...>
    {
      using AC = typename Statement::_all_provided_aggregates;
      using SAC = typename Statement::_all_provided_static_aggregates;

      static constexpr bool all_aggregate = logic::all<is_aggregate_expression<AC, Columns>::value...>::value;
      static constexpr bool all_static_aggregate =
          logic::all<static_part_is_aggregate_expression<SAC, Columns>::value...>::value;

      using type = static_combined_check_t<
          static_check_t<all_aggregate, assert_select_columns_with_group_by_are_aggregates_t>,
          static_check_t<all_static_aggregate, assert_select_columns_with_group_by_match_static_aggregates_t>>;
    };

    // In the absence of group_by aggregates, either
    // - all select columns have to be non aggregate expression (or neutral)
    // - all select columns have to be aggregate expressions (or neutral)
    template <typename Statement, typename... Columns>
    struct select_columns_aggregate_check<false, Statement, Columns...>
    {
      using AC = typename Statement::_all_provided_aggregates;
      using SAC = typename Statement::_all_provided_static_aggregates;

      static constexpr bool all_aggregate = logic::all<is_aggregate_expression<AC, Columns>::value...>::value;
      static constexpr bool no_aggregate = logic::all<is_non_aggregate_expression<AC, Columns>::value...>::value;

      using type = std::conditional_t<
          no_aggregate,
          consistent_t,
          static_combined_check_t<static_check_t<all_aggregate, assert_select_columns_all_aggregates_t>>>;
    };

    template <bool HasGroupBy, typename Statement, typename... Columns>
    using select_columns_aggregate_check_t =
        typename select_columns_aggregate_check<HasGroupBy, Statement, Columns...>::type;
  }  // namespace detail

}  // namespace sqlpp

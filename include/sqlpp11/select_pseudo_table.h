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

#include <sqlpp11/data_types/no_value.h>
#include <sqlpp11/table.h>

namespace sqlpp
{
  // FIXME: We might use field specs here (same as with cte)
  //
  // provide type information for sub-selects that are used as named expressions or tables
  template <typename Select, typename NamedExpr>
  struct select_column_spec_t
  {
    using _alias_t = typename NamedExpr::_alias_t;

    static constexpr bool _can_be_null = can_be_null_t<NamedExpr>::value;
    static constexpr bool _depends_on_outer_table =
        detail::make_intersect_set_t<required_tables_of<NamedExpr>, typename Select::_used_outer_tables>::size::value >
        0;

    using _traits = make_traits<value_type_of<NamedExpr>,
                                tag::must_not_insert,
                                tag::must_not_update,
                                tag_if<tag::can_be_null, _can_be_null or _depends_on_outer_table>>;
  };

  template<typename... NamedExpr>
  struct select_expression_type {
    using value_t = no_value_t;
    static constexpr bool _is_expression = false;
    static constexpr bool _can_be_null = false;
  };

  template<typename NamedExpr>
  struct select_expression_type<NamedExpr> {
    using value_t = value_type_of<NamedExpr>;
    static constexpr bool _is_expression = true;
    static constexpr bool _can_be_null = can_be_null_t<NamedExpr>::value;
  };

  template <typename Select, typename... NamedExpr>
  struct select_pseudo_table_t
      : public table_t<select_pseudo_table_t<Select, NamedExpr...>, select_column_spec_t<Select, NamedExpr>...>
  {
    using _expr_t = select_expression_type<NamedExpr...>;
    using _traits = make_traits<
        // Usage as named expression
        typename _expr_t::value_t,
        tag_if<tag::is_expression, _expr_t::_is_expression>,
        tag_if<tag::can_be_null, _expr_t::_can_be_null>,
        // Usage as table
        tag::is_table,
        tag::is_pseudo_table,
        tag_if<tag::requires_parens, requires_parens_t<Select>::value>>;
    using _nodes = detail::type_vector<>;

    select_pseudo_table_t(Select select) : _select(select)
    {
    }

    select_pseudo_table_t(const select_pseudo_table_t& rhs) = default;
    select_pseudo_table_t(select_pseudo_table_t&& rhs) = default;
    select_pseudo_table_t& operator=(const select_pseudo_table_t& rhs) = default;
    select_pseudo_table_t& operator=(select_pseudo_table_t&& rhs) = default;
    ~select_pseudo_table_t() = default;

    Select _select;
  };

  template <typename Context, typename Select, typename... NamedExpr>
  Context& serialize(const select_pseudo_table_t<Select, NamedExpr...>& t, Context& context)
  {
    serialize(t._select, context);
    return context;
  }
}  // namespace sqlpp

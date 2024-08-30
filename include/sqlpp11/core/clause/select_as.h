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

#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/operator/enable_as.h>
#include <sqlpp11/core/basic/enable_join.h>

namespace sqlpp
{
  // pseudo_column_t is the column of a sub-select used as a table, for instance.
#warning: Maybe use this for CTE as well
#warning: Should be in its own header
  template <typename TableAliasProvider, typename ColumnSpec>
  struct pseudo_column_t : public enable_as<pseudo_column_t<TableAliasProvider, ColumnSpec>>
  {
  };
  template <typename TableAliasProvider, typename ColumnSpec>
  struct name_tag_of<pseudo_column_t<TableAliasProvider, ColumnSpec>> : public name_tag_of<ColumnSpec>
  {
  };

  template <typename TableAliasProvider, typename ColumnSpec>
  struct value_type_of<pseudo_column_t<TableAliasProvider, ColumnSpec>> : public value_type_of<ColumnSpec>
  {
  };

  template <typename Context, typename TableAliasProvider, typename ColumnSpec>
  auto to_sql_string(Context& context, const pseudo_column_t<TableAliasProvider, ColumnSpec>& t) -> std::string
  {
    using T = pseudo_column_t<TableAliasProvider, ColumnSpec>;

    return name_to_sql_string(context, name_tag_of_t<TableAliasProvider>::name) + "."+    name_to_sql_string(context, name_tag_of_t<T>::name);
  }

  template <typename Select, typename AliasProvider, typename... ColumnSpecs>
  struct select_as_t : public ColumnSpecs::_sqlpp_name_tag::template _member_t<pseudo_column_t<AliasProvider, ColumnSpecs>>...,
                       public enable_join<select_as_t<Select, AliasProvider, ColumnSpecs...>>
  {
    select_as_t(Select select) : _select(select)
    {
    }

    select_as_t(const select_as_t& rhs) = default;
    select_as_t(select_as_t&& rhs) = default;
    select_as_t& operator=(const select_as_t& rhs) = default;
    select_as_t& operator=(select_as_t&& rhs) = default;
    ~select_as_t() = default;

    Select _select;
  };

  // The Select expression has a value in case it has just one column selected.
  template<typename Select, typename AliasProvider, typename ColumnSpec>
    struct value_type_of<select_as_t<Select, AliasProvider, ColumnSpec>> : value_type_of<Select> {};

  template<typename Select, typename AliasProvider, typename... ColumnSpecs>
    struct name_tag_of<select_as_t<Select, AliasProvider, ColumnSpecs...>> : name_tag_of<AliasProvider> {};

  template <typename Select, typename AliasProvider, typename... ColumnSpecs>
  struct is_table<select_as_t<Select, AliasProvider, ColumnSpecs...>>
      : std::integral_constant<bool, Select::_can_be_used_as_table()>
  {
  };

  template <typename Select, typename AliasProvider, typename... ColumnSpecs>
  struct provided_tables_of<select_as_t<Select, AliasProvider, ColumnSpecs...>>
      : public std::conditional<Select::_can_be_used_as_table(),
                                sqlpp::detail::type_vector<select_as_t<Select, AliasProvider, ColumnSpecs...>>,
                                sqlpp::detail::type_vector<>>
  {
  };

  template <typename Select, typename AliasProvider, typename... ColumnSpecs>
  struct provided_static_tables_of<select_as_t<Select, AliasProvider, ColumnSpecs...>>
      : public provided_tables_of<select_as_t<Select, AliasProvider, ColumnSpecs...>>
  {
  };

  template <typename Select, typename AliasProvider, typename... ColumnSpecs>
  struct provided_optional_tables_of<select_as_t<Select, AliasProvider, ColumnSpecs...>>
      : public provided_tables_of<select_as_t<Select, AliasProvider, ColumnSpecs...>>
  {
  };

#warning: V1.0 has empty nodes. Is that correct? In either case document the decision here.
#warning: Need to add required tables of

  template <typename Context, typename Select, typename AliasProvider, typename... ColumnSpecs>
  auto to_sql_string(Context& context, const select_as_t<Select, AliasProvider, ColumnSpecs...>& t) -> std::string
  {
    return operand_to_sql_string(context, t._select) + " AS " +name_to_sql_string(context, name_tag_of_t<AliasProvider>::name);
  }
}  // namespace sqlpp

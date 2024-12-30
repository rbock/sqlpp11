#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include <sqlpp11/core/operator/enable_as.h>
#include <sqlpp11/core/operator/enable_comparison.h>
#include <sqlpp11/core/operator/expression_as.h>
#include <sqlpp11/core/operator/assign_expression.h>
#include <sqlpp11/core/basic/column_fwd.h>
#include <sqlpp11/core/default_value.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/wrong.h>
#include <sqlpp11/core/detail/type_vector.h>

namespace sqlpp
{
  // Table can be a table_t or a cte_ref_t or a select_ref_t
  template <typename Table, typename ColumnSpec>
  struct column_t : public enable_as<column_t<Table, ColumnSpec>>,
  public enable_comparison<column_t<Table, ColumnSpec>>
  {
    using _spec_t = ColumnSpec;
    using _table = Table;

    column_t() = default;
    column_t(const column_t&) = default;
    column_t(column_t&&) = default;
    column_t& operator=(const column_t&) = default;
    column_t& operator=(column_t&&) = default;
    ~column_t() = default;

    static auto table() -> _table
    {
      return _table{};
    }

    template <typename T>
    auto operator=(T value) const -> assign_expression<column_t, op_assign, T>
    {
      return assign(*this, std::move(value));
    }

    template <typename T>
    auto operator+=(T value) const -> decltype(plus_assign(*this, std::declval<T>()))
    {
      return plus_assign(*this, std::move(value));
    }

    template <typename T>
    auto operator-=(T value) const -> decltype(minus_assign(*this, std::declval<T>()))
    {
      return minus_assign(*this, std::move(value));
    }
  };

  template<typename Table, typename ColumnSpec>
  struct is_aggregate_neutral<column_t<Table, ColumnSpec>> : public std::false_type
  {
  };

  template<typename Table, typename ColumnSpec>
  struct has_default<column_t<Table, ColumnSpec>> : public ColumnSpec::has_default
  {
  };

  template<typename Table, typename ColumnSpec>
  struct is_column<column_t<Table, ColumnSpec>> : public std::true_type
  {
  };

  // Can be used in group_by
  template<typename Table, typename ColumnSpec>
  struct is_group_by_column<column_t<Table, ColumnSpec>> : public std::true_type
  {
  };

  template<typename Table, typename ColumnSpec>
  struct name_tag_of<column_t<Table, ColumnSpec>> : public name_tag_of<ColumnSpec>{};

  template<typename Table, typename ColumnSpec>
  struct required_tables_of<column_t<Table, ColumnSpec>>
  {
    using type = detail::type_set<Table>;
  };

  template <typename Table, typename ColumnSpec>
  struct required_static_tables_of<column_t<Table, ColumnSpec>> : public required_tables_of<column_t<Table, ColumnSpec>>
  {
  };

  template<typename Table, typename ColumnSpec>
  struct value_type_of<column_t<Table, ColumnSpec>>
  {
    using type = typename ColumnSpec::value_type;
  };

  template <typename Context, typename Table, typename ColumnSpec>
  auto to_sql_string(Context& context, const column_t<Table, ColumnSpec>&) -> std::string
  {
    using T = column_t<Table, ColumnSpec>;

    return name_to_sql_string(context, name_tag_of_t<Table>{}) + "." +
           name_to_sql_string(context, name_tag_of_t<T>{});
  }
}  // namespace sqlpp

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

#ifndef SQLPP_COLUMN_H
#define SQLPP_COLUMN_H

#include <sqlpp11/alias.h>
#include <sqlpp11/column_fwd.h>
#include <sqlpp11/default_value.h>
#include <sqlpp11/null.h>
#include <sqlpp11/sort_order.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/assignment.h>
#include <sqlpp11/expression.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/wrong.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
  template <typename Table, typename ColumnSpec>
  struct column_t : public expression_operators<column_t<Table, ColumnSpec>, value_type_of<ColumnSpec>>,
                    public column_operators<column_t<Table, ColumnSpec>, value_type_of<ColumnSpec>>
  {
    struct _traits
    {
      using _value_type = value_type_of<ColumnSpec>;
      using _tags = detail::make_joined_set_t<detail::type_set<tag::is_column, tag::is_expression, tag::is_selectable>,
                                              typename ColumnSpec::_traits::_tags>;
    };

    using _nodes = detail::type_vector<>;
    using _required_tables = detail::type_set<Table>;
    using _can_be_null = column_spec_can_be_null_t<ColumnSpec>;

    using _spec_t = ColumnSpec;
    using _table = Table;
    using _alias_t = typename _spec_t::_alias_t;

    template <typename T>
    using _is_valid_assignment_operand = is_valid_assignment_operand<value_type_of<ColumnSpec>, T>;

    column_t() = default;
    column_t(const column_t&) = default;
    column_t(column_t&&) = default;
    column_t& operator=(const column_t&) = default;
    column_t& operator=(column_t&&) = default;
    ~column_t() = default;

    template <typename T = _table>
    auto table() const -> _table
    {
      static_assert(is_table_t<T>::value, "cannot call get_table for columns of a sub-selects or cte");
      return _table{};
    }

    template <typename alias_provider>
    expression_alias_t<column_t, alias_provider> as(const alias_provider&) const
    {
      return {*this};
    }

    template <typename T>
    auto operator=(T t) const -> assignment_t<column_t, wrap_operand_t<T>>
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_assignment_operand<rhs>::value, "invalid rhs assignment operand");

      return {*this, {rhs{t}}};
    }

    auto operator=(null_t) const -> assignment_t<column_t, null_t>
    {
      static_assert(can_be_null_t<column_t>::value, "column cannot be null");
      return {*this, null_t{}};
    }

    auto operator=(default_value_t) const -> assignment_t<column_t, default_value_t>
    {
      return {*this, default_value_t{}};
    }
  };

  // workaround for msvs bug https://connect.microsoft.com/VisualStudio/feedback/details/2173053
  //  template <typename Context, typename... Args>
  //  struct serializer_t<Context, column_t<Args...>>
  //  {
  //	  using _serialize_check = consistent_t;
  //	  using T = column_t<Args...>;
  //
  //	  static Context& _(const T&, Context& context)
  //	  {
  //		  context << name_of<typename T::_table>::char_ptr() << '.' << name_of<T>::char_ptr();
  //		  return context;
  //	  }
  //  };
  template <typename Context, typename Args1, typename Args2>
  struct serializer_t<Context, column_t<Args1, Args2>>
  {
    using _serialize_check = consistent_t;
    using T = column_t<Args1, Args2>;

    static Context& _(const T&, Context& context)
    {
      context << name_of<typename T::_table>::char_ptr() << '.' << name_of<T>::char_ptr();
      return context;
    }
  };
}

#endif

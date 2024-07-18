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

#include <sqlpp11/enable_as.h>
#include <sqlpp11/operator/as_expression.h>
#include <sqlpp11/operator/assign_expression.h>
#include <sqlpp11/column_fwd.h>
#include <sqlpp11/default_value.h>
#include <sqlpp11/sort_order.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/wrong.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
#warning: need to reactivate column operators?
  template <typename Table, typename ColumnSpec>
  struct column_t : public enable_as<column_t<Table, ColumnSpec>>
                    // : public expression_operators<column_t<Table, ColumnSpec>, typename ColumnSpec::value_type>,
                 //   public column_operators<column_t<Table, ColumnSpec>, typename ColumnSpec::value_type>
  {
    struct _traits
    {
      using _value_type = typename ColumnSpec::value_type;
      using _tags = detail::make_joined_set_t<detail::type_set<tag::is_column, tag::is_expression, tag::is_selectable>>;
    };

    using _nodes = detail::type_vector<>;
    using _required_tables = detail::type_set<Table>;

    using _spec_t = ColumnSpec;
    using _table = Table;
    using _alias_t = typename _spec_t::_alias_t;

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

    template <typename T>
    auto operator=(T value) const -> assign_expression<column_t, T>
    {
      return assign(*this, std::move(value));
    }
  };

  template<typename Table, typename ColumnSpec>
  struct value_type_of<column_t<Table, ColumnSpec>>
  {
    using type = typename ColumnSpec::value_type;
  };

  template<typename Table, typename ColumnSpec>
  struct has_default<column_t<Table, ColumnSpec>> : public ColumnSpec::has_default
  {
  };

  template<typename Table, typename ColumnSpec>
  struct has_name<column_t<Table, ColumnSpec>> : std::true_type
  {
  };

  template <typename Context, typename Table, typename ColumnSpec>
  Context& serialize(Context& context, const column_t<Table, ColumnSpec>&)
  {
    using T = column_t<Table, ColumnSpec>;

    context << name_of<typename T::_table>::template char_ptr<Context>() << '.'
            << name_of<T>::template char_ptr<Context>();
    return context;
  }
}  // namespace sqlpp

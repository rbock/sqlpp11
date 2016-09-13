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

#ifndef SQLPP_TABLE_ALIAS_H
#define SQLPP_TABLE_ALIAS_H

#include <sqlpp11/alias.h>
#include <sqlpp11/column_fwd.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/interpret.h>
#include <sqlpp11/join.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173198
  //  template <typename AliasProvider, typename Table, typename... ColumnSpec>
  //  struct table_alias_t : public member_t<ColumnSpec, column_t<AliasProvider, ColumnSpec>>...
  template <typename AliasProvider, typename Table, typename... ColumnSpec>
  struct table_alias_t : public ColumnSpec::_alias_t::template _member_t<column_t<AliasProvider, ColumnSpec>>...
  {
    using _traits = make_traits<value_type_of<Table>,
                                tag::is_table,
                                tag::is_alias,
                                tag_if<tag::is_selectable, is_expression_t<Table>::value>>;

    using _nodes = detail::type_vector<>;
    using _required_ctes = required_ctes_of<Table>;
    using _provided_tables = detail::type_set<AliasProvider>;

    static_assert(required_tables_of<Table>::size::value == 0, "table aliases must not depend on external tables");

    using _alias_t = typename AliasProvider::_alias_t;
    using _column_tuple_t = std::tuple<column_t<AliasProvider, ColumnSpec>...>;

    table_alias_t(Table table) : _table(table)
    {
    }

    template <typename T>
    auto join(T t) const -> decltype(::sqlpp::join(*this, t))
    {
      return ::sqlpp::join(*this, t);
    }

    template <typename T>
    auto inner_join(T t) const -> decltype(::sqlpp::inner_join(*this, t))
    {
      return ::sqlpp::inner_join(*this, t);
    }

    template <typename T>
    auto left_outer_join(T t) const -> decltype(::sqlpp::left_outer_join(*this, t))
    {
      return ::sqlpp::left_outer_join(*this, t);
    }

    template <typename T>
    auto right_outer_join(T t) const -> decltype(::sqlpp::right_outer_join(*this, t))
    {
      return ::sqlpp::right_outer_join(*this, t);
    }

    template <typename T>
    auto outer_join(T t) const -> decltype(::sqlpp::outer_join(*this, t))
    {
      return ::sqlpp::outer_join(*this, t);
    }

    template <typename T>
    auto cross_join(T t) const -> decltype(::sqlpp::cross_join(*this, t))
    {
      return ::sqlpp::cross_join(*this, t);
    }

    Table _table;
  };

  template <typename Context, typename AliasProvider, typename Table, typename... ColumnSpec>
  struct serializer_t<Context, table_alias_t<AliasProvider, Table, ColumnSpec...>>
  {
    using _serialize_check = serialize_check_of<Context, Table>;
    using T = table_alias_t<AliasProvider, Table, ColumnSpec...>;

    static Context& _(const T& t, Context& context)
    {
      if (requires_braces_t<Table>::value)
        context << "(";
      serialize(t._table, context);
      if (requires_braces_t<Table>::value)
        context << ")";
      context << " AS " << name_of<T>::char_ptr();
      return context;
    }
  };
}

#endif

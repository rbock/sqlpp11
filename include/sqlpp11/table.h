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

#ifndef SQLPP_TABLE_H
#define SQLPP_TABLE_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/table_alias.h>
#include <sqlpp11/all_of.h>
#include <sqlpp11/column.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/join.h>
#include <sqlpp11/data_types/no_value.h>

namespace sqlpp
{
  struct table_base_t
  {
  };

  // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173198
  //  template <typename Table, typename... ColumnSpec>
  //  struct table_t : public table_base_t, public member_t<ColumnSpec, column_t<Table, ColumnSpec>>...
  template <typename Table, typename... ColumnSpec>
  struct table_t : public table_base_t, public ColumnSpec::_alias_t::template _member_t<column_t<Table, ColumnSpec>>...
  {
    using _traits = make_traits<no_value_t, tag::is_raw_table, tag::is_table>;

    using _nodes = detail::type_vector<>;
    using _provided_tables = detail::type_set<Table>;

    static_assert(sizeof...(ColumnSpec), "at least one column required per table");
    using _required_insert_columns =
        typename detail::make_type_set_if<require_insert_t, column_t<Table, ColumnSpec>...>::type;
    using _column_tuple_t = std::tuple<column_t<Table, ColumnSpec>...>;
    template <typename AliasProvider, typename T>
    using _foreign_table_alias_t = table_alias_t<AliasProvider, T, ColumnSpec...>;
    template <typename AliasProvider>
    using _alias_t = table_alias_t<AliasProvider, Table, ColumnSpec...>;

    template <typename T>
    auto join(T t) const -> decltype(::sqlpp::join(std::declval<Table>(), t))
    {
      return ::sqlpp::join(*static_cast<const Table*>(this), t);
    }

    template <typename T>
    auto inner_join(T t) const -> decltype(::sqlpp::inner_join(std::declval<Table>(), t))
    {
      return ::sqlpp::inner_join(*static_cast<const Table*>(this), t);
    }

    template <typename T>
    auto left_outer_join(T t) const -> decltype(::sqlpp::left_outer_join(std::declval<Table>(), t))
    {
      return ::sqlpp::left_outer_join(*static_cast<const Table*>(this), t);
    }

    template <typename T>
    auto right_outer_join(T t) const -> decltype(::sqlpp::right_outer_join(std::declval<Table>(), t))
    {
      return ::sqlpp::right_outer_join(*static_cast<const Table*>(this), t);
    }

    template <typename T>
    auto outer_join(T t) const -> decltype(::sqlpp::outer_join(std::declval<Table>(), t))
    {
      return ::sqlpp::outer_join(*static_cast<const Table*>(this), t);
    }

    template <typename AliasProvider>
    _alias_t<AliasProvider> as(const AliasProvider&) const
    {
      return {*static_cast<const Table*>(this)};
    }

    template <typename T>
    auto cross_join(T t) const -> decltype(::sqlpp::cross_join(std::declval<Table>(), t))
    {
      return ::sqlpp::cross_join(*static_cast<const Table*>(this), t);
    }

    const Table& ref() const
    {
      return *static_cast<const Table*>(this);
    }
  };

  template <typename Context, typename X>
  struct serializer_t<
      Context,
      X,
      typename std::enable_if<std::is_base_of<table_base_t, X>::value and not is_pseudo_table_t<X>::value, void>::type>
  {
    using _serialize_check = consistent_t;
    using T = X;

    static Context& _(const T&, Context& context)
    {
      context << name_of<T>::char_ptr();
      return context;
    }
  };
}

#endif

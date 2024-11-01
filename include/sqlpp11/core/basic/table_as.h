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

#include <sqlpp11/core/basic/enable_join.h>
#include <sqlpp11/core/basic/table_columns.h>
#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/to_sql_string.h>
#include <sqlpp11/core/basic/join.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename TableSpec, typename NameTag>
  struct table_as_t : public TableSpec::_table_columns<table_as_t<TableSpec, NameTag>>,
                         public enable_join<table_as_t<TableSpec, NameTag>>
  {
    using _nodes = detail::type_vector<>;

    static_assert(required_tables_of_t<TableSpec>::empty(), "table aliases must not depend on external tables");

#warning: need to inherit?
    //using _column_tuple_t = std::tuple<column_t<NameTagProvider, ColumnSpec>...>;
  };

  template<typename TableSpec, typename NameTag>
    struct is_table<table_as_t<TableSpec, NameTag>> : public std::true_type{};

  template <typename TableSpec, typename NameTag>
  struct name_tag_of<table_as_t<TableSpec, NameTag>>
  {
    using type = NameTag;
  };

  template <typename TableSpec, typename NameTag>
  struct provided_tables_of<table_as_t<TableSpec, NameTag>>
  {
    using type = sqlpp::detail::type_vector<table_as_t<TableSpec, NameTag>>;
  };

  template <typename TableSpec, typename NameTag>
  struct provided_static_tables_of<table_as_t<TableSpec, NameTag>> : public provided_tables_of<table_as_t<TableSpec, NameTag>>
  {
  };

  template <typename TableSpec, typename NameTag>
  struct provided_optional_tables_of<table_as_t<TableSpec, NameTag>> : public provided_tables_of<table_as_t<TableSpec, NameTag>>
  {
  };

  template <typename Context, typename TableSpec, typename NameTag>
  auto to_sql_string(Context& context, const table_as_t<TableSpec, NameTag>&) -> std::string
  {
    return name_to_sql_string(context, name_tag_of_t<TableSpec>::name) + " AS " +
    name_to_sql_string(context, NameTag::name);
  }
}  // namespace sqlpp

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
  template <typename NameTagProvider, typename TableSpec>
  struct table_as_t : public TableSpec::_table_columns<table_as_t<NameTagProvider, TableSpec>>,
                         public enable_join<table_as_t<NameTagProvider, TableSpec>>
  {
    using _nodes = detail::type_vector<>;
    using _required_ctes = required_ctes_of<TableSpec>;
    using _provided_tables = detail::type_set<NameTagProvider>;

    static_assert(required_tables_of_t<TableSpec>::empty(), "table aliases must not depend on external tables");

#warning: need to inherit?
    //using _column_tuple_t = std::tuple<column_t<NameTagProvider, ColumnSpec>...>;
  };

  template<typename NameTagProvider, typename TableSpec>
    struct is_table<table_as_t<NameTagProvider, TableSpec>> : public std::true_type{};

  template<typename NameTagProvider, typename TableSpec>
    struct name_tag_of<table_as_t<NameTagProvider, TableSpec>> : public name_tag_of<NameTagProvider>{};

  template <typename NameTagProvider, typename TableSpec>
  struct provided_tables_of<table_as_t<NameTagProvider, TableSpec>>
  {
    using type = sqlpp::detail::type_vector<table_as_t<NameTagProvider, TableSpec>>;
  };

  template <typename NameTagProvider, typename TableSpec>
  struct provided_static_tables_of<table_as_t<NameTagProvider, TableSpec>> : public provided_tables_of<table_as_t<NameTagProvider, TableSpec>>
  {
  };

  template <typename NameTagProvider, typename TableSpec>
  struct provided_optional_tables_of<table_as_t<NameTagProvider, TableSpec>> : public provided_tables_of<table_as_t<NameTagProvider, TableSpec>>
  {
  };

  template <typename Context, typename NameTagProvider, typename TableSpec>
  auto to_sql_string(Context& context, const table_as_t<NameTagProvider, TableSpec>&) -> std::string
  {
    return name_to_sql_string(context, name_tag_of_t<TableSpec>::name) + " AS " +
    name_to_sql_string(context, name_tag_of_t<NameTagProvider>::name);
  }
}  // namespace sqlpp

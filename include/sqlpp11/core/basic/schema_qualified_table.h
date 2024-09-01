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

#include <sqlpp11/core/basic/column_fwd.h>
#include <sqlpp11/core/to_sql_string.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/basic/schema.h>
#include <sqlpp11/core/basic/table_as.h>
#include <sqlpp11/core/detail/type_set.h>

#include <utility>

namespace sqlpp
{
  template <typename Table>
  struct schema_qualified_table_t
  {
    using _traits = make_traits<value_type_of_t<Table>>;

    using _nodes = detail::type_vector<>;
    using _required_ctes = detail::type_set<>;
    using _provided_tables = detail::type_set<>;

    schema_qualified_table_t(schema_t schema, Table table) : _schema(std::move(schema)), _table(table)
    {
    }

    template <typename NameTagProvider>
    typename Table::template _foreign_table_as_t<NameTagProvider, schema_qualified_table_t> as(
        const NameTagProvider& /*unused*/) const
    {
      return {*this};
    }

    schema_t _schema;
    Table _table;
  };

  template<typename Table>
    struct is_table<schema_qualified_table_t<Table>> : public std::true_type {};

  template <typename Context, typename Table>
  auto to_sql_string(Context& context, const schema_qualified_table_t<Table>& t) -> std::string
  {
    to_sql_string(context, t._schema);
    context << '.';
    to_sql_string(context, t._table);
    return context;
  }

  template <typename Table>
  auto schema_qualified_table(schema_t schema, Table table) -> schema_qualified_table_t<Table>
  {
    static_assert(required_tables_of_t<Table>::size::value == 0,
                  "schema qualified tables must not depend on other tables");
    static_assert(required_ctes_of<Table>::size::value == 0,
                  "schema qualified tables must not depend on common table expressions");
    static_assert(is_raw_table_t<Table>::value,
                  "table must be a raw table, i.e. not an alias or common table expression");

    return {schema, table};
  }
}  // namespace sqlpp

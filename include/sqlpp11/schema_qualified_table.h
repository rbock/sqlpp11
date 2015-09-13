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

#ifndef SQLPP_SCHEMA_QUALIFIED_TABLE_H
#define SQLPP_SCHEMA_QUALIFIED_TABLE_H

#include <sqlpp11/column_fwd.h>
#include <sqlpp11/interpret.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/schema.h>
#include <sqlpp11/table_alias.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
  template <typename Table>
  struct schema_qualified_table_t
  {
    using _traits = make_traits<value_type_of<Table>, tag::is_table>;

    using _nodes = detail::type_vector<>;
    using _required_ctes = detail::type_set<>;
    using _provided_tables = detail::type_set<>;

    schema_qualified_table_t(schema_t schema, Table table) : _schema(schema), _table(table)
    {
    }

    template <typename AliasProvider>
    typename Table::template _foreign_table_alias_t<AliasProvider, schema_qualified_table_t> as(
        const AliasProvider&) const
    {
      return {*this};
    }

    schema_t _schema;
    Table _table;
  };

  template <typename Context, typename Table>
  struct serializer_t<Context, schema_qualified_table_t<Table>>
  {
    using _serialize_check = serialize_check_of<Context, Table>;
    using T = schema_qualified_table_t<Table>;

    static Context& _(const T& t, Context& context)
    {
      serialize(t._schema, context);
      context << '.';
      serialize(t._table, context);
      return context;
    }
  };

  template <typename Table>
  auto schema_qualified_table(schema_t schema, Table table) -> schema_qualified_table_t<Table>
  {
    static_assert(required_tables_of<Table>::size::value == 0,
                  "schema qualified tables must not depend on other tables");
    static_assert(required_ctes_of<Table>::size::value == 0,
                  "schema qualified tables must not depend on common table expressions");
    static_assert(is_raw_table_t<Table>::value,
                  "table must be a raw table, i.e. not an alias or common table expression");

    return {schema, table};
  }
}

#endif

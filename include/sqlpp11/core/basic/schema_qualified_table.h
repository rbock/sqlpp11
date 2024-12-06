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
  template <typename TableSpec, typename NameTag>
  struct schema_qualified_table_as_t
   : public TableSpec::_table_columns<schema_qualified_table_as_t<TableSpec, NameTag>>, public enable_join<schema_qualified_table_as_t<TableSpec, NameTag>>
  {
    schema_qualified_table_as_t(schema_t schema) : _schema(std::move(schema))
    {
    }

    schema_t _schema;
  };

  template<typename TableSpec, typename NameTag>
    struct is_table<schema_qualified_table_as_t<TableSpec, NameTag>> : public std::true_type {};

  template <typename TableSpec, typename NameTag>
  struct name_tag_of<schema_qualified_table_as_t<TableSpec, NameTag>> {
    using type = NameTag;
  };

  template<typename TableSpec, typename NameTag>
    struct provided_tables_of<schema_qualified_table_as_t<TableSpec, NameTag>> 
    {
      using type = detail::type_set<schema_qualified_table_as_t<TableSpec, NameTag>>;
    };

  template <typename Context, typename TableSpec, typename NameTag>
  auto to_sql_string(Context& context, const schema_qualified_table_as_t<TableSpec, NameTag>& t) -> std::string
  {
    return to_sql_string(context, t._schema) + "." + name_to_sql_string(context, name_tag_of_t<TableSpec>{}) +
           " AS " + name_to_sql_string(context, NameTag{});
  }

  template <typename TableSpec>
  struct schema_qualified_table_t
  {
    schema_qualified_table_t(schema_t schema) : _schema(std::move(schema))
    {
    }

    template <typename NameTagProvider>
    auto as(const NameTagProvider& /*unused*/) const->schema_qualified_table_as_t<TableSpec, name_tag_of_t<NameTagProvider>>
    {
      return {_schema};
    }

    schema_t _schema;
  };

  template <typename TableSpec>
  auto schema_qualified_table(schema_t schema, table_t<TableSpec>) -> schema_qualified_table_t<TableSpec>
  {
    return {schema};
  }
}  // namespace sqlpp

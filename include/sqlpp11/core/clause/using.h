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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/basic/table_ref.h>
#include <sqlpp11/core/to_sql_string.h>
#include <sqlpp11/core/query/statement.h>
#include <sqlpp11/core/clause/clause_base.h>
#include <sqlpp11/core/query/dynamic.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  // USING is used in DELETE statements in
  // * PostgreSQL
  // * MySQL

  // USING
  template <typename Table>
  struct using_t
  {
    using_t(Table table) : _table(table)
    {
    }

    using_t(const using_t&) = default;
    using_t(using_t&&) = default;
    using_t& operator=(const using_t&) = default;
    using_t& operator=(using_t&&) = default;
    ~using_t() = default;

    Table _table;
  };

  template <typename Table>
  struct is_clause<using_t<Table>> : public std::true_type
  {
  };

  template <typename Table>
  struct nodes_of<using_t<Table>>
  {
    using type = detail::type_vector<Table>;
  };

  template <typename Statement, typename Table>
  struct consistency_check<Statement, using_t<Table>>
  {
    using type = consistent_t;
  };

  template<typename Table>
  struct provided_tables_of<using_t<Table>> : public provided_tables_of<Table>
  {
  };

  template<typename  Table>
  struct provided_optional_tables_of<using_t<Table>> : public provided_optional_tables_of<Table>
  {
  };

  // NO USING YET
  struct no_using_t
  {
  };

  template <typename Statement>
  struct clause_base<no_using_t, Statement> : public clause_data<no_using_t, Statement>
  {
    using clause_data<no_using_t, Statement>::clause_data;

    template <typename Table, typename = sqlpp::enable_if_t<is_table<remove_dynamic_t<Table>>::value>>
    auto using_(Table table) const -> decltype(new_statement(*this, using_t<table_ref_t<Table>>{make_table_ref(table)}))
    {
      return new_statement(*this, using_t<table_ref_t<Table>>{make_table_ref(table)});
    }
  };

  template <typename Statement>
  struct consistency_check<Statement, no_using_t>
  {
    using type = consistent_t;
  };

  // Serialization
  template <typename Context>
  inline auto to_sql_string(Context&, const no_using_t&) -> std::string
  {
    return {};
  }

  template <typename Context, typename Table>
  auto to_sql_string(Context& context, const using_t<dynamic_t<Table>>& t) -> std::string
  {
    if (t._table._condition)
    {
      return " USING " + to_sql_string(context, t._table._expr);
    }
    return {};
  }

  template <typename Context, typename Table>
  auto to_sql_string(Context& context, const using_t<Table>& t) -> std::string
  {
    return " USING " + to_sql_string(context, t._table);
  }

  template <typename T>
  auto using_(T t) -> decltype(statement_t<no_using_t>{}.using_(std::move(t)))
  {
    return statement_t<no_using_t>{}.using_(std::move(t));
  }

}  // namespace sqlpp

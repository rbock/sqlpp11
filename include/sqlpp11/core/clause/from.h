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

#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/no_data.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/basic/table_ref.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename Table>
  struct from_t
  {
    Table _table;
  };

  template <typename Table>
  struct is_clause<from_t<Table>> : public std::true_type
  {
  };

  template <typename Statement, typename Table>
  struct consistency_check<Statement, from_t<Table>>
  {
    using type = consistent_t;
  };

  template<typename Table>
  struct nodes_of<from_t<Table>>
  {
    using type = detail::type_vector<Table>;
  };

  template<typename Table>
  struct provided_tables_of<from_t<Table>> : public provided_tables_of<Table>
  {
  };

  template<typename Table>
  struct provided_optional_tables_of<from_t<Table>> : public provided_optional_tables_of<Table>
  {
  };

  struct no_from_t
  {
  };

  template <typename Statement>
  struct clause_base<no_from_t, Statement> : public clause_data<no_from_t, Statement>
  {
    using clause_data<no_from_t, Statement>::clause_data;

    template <typename Table>
    auto from(Table table) const -> decltype(new_statement(*this, from_t<table_ref_t<Table>>{make_table_ref(table)}))
    {
      SQLPP_STATIC_ASSERT(not is_pre_join<Table>::value,
                          "from() join argument is missing condition, please use an explicit on() condition");
      SQLPP_STATIC_ASSERT(is_table<Table>::value, "from() argument has to be a table or join expression");

      return new_statement(*this, from_t<table_ref_t<Table>>{make_table_ref(table)});
    }
  };

  template <typename Statement>
  struct consistency_check<Statement, no_from_t>
  {
    using type = consistent_t;
  };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const no_from_t&) -> std::string
  {
    return "";
  }

  template <typename Context, typename Table>
  auto to_sql_string(Context& context, const from_t<Table>& t) -> std::string
  {
    return " FROM " + to_sql_string(context, t._table);
  }

  template <typename T>
  auto from(T&& t) -> decltype(statement_t<no_from_t>().from(std::forward<T>(t)))
  {
    return statement_t<no_from_t>().from(std::forward<T>(t));
  }

}  // namespace sqlpp

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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/no_data.h>
#include <sqlpp11/core/database/prepared_insert.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename Table>
  struct single_table_t
  {
    single_table_t(Table table) : _table(table)
    {
    }

    single_table_t(const single_table_t&) = default;
    single_table_t(single_table_t&&) = default;
    single_table_t& operator=(const single_table_t&) = default;
    single_table_t& operator=(single_table_t&&) = default;
    ~single_table_t() = default;

    Table _table;
  };

  template <typename Table>
  struct is_clause<single_table_t<Table>> : public std::true_type
  {
  };

  template <typename Statement, typename Table>
  struct consistency_check<Statement, single_table_t<Table>>
  {
    using type = consistent_t;
  };

  template <typename Table>
  struct nodes_of<single_table_t<Table>>
  {
    using type = detail::type_vector<Table>;
  };

  template<typename Table>
  struct provided_tables_of<single_table_t<Table>> : public provided_tables_of<Table>
  {
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_update_table_arg_is_table_t, "argument for update() must be a table");
  template <typename Table>
  struct check_update_table
  {
    using type = static_combined_check_t<static_check_t<is_raw_table<Table>::value, assert_update_table_arg_is_table_t>>;
  };
  template <typename Table>
  using check_update_table_t = typename check_update_table<Table>::type;

  // NO TABLE YET
  struct no_single_table_t
  {
  };

  template <typename Statement>
  struct clause_base<no_single_table_t, Statement> : public clause_data<no_single_table_t, Statement>
  {
    using clause_data<no_single_table_t, Statement>::clause_data;

#warning : reactivate check_update_table_t
    template <typename Table>
    auto single_table(Table table) const -> decltype(new_statement(*this, single_table_t<Table>{table}))
    {
      static_assert(required_tables_of_t<single_table_t<Table>>::empty(),
                    "argument depends on another table in single_table()");

      return new_statement(*this, single_table_t<Table>{table});
    }
  };

  template <typename Statement>
  struct consistency_check<Statement, no_single_table_t>
  {
    using type = consistent_t;
  };

  // Interpreters
  template <typename Context, typename Table>
  auto to_sql_string(Context& context, const single_table_t<Table>& t) -> std::string
  {
    return to_sql_string(context, t._table);
  }

  template <typename T>
  auto single_table(T t) -> decltype(statement_t<no_single_table_t>().single_table(t))
  {
    return statement_t<no_single_table_t>().single_table(std::move(t));
  }
}  // namespace sqlpp

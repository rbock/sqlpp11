#pragma once

/*
 * Copyright (c) 2013 - 2015, Roland Bock
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

#include <sqlpp11/core/default_value.h>
#include <sqlpp11/core/clause/insert_value_list.h>
#include <sqlpp11/core/clause/into.h>
#include <sqlpp11/core/noop.h>
#include <sqlpp11/core/database/parameter_list.h>
#include <sqlpp11/core/database/prepared_insert.h>
#include <sqlpp11/core/query/statement.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  namespace sqlite3
  {
    struct insert_or_replace_name_t
    {
    };
    struct insert_or_ignore_name_t
    {
    };

    template <typename InsertOrAlternative>
    struct insert_or_t
    {
    };
  }

  template <typename Statement, typename InsertOrAlternative>
  struct consistency_check<Statement, sqlite3::insert_or_t<InsertOrAlternative>>
  {
    using type = ::sqlpp::consistent_t;
  };

  template <typename InsertOrAlternative>
  struct is_result_clause<sqlite3::insert_or_t<InsertOrAlternative>> : public std::true_type
  {
  };

  template <typename InsertOrAlternative>
  struct result_methods_of<sqlite3::insert_or_t<InsertOrAlternative>>
  {
    struct type
    {
      template <typename Statement, typename Db>
      auto _run(this Statement&& statement, Db& db)
      {
        return db.insert(std::forward<Statement>(statement));
      }

      template <typename Statement, typename Db>
      auto _prepare(this Statement&& statement, Db& db) -> prepared_insert_t<Db, std::decay_t<Statement>>
      {
        return {{}, db.prepare_insert(std::forward<Statement>(statement))};
      }
    };
  };

  namespace sqlite3 {
    template <typename InsertOrAlternative>
    using blank_insert_or_t =
        statement_t<insert_or_t<InsertOrAlternative>, no_into_t, no_insert_value_list_t>;

    using blank_insert_or_replace_t = blank_insert_or_t<insert_or_replace_name_t>;

    using blank_insert_or_ignore_t = blank_insert_or_t<insert_or_ignore_name_t>;

    inline auto insert_or_replace() -> blank_insert_or_replace_t
    {
      return {blank_insert_or_replace_t()};
    }

    template <typename _Table>
    constexpr auto insert_or_replace_into(_Table table) -> decltype(blank_insert_or_replace_t().into(table))
    {
      return {blank_insert_or_replace_t().into(table)};
    }

    inline auto insert_or_ignore() -> blank_insert_or_ignore_t
    {
      return {blank_insert_or_ignore_t()};
    }

    template <typename _Table>
    constexpr auto insert_or_ignore_into(_Table table) -> decltype(blank_insert_or_ignore_t().into(table))
    {
      return {blank_insert_or_ignore_t().into(table)};
    }

    inline auto to_sql_string(sqlite3::context_t& , const sqlite3::insert_or_t<sqlite3::insert_or_replace_name_t>&) -> std::string
    {
      return "INSERT OR REPLACE ";
    }

    inline auto to_sql_string(sqlite3::context_t& , const sqlite3::insert_or_t<sqlite3::insert_or_ignore_name_t>&) -> std::string
    {
      return "INSERT OR IGNORE ";
    }
  }  // namespace sqlite3
}

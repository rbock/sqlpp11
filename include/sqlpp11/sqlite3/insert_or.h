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

#include <sqlpp11/default_value.h>
#include <sqlpp11/insert_value_list.h>
#include <sqlpp11/into.h>
#include <sqlpp11/noop.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_insert.h>
#include <sqlpp11/statement.h>
#include <sqlpp11/type_traits.h>

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
    struct insert_or_t : public statement_name_t<InsertOrAlternative>
    {
      using _traits = make_traits<no_value_t, tag::is_return_value>;
      struct _alias_t
      {
      };

      template <typename Statement>
      struct _result_methods_t
      {
        using _statement_t = Statement;

        const _statement_t& _get_statement() const
        {
          return static_cast<const _statement_t&>(*this);
        }

        template <typename Db>
        auto _run(Db& db) const -> decltype(db.insert(this->_get_statement()))
        {
          return db.insert(_get_statement());
        }

        template <typename Db>
        auto _prepare(Db& db) const -> prepared_insert_t<Db, _statement_t>
        {
          return {{}, db.prepare_insert(_get_statement())};
        }
      };
    };

    template <typename Database, typename InsertOrAlternative>
    using blank_insert_or_t =
        statement_t<Database, insert_or_t<InsertOrAlternative>, no_into_t, no_insert_value_list_t>;

    template <typename Database>
    using blank_insert_or_replace_t = blank_insert_or_t<Database, insert_or_replace_name_t>;

    template <typename Database>
    using blank_insert_or_ignore_t = blank_insert_or_t<Database, insert_or_ignore_name_t>;

    inline auto insert_or_replace() -> blank_insert_or_replace_t<void>
    {
      return {blank_insert_or_replace_t<void>()};
    }

    template <typename Table>
    constexpr auto insert_or_replace_into(Table table) -> decltype(blank_insert_or_replace_t<void>().into(table))
    {
      return {blank_insert_or_replace_t<void>().into(table)};
    }

    template <typename Database>
    constexpr auto dynamic_insert_or_replace(const Database&) -> decltype(blank_insert_or_replace_t<Database>())
    {
      return {blank_insert_or_replace_t<Database>()};
    }

    template <typename Database, typename Table>
    constexpr auto dynamic_insert_or_replace_into(const Database&, Table table)
        -> decltype(blank_insert_or_replace_t<Database>().into(table))
    {
      return {blank_insert_or_replace_t<Database>().into(table)};
    }

    inline auto insert_or_ignore() -> blank_insert_or_ignore_t<void>
    {
      return {blank_insert_or_ignore_t<void>()};
    }

    template <typename Table>
    constexpr auto insert_or_ignore_into(Table table) -> decltype(blank_insert_or_ignore_t<void>().into(table))
    {
      return {blank_insert_or_ignore_t<void>().into(table)};
    }

    template <typename Database>
    constexpr auto dynamic_insert_or_ignore(const Database&) -> decltype(blank_insert_or_ignore_t<Database>())
    {
      return {blank_insert_or_ignore_t<Database>()};
    }

    template <typename Database, typename Table>
    constexpr auto dynamic_insert_or_ignore_into(const Database&, Table table)
        -> decltype(blank_insert_or_ignore_t<Database>().into(table))
    {
      return {blank_insert_or_ignore_t<Database>().into(table)};
    }

    inline sqlite3::context_t& serialize(const sqlite3::insert_or_replace_name_t&, sqlite3::context_t& context)
    {
      context << "INSERT OR REPLACE ";
      return context;
    }

    inline sqlite3::context_t& serialize(const sqlite3::insert_or_ignore_name_t&, sqlite3::context_t& context)
    {
      context << "INSERT OR IGNORE ";
      return context;
    }
  }
}

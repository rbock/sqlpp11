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

#include <sqlpp11/core/query/statement.h>
#include <sqlpp11/core/database/connection.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/database/prepared_insert.h>
#include <sqlpp11/core/default_value.h>
#include <sqlpp11/core/clause/into.h>
#include <sqlpp11/core/clause/insert_value_list.h>

namespace sqlpp
{
  struct insert_t
  {
    template <typename Statement>
    struct _result_methods_t
    {
      using _statement_t = Statement;

      const _statement_t& _get_statement() const
      {
        return static_cast<const _statement_t&>(*this);
      }

      // Execute
      template <typename Db, typename Composite>
      auto _run(Db& db, const Composite& composite) const -> decltype(db.insert(composite))
      {
        return db.insert(composite);
      }

      template <typename Db>
      auto _run(Db& db) const -> decltype(db.insert(this->_get_statement()))
      {
        return db.insert(_get_statement());
      }

      // Prepare
      template <typename Db, typename Composite>
      auto _prepare(Db& db, const Composite& composite) const -> prepared_insert_t<Db, Composite>
      {
        return {{}, db.prepare_insert(composite)};
      }

      template <typename Db>
      auto _prepare(Db& db) const -> prepared_insert_t<Db, _statement_t>
      {
        return {{}, db.prepare_insert(_get_statement())};
      }
    };
  };

  template <>
  struct is_clause<insert_t> : public std::true_type
  {
  };

  template<typename Statement>
    struct consistency_check<Statement, insert_t> { using type = consistent_t; };

  template<>
    struct is_result_clause<insert_t> : public std::true_type {};

  template <typename Context>
  auto to_sql_string(Context& , const insert_t&) -> std::string
  {
    return "INSERT";
  }

  using blank_insert_t = statement_t<insert_t, no_into_t, no_insert_value_list_t>;

  inline auto insert() -> blank_insert_t
  {
    return {};
  }

  template <typename Table>
  constexpr auto insert_into(Table table) -> decltype(blank_insert_t().into(table))
  {
    return {blank_insert_t().into(table)};
  }

}  // namespace sqlpp

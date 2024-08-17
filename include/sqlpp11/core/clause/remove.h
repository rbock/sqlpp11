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
#include <sqlpp11/core/database/prepared_remove.h>
#include <sqlpp11/core/noop.h>
#include <sqlpp11/core/clause/from.h>
#include <sqlpp11/core/clause/using.h>
#include <sqlpp11/core/clause/where.h>

namespace sqlpp
{
  struct remove_name_t
  {
  };
  struct remove_t : public statement_name_t<remove_name_t>
  {
    using _traits = make_traits<no_value_t>;
    struct _sqlpp_name_tag
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

      // Execute
      template <typename Db, typename Composite>
      auto _run(Db& db, const Composite& composite) const -> decltype(db.remove(composite))
      {
        return db.remove(composite);
      }

      template <typename Db>
      auto _run(Db& db) const -> decltype(db.remove(this->_get_statement()))
      {
        return db.remove(_get_statement());
      }

      // Prepare
      template <typename Db, typename Composite>
      auto _prepare(Db& db, const Composite& composite) const -> prepared_remove_t<Db, Composite>
      {
        return {{}, db.prepare_remove(composite)};
      }

      template <typename Db>
      auto _prepare(Db& db) const -> prepared_remove_t<Db, _statement_t>
      {
        return {{}, db.prepare_remove(_get_statement())};
      }
    };
  };

  template<>
    struct is_result_clause<remove_t> : public std::true_type {};

  template <typename Context>
  auto to_sql_string(Context& , const remove_name_t&) -> std::string
  {
    return "DELETE";
  }

  using blank_remove_t = statement_t<remove_t, no_from_t, no_using_t, no_where_t<true>>;

  inline auto remove() -> blank_remove_t
  {
    return {};
  }

  template <typename Table>
  auto remove_from(Table table) -> decltype(blank_remove_t().from(table))
  {
    return {blank_remove_t().from(table)};
  }

}  // namespace sqlpp

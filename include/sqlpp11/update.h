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

#ifndef SQLPP_UPDATE_H
#define SQLPP_UPDATE_H

#include <sqlpp11/statement.h>
#include <sqlpp11/connection.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_update.h>
#include <sqlpp11/single_table.h>
#include <sqlpp11/update_list.h>
#include <sqlpp11/noop.h>
#include <sqlpp11/where.h>

namespace sqlpp
{
  struct update_name_t
  {
  };

  struct update_t : public statement_name_t<update_name_t>
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

      // Execute
      template <typename Db, typename Composite>
      auto _run(Db& db, const Composite& composite) const -> decltype(db.update(composite))
      {
        return db.update(composite);
      }

      template <typename Db>
      auto _run(Db& db) const -> decltype(db.update(this->_get_statement()))
      {
        return db.update(_get_statement());
      }

      // Prepare
      template <typename Db, typename Composite>
      auto _prepare(Db& db, const Composite& composite) const -> prepared_update_t<Db, Composite>
      {
        return {{}, db.prepare_update(composite)};
      }

      template <typename Db>
      auto _prepare(Db& db) const -> prepared_update_t<Db, _statement_t>
      {
        return {{}, db.prepare_update(_get_statement())};
      }
    };
  };

  template <typename Context>
  struct serializer_t<Context, update_name_t>
  {
    using _serialize_check = consistent_t;
    using T = update_name_t;

    static Context& _(const T&, Context& context)
    {
      context << "UPDATE ";

      return context;
    }
  };

  template <typename Database>
  using blank_update_t = statement_t<Database, update_t, no_single_table_t, no_update_list_t, no_where_t<true>>;

  template <typename Table>
  constexpr auto update(Table table) -> decltype(blank_update_t<void>().single_table(table))
  {
    return {blank_update_t<void>().single_table(table)};
  }

  template <typename Database, typename Table>
  constexpr auto dynamic_update(const Database&, Table table)
      -> decltype(blank_update_t<Database>().single_table(table))
  {
    static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
    return {blank_update_t<Database>().single_table(table)};
  }
}

#endif

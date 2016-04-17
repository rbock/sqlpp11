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

#ifndef SQLPP_SELECT_H
#define SQLPP_SELECT_H

#include <sqlpp11/statement.h>

#include <sqlpp11/noop.h>
#include <sqlpp11/connection.h>
#include <sqlpp11/with.h>
#include <sqlpp11/select_flag_list.h>
#include <sqlpp11/select_column_list.h>
#include <sqlpp11/from.h>
#include <sqlpp11/where.h>
#include <sqlpp11/group_by.h>
#include <sqlpp11/having.h>
#include <sqlpp11/order_by.h>
#include <sqlpp11/limit.h>
#include <sqlpp11/offset.h>
#include <sqlpp11/union.h>
#include <sqlpp11/expression.h>
#include <sqlpp11/wrong.h>

namespace sqlpp
{
  struct select_name_t
  {
  };

  struct select_t : public statement_name_t<select_name_t, tag::is_select>
  {
  };

  template <typename Context>
  struct serializer_t<Context, select_name_t>
  {
    using _serialize_check = consistent_t;
    using T = select_name_t;

    static Context& _(const T&, Context& context)
    {
      context << "SELECT ";

      return context;
    }
  };

  template <typename Database>
  using blank_select_t = statement_t<Database,
                                     no_with_t,
                                     select_t,
                                     no_select_flag_list_t,
                                     no_select_column_list_t,
                                     no_from_t,
                                     no_where_t<true>,
                                     no_group_by_t,
                                     no_having_t,
                                     no_order_by_t,
                                     no_limit_t,
                                     no_offset_t,
                                     no_union_t>;

  inline blank_select_t<void> select()  // FIXME: These should be constexpr
  {
    return {};
  }

  template <typename... Columns>
  auto select(Columns... columns) -> decltype(blank_select_t<void>().columns(columns...))
  {
    return blank_select_t<void>().columns(columns...);
  }

  template <typename Database>
  blank_select_t<Database> dynamic_select(const Database&)
  {
    static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
    return {};
  }

  template <typename Database, typename... Columns>
  auto dynamic_select(const Database&, Columns... columns) -> decltype(blank_select_t<Database>().columns(columns...))
  {
    static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
    return blank_select_t<Database>().columns(columns...);
  }
}
#endif

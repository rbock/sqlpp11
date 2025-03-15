#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp23/core/query/statement.h>

#include <sqlpp23/core/clause/for_update.h>
#include <sqlpp23/core/clause/from.h>
#include <sqlpp23/core/clause/group_by.h>
#include <sqlpp23/core/clause/having.h>
#include <sqlpp23/core/clause/limit.h>
#include <sqlpp23/core/clause/offset.h>
#include <sqlpp23/core/clause/order_by.h>
#include <sqlpp23/core/clause/select_column_list.h>
#include <sqlpp23/core/clause/select_flag_list.h>
#include <sqlpp23/core/clause/union.h>
#include <sqlpp23/core/clause/where.h>
#include <sqlpp23/core/clause/with.h>
#include <sqlpp23/core/database/connection.h>
#include <sqlpp23/core/wrong.h>

namespace sqlpp {
struct select_t {
};

  template <typename Context>
  auto to_sql_string(Context&, const select_t&) -> std::string {
    return "SELECT ";
  }

template <> struct is_clause<select_t> : public std::true_type {};

template <typename Statement> struct consistency_check<Statement, select_t> {
  using type = consistent_t;
};

using blank_select_t =
    statement_t<no_with_t, select_t, no_select_flag_list_t,
                no_select_column_list_t, no_from_t, no_where_t, no_group_by_t,
                no_having_t, no_order_by_t, no_limit_t, no_offset_t, no_union_t,
                no_for_update_t>;

inline constexpr blank_select_t select()
{
  return {};
}

template <typename... Columns>
  requires(sizeof...(Columns) > 0)
auto select(Columns... columns) {
  return blank_select_t().columns(columns...);
}

} // namespace sqlpp

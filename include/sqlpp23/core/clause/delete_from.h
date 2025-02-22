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

#include <sqlpp23/core/clause/single_table.h>
#include <sqlpp23/core/clause/using.h>
#include <sqlpp23/core/clause/where.h>
#include <sqlpp23/core/database/connection.h>
#include <sqlpp23/core/database/prepared_delete.h>
#include <sqlpp23/core/query/statement.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
struct delete_t {};

template <> struct is_clause<delete_t> : public std::true_type {};

template <typename Statement> struct consistency_check<Statement, delete_t> {
  using type = consistent_t;
};

template <> struct is_result_clause<delete_t> : public std::true_type {};

struct delete_result_methods_t {
  // Execute
  template <typename Statement, typename Db>
  auto _run(this Statement &&statement, Db &db)
      -> decltype(db.remove(std::forward<Statement>(statement))) {
    return db.remove(std::forward<Statement>(statement));
  }

  // Prepare
  template <typename Statement, typename Db>
  auto _prepare(this Statement &&statement, Db &db)
      -> prepared_delete_t<Db, std::decay_t<Statement>> {
    return {{}, db.prepare_remove(std::forward<Statement>(statement))};
  }
};

template <> struct result_methods_of<delete_t> {
  using type = delete_result_methods_t;
};

template <typename Context>
auto to_sql_string(Context &, const delete_t &) -> std::string {
  return "DELETE FROM ";
}

using blank_delete_t =
    statement_t<delete_t, no_single_table_t, no_using_t, no_where_t>;

inline auto delete_from() -> blank_delete_t { return {}; }

template <typename _Table>
auto delete_from(_Table table)
    -> decltype(blank_delete_t().single_table(table)) {
  return {blank_delete_t().single_table(table)};
}

} // namespace sqlpp

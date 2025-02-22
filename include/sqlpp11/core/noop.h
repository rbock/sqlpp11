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

#include <sqlpp11/core/database/prepared_execute.h>
#include <sqlpp11/core/to_sql_string.h>
#include <type_traits>

namespace sqlpp {
struct noop {
  // This represents a void result.
  struct _result_methods_t {
    // Execute
    template <typename Statement, typename Db>
    auto _run(this Statement &&statement, Db &db) {
      return db.execute(std::forward<Statement>(statement));
    }

    // Prepare
    template <typename Statement, typename Db>
    auto _prepare(this Statement &&statement, Db &db)
        -> prepared_execute_t<Db, std::decay_t<Statement>> {
      return {{}, db.prepare_execute(std::forward<Statement>(statement))};
    }
  };
};

template <> struct result_methods_of<noop> {
  struct type {
    // Execute
    template <typename Statement, typename Db>
    auto _run(this Statement &&statement, Db &db) {
      return db.execute(std::forward<Statement>(statement));
    }

    // Prepare
    template <typename Statement, typename Db>
    auto _prepare(this Statement &&statement, Db &db)
        -> prepared_execute_t<Db, std::decay_t<Statement>> {
      return {{}, db.prepare_execute(std::forward<Statement>(statement))};
    }
  };
};

template <typename Context>
auto to_sql_string(Context &, const noop &) -> std::string {
  return {};
}

template <typename T> struct is_noop : std::is_same<T, noop> {};
} // namespace sqlpp

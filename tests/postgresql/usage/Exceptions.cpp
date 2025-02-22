/*
 * Copyright (c) 2014-2015, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
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

#include <sqlpp11/core/basic/verbatim.h>
#include <sqlpp11/core/database/exception.h>
#include <sqlpp11/core/name/create_name_tag.h>
#include <sqlpp11/postgresql/postgresql.h>

#include "assertThrow.h"

#include "make_test_connection.h"
#include <sqlpp11/tests/postgresql/tables.h>

namespace sql = sqlpp::postgresql;
int Exceptions(int, char *[]) {
  {
    // broken_connection exception on bad config
    auto config = std::make_shared<sql::connection_config>();
    config->host = "non-existing-host";
    assert_throw(sql::connection db(config), sql::broken_connection);
  }

  test::TabExcept tab;
  sql::connection db = sql::make_test_connection();

  try {
    test::createTabExcept(db);
    assert_throw(db(insert_into(tab).set(
                     tab.intSmallNU = std::numeric_limits<int16_t>::max() + 1)),
                 sql::data_exception);
    assert_throw(db(insert_into(tab).set(tab.textShortN = "123456")),
                 sql::check_violation);
    db(insert_into(tab).set(tab.intSmallNU = 5));
    assert_throw(db(insert_into(tab).set(tab.intSmallNU = 5)),
                 sql::integrity_constraint_violation);
    assert_throw(db.last_insert_id("tabfoo", "no_such_column"),
                 sqlpp::postgresql::undefined_table);

    try {
      // Cause specific error
      db.execute(R"(create or replace function cause_error() returns int as $$
                      begin
                        raise exception 'User error' USING ERRCODE='ZX123';
                      end;
                    $$ language plpgsql
                    )");

      db.execute("select cause_error();");
    } catch (const sql::sql_user_error &e) {
      std::cout << "Caught expected error. Code: " << e.code() << '\n';
      if (e.code() != "ZX123")
        throw std::runtime_error("unexpected error code");
    }
  } catch (const sql::failure &e) {
    std::cout << e.what();
    return 1;
  }

  return 0;
}

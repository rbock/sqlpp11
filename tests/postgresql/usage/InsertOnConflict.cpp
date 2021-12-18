/**
 * Copyright © 2014-2019, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
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

#include <iostream>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include "TabBar.h"
#include "TabFoo.h"
#include "make_test_connection.h"

namespace sql = sqlpp::postgresql;

int InsertOnConflict(int, char*[])
{
  model::TabFoo foo = {};

  sql::connection db = sql::make_test_connection();

  db.execute(R"(DROP TABLE IF EXISTS tabfoo;)");
  db.execute(R"(CREATE TABLE tabfoo
             (
               alpha bigserial PRIMARY KEY NOT NULL,
               beta smallint,
               gamma text,
               c_bool boolean,
               c_timepoint timestamp with time zone,
               c_day date
             ))");

#warning: Need to add serialization tests
  // Test on conflict
  db(sql::insert_into(foo).default_values().on_conflict().do_nothing());

  // Test on conflict (with conflict target)
  db(sql::insert_into(foo).default_values().on_conflict(foo.alpha).do_nothing());

  // Conflict target
  db(sql::insert_into(foo).default_values().on_conflict(foo.alpha).do_update(
      foo.beta = 5, foo.gamma = "test bla", foo.c_bool = true));

  // With where statement
  for (const auto& row : db(sql::insert_into(foo)
                     .default_values()
                     .on_conflict(foo.alpha)
                     .do_update(foo.beta = 5, foo.gamma = "test bla", foo.c_bool = true)
                     .where(foo.beta == 2)
                     .returning(foo.gamma)))
  {
    std::cout << row.gamma << std::endl;
  }

  // Returning
  for (const auto& row : db(sql::insert_into(foo)
                     .default_values()
                     .on_conflict(foo.alpha)
                     .do_update(foo.beta = 5, foo.gamma = "test bla", foo.c_bool = true)
                     .returning(foo.beta)))
  {
    std::cout << row.beta << std::endl;
  }

  return 0;
}

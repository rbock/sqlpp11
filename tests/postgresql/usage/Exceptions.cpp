/*
 * Copyright (c) 2014-2015, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/exception.h>
#include <sqlpp11/custom_query.h>
#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/verbatim.h>
#include <sqlpp11/alias_provider.h>

#include "assertThrow.h"

#include "TabBar.h"
#include "TabFoo.h"
#include "make_test_connection.h"

namespace sql = sqlpp::postgresql;
int Exceptions(int, char*[])
{
  {
    // broken_connection exception on bad config
    auto config = std::make_shared<sql::connection_config>();
    config->host = "non-existing-host";
    assert_throw(sql::connection db(config), sql::broken_connection);
  }

  model::TabFoo foo = {};
  sql::connection db = sql::make_test_connection();

  try
  {
    db.execute(R"(DROP TABLE IF EXISTS tabfoo;)");
    db.execute(R"(CREATE TABLE tabfoo
                   (
                   alpha bigserial NOT NULL,
                   beta smallint UNIQUE,
                   gamma text CHECK( length(gamma) < 5 ),
                   c_bool boolean,
                   c_timepoint timestamp with time zone DEFAULT now(),
                   c_day date
                   ))");
    db.execute(R"(create or replace function cause_error() returns int as $$
                    begin
                      raise exception 'User error' USING ERRCODE='ZX123'; 
                    end;
                  $$ language plpgsql
                  )");

    assert_throw(db(insert_into(foo).set(foo.beta = std::numeric_limits<int16_t>::max() + 1)), sql::data_exception);
    assert_throw(db(insert_into(foo).set(foo.gamma = "123456")), sql::check_violation);
    db(insert_into(foo).set(foo.beta = 5));
    assert_throw(db(insert_into(foo).set(foo.beta = 5)), sql::integrity_constraint_violation);
    assert_throw(db.last_insert_id("tabfoo", "no_such_column"), sqlpp::postgresql::undefined_table);    
    
    try 
    {
      db.execute("select cause_error();");
    } 
    catch( const sql::sql_user_error& e)
    {
      std::cout << e.code();
      assert( e.code() == "ZX123");
    }

  }
  catch (const sql::failure& e)
  {
    std::cout << e.what();
    return 1;
  }

  return 0;
}

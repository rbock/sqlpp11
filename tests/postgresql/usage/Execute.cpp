/*
 * Copyright (c) 2024, Roland Bock
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

#include "Sample.h"
#include "make_test_connection.h"

#include <sqlpp11/alias_provider.h>
#include <sqlpp11/parameter.h>
#include <sqlpp11/update.h>
#include <sqlpp11/verbatim.h>
#include <sqlpp11/postgresql/connection.h>

#include <iostream>

namespace sql = sqlpp::postgresql;

int Execute(int, char*[])
{
  sql::connection db = sql::make_test_connection();

  // execute supports single statements.
  db.execute(R"(SELECT 1)");

  // execute throws an exception if multiple statements are passed in the string.
  try
  {
    db.execute(R"(SELECT 1; SELECT 2)");
  }
  catch (const sqlpp::exception& e)
  {
    const auto message = std::string(e.what());
    if (message.find("Cannot execute multi-statements") == message.npos)
    {
      std::cerr << "Unexpected exception for multi-statement: " << message;
      return 1;
    }
  }

  // execute supports running a prepared statement
  const auto tab = test::TabBar{};

  db.execute(R"(CREATE TABLE tab_bar (
                	alpha bigint AUTO_INCREMENT,
                	beta varchar(255) NULL DEFAULT "",
                	gamma bool NOT NULL,
                	delta int);
              )");

  auto u = sqlpp::update(tab)
              .set(tab.delta = sqlpp::parameter(tab.delta))
              .where(sqlpp::verbatim<sqlpp::unsigned_integral>("ROWID")
                    == sqlpp::parameter(sqlpp::unsigned_integral(), sqlpp::alias::i));

  auto u_stmnt = db.prepare(u);
  u_stmnt.params.delta = 42;
  u_stmnt.params.i = 69u;

  db.execute(u_stmnt);

  return 0;
}

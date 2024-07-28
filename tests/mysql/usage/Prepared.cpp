/*
 * Copyright (c) 2013 - 2016, Roland Bock
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

#include "make_test_connection.h"
#include "Tables.h"
#include <cassert>
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/clause/insert.h>
#include <sqlpp11/mysql/connection.h>
#include <sqlpp11/clause/remove.h>
#include <sqlpp11/clause/select.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/clause/update.h>

#include <iostream>
#include <vector>

const auto library_raii = sqlpp::mysql::scoped_library_initializer_t{0, nullptr, nullptr};

namespace sql = sqlpp::mysql;
const auto tab = test::TabSample{};

void testPreparedStatementResult(sql::connection& db)
{
  auto preparedInsert = db.prepare(insert_into(tab).set(tab.textN = parameter(tab.textN)));
  preparedInsert.params.textN = sqlpp::null;
  db(preparedInsert);
  preparedInsert.params.textN = "17";
  db(preparedInsert);
  preparedInsert.params.textN = sqlpp::value_or_null<sqlpp::text>(sqlpp::null);
  db(preparedInsert);
  preparedInsert.params.textN = sqlpp::value_or_null("17");
  db(preparedInsert);

  auto preparedSelectAll = db.prepare(sqlpp::select(count(tab.intN)).from(tab).unconditionally());
  auto preparedUpdateAll = db.prepare(sqlpp::update(tab).set(tab.boolN = false).unconditionally());

  {
    // explicit result scope
    // if results are released update should execute without exception
    auto result = db(preparedSelectAll);
    std::ignore = result.front().count;
  }

  db(preparedUpdateAll);
}

int Prepared(int, char*[])
{
  sql::global_library_init();
  try
  {
    auto db = sql::make_test_connection();
    test::createTabSample(db);

    testPreparedStatementResult(db);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

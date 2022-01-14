/*
 * Copyright (c) 2013 - 2015, Roland Bock
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

#include <cassert>
#include <iostream>
#include <vector>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include "TabFoo.h"
#include "make_test_connection.h"

namespace sql = sqlpp::postgresql;
model::TabFoo tab = {};

void testSelectAll(sql::connection& db, int expectedRowCount)
{
  std::cerr << "--------------------------------------" << std::endl;
  int i = 0;
  for (const auto& row : db(sqlpp::select(all_of(tab)).from(tab).unconditionally()))
  {
    ++i;
    std::cerr << ">>> row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma
              << std::endl;
    assert(i == row.alpha);
  };
  assert(i == expectedRowCount);

  auto preparedSelectAll = db.prepare(sqlpp::select(all_of(tab)).from(tab).unconditionally());
  i = 0;
  for (const auto& row : db(preparedSelectAll))
  {
    ++i;
    std::cerr << ">>> row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma
              << std::endl;
    assert(i == row.alpha);
  };
  assert(i == expectedRowCount);
  std::cerr << "--------------------------------------" << std::endl;
}

int Select(int, char*[])
{
  sql::connection db = sql::make_test_connection();

  db.execute(R"(DROP TABLE IF EXISTS tabfoo;)");
  db.execute(R"(CREATE TABLE tabfoo
             (
               alpha bigserial NOT NULL,
               beta smallint,
               gamma text,
               c_bool boolean,
               c_timepoint timestamp with time zone,
               c_day date
             ))");

  testSelectAll(db, 0);
  db(insert_into(tab).default_values());
  testSelectAll(db, 1);
  db(insert_into(tab).set(tab.c_bool = true, tab.gamma = "cheesecake"));
  testSelectAll(db, 2);
  db(insert_into(tab).set(tab.c_bool = true, tab.gamma = "cheesecake"));
  testSelectAll(db, 3);

  // Test size functionality
  const auto test_size = db(select(all_of(tab)).from(tab).unconditionally());
  assert(test_size.size() == 3);

  // test functions and operators
  db(select(all_of(tab)).from(tab).where(tab.alpha.is_null()));
  db(select(all_of(tab)).from(tab).where(tab.alpha.is_not_null()));
  db(select(all_of(tab)).from(tab).where(tab.alpha.in(1, 2, 3)));
  db(select(all_of(tab)).from(tab).where(tab.alpha.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));
  db(select(all_of(tab)).from(tab).where(tab.alpha.not_in(1, 2, 3)));
  db(select(all_of(tab)).from(tab).where(tab.alpha.not_in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));
  db(select(count(tab.alpha)).from(tab).unconditionally());
  db(select(avg(tab.alpha)).from(tab).unconditionally());
  db(select(max(tab.alpha)).from(tab).unconditionally());
  db(select(min(tab.alpha)).from(tab).unconditionally());
  db(select(exists(select(tab.alpha).from(tab).where(tab.alpha > 7))).from(tab).unconditionally());
  db(select(all_of(tab)).from(tab).where(tab.alpha == any(select(tab.alpha).from(tab).where(tab.alpha < 3))));
  db(select(all_of(tab)).from(tab).where(tab.alpha == some(select(tab.alpha).from(tab).where(tab.alpha < 3))));
  db(select(all_of(tab)).from(tab).where(tab.alpha + tab.alpha > 3));
  db(select(all_of(tab)).from(tab).where((tab.gamma + tab.gamma) == ""));
  db(select(all_of(tab)).from(tab).where((tab.gamma + tab.gamma).like("%'\"%")));

  // test boolean value
  db(insert_into(tab).set(tab.c_bool = true, tab.gamma = "asdf"));
  db(insert_into(tab).set(tab.c_bool = false, tab.gamma = "asdfg"));

  assert(db(select(tab.c_bool).from(tab).where(tab.gamma == "asdf")).front().c_bool);
  assert(not db(select(tab.c_bool).from(tab).where(tab.gamma == "asdfg")).front().c_bool);
  assert(not db(select(tab.c_bool).from(tab).where(tab.alpha == 1)).front().c_bool);

  // test

  // update
  db(update(tab).set(tab.c_bool = false).where(tab.alpha.in(1)));
  db(update(tab).set(tab.c_bool = false).where(tab.alpha.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));

  // remove
  db(remove_from(tab).where(tab.alpha == tab.alpha + 3));

  auto result1 = db(select(all_of(tab)).from(tab).unconditionally());
  std::cerr << "Accessing a field directly from the result (using the current row): " << result1.begin()->alpha
            << std::endl;
  std::cerr << "Can do that again, no problem: " << result1.begin()->alpha << std::endl;

  auto tx = start_transaction(db);
  auto result2 = db(select(all_of(tab), select(max(tab.alpha)).from(tab)).from(tab).unconditionally());
  if (const auto& row = *result2.begin())
  {
    auto a = row.alpha;
    auto m = row.max;
    std::cerr << "-----------------------------" << a << ", " << m << std::endl;
  }
  tx.commit();

  return 0;
}

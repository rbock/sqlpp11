/*
 * Copyright (c) 2013 - 2015, Roland Bock
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

#include <cassert>
#include <iostream>
#include <vector>

#include <sqlpp23/postgresql/postgresql.h>
#include <sqlpp23/sqlpp23.h>

#include "make_test_connection.h"
#include "sqlpp23/tests/core/result_helpers.h"
#include <sqlpp23/tests/postgresql/tables.h>

namespace sql = sqlpp::postgresql;
test::TabFoo tab = {};

void testSelectAll(sql::connection &db, int expectedRowCount) {
  std::cerr << "--------------------------------------" << std::endl;
  int i = 0;
  for (const auto &row : db(sqlpp::select(all_of(tab)).from(tab).where(true))) {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", row.intN: " << row.intN
              << ", row.textNnD: " << row.textNnD << std::endl;
    assert(i == row.id);
  };
  assert(i == expectedRowCount);

  auto preparedSelectAll =
      db.prepare(sqlpp::select(all_of(tab)).from(tab).where(true));
  i = 0;
  for (const auto &row : db(preparedSelectAll)) {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", row.intN: " << row.intN
              << ", row.textNnD: " << row.textNnD << std::endl;
    assert(i == row.id);
  };
  assert(i == expectedRowCount);
  std::cerr << "--------------------------------------" << std::endl;
}

namespace {
SQLPP_CREATE_NAME_TAG(something);
}

int Select(int, char *[]) {
  sql::connection db = sql::make_test_connection();

  test::createTabFoo(db);

  testSelectAll(db, 0);
  db(insert_into(tab).default_values());
  testSelectAll(db, 1);
  db(insert_into(tab).set(tab.boolN = true, tab.textNnD = "cheesecake"));
  testSelectAll(db, 2);
  db(insert_into(tab).set(tab.boolN = true, tab.textNnD = "cheesecake"));
  testSelectAll(db, 3);

  // Test size functionality
  const auto test_size = db(select(all_of(tab)).from(tab).where(true));
  assert(test_size.size() == 3);

  // test functions and operators
  db(select(all_of(tab)).from(tab).where(tab.id.is_null()));
  db(select(all_of(tab)).from(tab).where(tab.id.is_not_null()));
  db(select(all_of(tab)).from(tab).where(tab.id.in(1, 2, 3)));
  db(select(all_of(tab))
         .from(tab)
         .where(tab.id.in(std::vector<int>{1, 2, 3, 4})));
  db(select(all_of(tab)).from(tab).where(tab.id.not_in(1, 2, 3)));
  db(select(all_of(tab))
         .from(tab)
         .where(tab.id.not_in(std::vector<int>{1, 2, 3, 4})));
  db(select(count(tab.id).as(something)).from(tab).where(true));
  db(select(avg(tab.id).as(something)).from(tab).where(true));
  db(select(max(tab.id).as(something)).from(tab).where(true));
  db(select(min(tab.id).as(something)).from(tab).where(true));
  db(select(exists(select(tab.id).from(tab).where(tab.id > 7)).as(something))
         .from(tab)
         .where(true));
  db(select(all_of(tab))
         .from(tab)
         .where(tab.id == any(select(tab.id).from(tab).where(tab.id < 3))));
  db(select(all_of(tab)).from(tab).where(tab.id + tab.id > 3));
  db(select(all_of(tab)).from(tab).where((tab.textNnD + tab.textNnD) == ""));
  db(select(all_of(tab))
         .from(tab)
         .where((tab.textNnD + tab.textNnD).like("%'\"%")));

  // test boolean value
  db(insert_into(tab).set(tab.boolN = true, tab.textNnD = "asdf"));
  db(insert_into(tab).set(tab.boolN = false, tab.textNnD = "asdfg"));

  assert(db(select(tab.boolN).from(tab).where(tab.textNnD == "asdf"))
             .front()
             .boolN);
  assert(not db(select(tab.boolN).from(tab).where(tab.textNnD == "asdfg"))
                 .front()
                 .boolN.value());
  assert(not db(select(tab.boolN).from(tab).where(tab.id == 1))
                 .front()
                 .boolN.has_value());

  // test

  // update
  db(update(tab).set(tab.boolN = false).where(tab.id.in(1)));
  db(update(tab)
         .set(tab.boolN = false)
         .where(tab.id.in(std::vector<int>{1, 2, 3, 4})));

  // delete
  db(delete_from(tab).where(tab.id == tab.id + 3));

  auto result1 = db(select(all_of(tab)).from(tab).where(true));
  std::cerr
      << "Accessing a field directly from the result (using the current row): "
      << result1.begin()->id << std::endl;
  std::cerr << "Can do that again, no problem: " << result1.begin()->id
            << std::endl;

  auto tx = start_transaction(db);
  auto result2 =
      db(select(all_of(tab),
                value(select(max(tab.id).as(something)).from(tab).where(true))
                    .as(something))
             .from(tab)
             .where(true));
  if (const auto &row = *result2.begin()) {
    auto a = row.id;
    auto m = row.something;
    std::cerr << "-----------------------------" << a << ", " << m << std::endl;
  }
  tx.commit();

  return 0;
}

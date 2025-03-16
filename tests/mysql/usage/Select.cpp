/*
 * Copyright (c) 2013 - 2016, Roland Bock
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

#include <sqlpp23/mysql/database/connection.h>
#include <sqlpp23/sqlpp23.h>
#include <sqlpp23/tests/core/result_helpers.h>
#include <cassert>
#include "Tables.h"
#include "make_test_connection.h"

#include <iostream>
#include <vector>

namespace {
const auto library_raii =
    sqlpp::mysql::scoped_library_initializer_t{0, nullptr, nullptr};

namespace sql = sqlpp::mysql;
const auto tab = test::TabSample{};

SQLPP_CREATE_NAME_TAG(something);
SQLPP_CREATE_NAME_TAG(max_int_n);
}  // namespace

void testSelectAll(sql::connection& db, int expectedRowCount) {
  std::cerr << "--------------------------------------" << std::endl;
  int i = 0;
  for (const auto& row : db(sqlpp::select(all_of(tab)).from(tab).where(true))) {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", >>> row.intN: " << row.intN
              << ", row.textN: " << row.textN << ", row.boolN: " << row.boolN
              << std::endl;
    assert(i == row.id);
  };
  assert(i == expectedRowCount);

  auto preparedSelectAll =
      db.prepare(sqlpp::select(all_of(tab)).from(tab).where(true));
  std::cerr << "--------------------------------------" << std::endl;
  i = 0;
  for (const auto& row : db(preparedSelectAll)) {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", >>> row.intN: " << row.intN
              << ", row.textN: " << row.textN << ", row.boolN: " << row.boolN
              << std::endl;
    assert(i == row.id);
  };
  assert(i == expectedRowCount);

  // Try running the same prepared statement again
  std::cerr << "--------------------------------------" << std::endl;
  i = 0;
  for (const auto& row : db(preparedSelectAll)) {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", >>> row.intN: " << row.intN
              << ", row.textN: " << row.textN << ", row.boolN: " << row.boolN
              << std::endl;
    assert(i == row.id);
  };
  assert(i == expectedRowCount);
  std::cerr << "--------------------------------------" << std::endl;
}

int Select(int, char*[]) {
  try {
    auto db = sql::make_test_connection();
    test::createTabSample(db);

    testSelectAll(db, 0);
    db(insert_into(tab).default_values());
    testSelectAll(db, 1);
    db(insert_into(tab).set(tab.boolN = true, tab.textN = "cheesecake"));
    testSelectAll(db, 2);
    db(insert_into(tab).set(tab.boolN = true, tab.textN = "cheesecake"));
    testSelectAll(db, 3);

    // Test size functionality
    const auto test_size = db(select(all_of(tab)).from(tab).where(true));
    assert(test_size.size() == 3ull);

    // test functions and operators
    db(select(all_of(tab)).from(tab).where(tab.intN.is_null()));
    db(select(all_of(tab)).from(tab).where(tab.intN.is_not_null()));
    db(select(all_of(tab)).from(tab).where(tab.intN.in(1, 2, 3)));
    db(select(all_of(tab))
           .from(tab)
           .where(tab.intN.in(std::vector<int>{1, 2, 3, 4})));
    db(select(all_of(tab)).from(tab).where(tab.intN.not_in(1, 2, 3)));
    db(select(all_of(tab))
           .from(tab)
           .where(tab.intN.not_in(std::vector<int>{1, 2, 3, 4})));
    db(select(count(tab.intN).as(something)).from(tab).where(true));
    db(select(avg(tab.intN).as(something)).from(tab).where(true));
    db(select(max(tab.intN).as(something)).from(tab).where(true));
    db(select(min(tab.intN).as(something)).from(tab).where(true));
    db(select(
           exists(select(tab.intN).from(tab).where(tab.intN > 7)).as(something))
           .from(tab)
           .where(true));
    db(select(all_of(tab))
           .from(tab)
           .where(tab.intN ==
                  any(select(tab.intN).from(tab).where(tab.intN < 3))));

    db(select(all_of(tab)).from(tab).where(tab.intN + tab.intN > 3));
    db(select(all_of(tab)).from(tab).where((tab.textN + tab.textN) == ""));
    db(select(all_of(tab))
           .from(tab)
           .where((tab.textN + tab.textN).like("%'\"%")));

    // insert
    db(insert_into(tab).set(tab.boolN = true));

    // update
    db(update(tab).set(tab.boolN = false).where(tab.intN.in(1)));
    db(update(tab)
           .set(tab.boolN = false)
           .where(tab.intN.in(std::vector<int>{1, 2, 3, 4})));

    // remove
    {
      db(delete_from(tab).where(tab.intN == tab.intN + 3));

      auto result = db(select(all_of(tab)).from(tab).where(true));
      std::cerr << "Accessing a field directly from the result (using the "
                   "current row): "
                << result.begin()->intN << std::endl;
      std::cerr << "Can do that again, no problem: " << result.begin()->intN
                << std::endl;
    }

    // transaction
    {
      auto tx = start_transaction(db);
      auto result = db(
          select(
              all_of(tab),
              value(select(max(tab.intN).as(max_int_n)).from(tab).where(true))
                  .as(max_int_n))
              .from(tab)
              .where(true));
      if (const auto& row = *result.begin()) {
        std::optional<long> a = row.intN;
        std::optional<long> m = row.max_int_n;
        std::cerr << "-----------------------------" << a << ", " << m
                  << std::endl;
      }
      tx.commit();
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

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
#include "../../include/test_helpers.h"

#include <iostream>
#include <vector>

const auto library_raii = sqlpp::mysql::scoped_library_initializer_t{0, nullptr, nullptr};

namespace sql = sqlpp::mysql;
const auto tab = test::TabSample{};

void testSelectAll(sql::connection& db, int expectedRowCount)
{
  std::cerr << "--------------------------------------" << std::endl;
  int i = 0;
  for (const auto& row : db(sqlpp::select(all_of(tab)).from(tab).unconditionally()))
  {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", >>> row.intN: " << row.intN << ", row.textN: " << row.textN
              << ", row.boolN: " << row.boolN << std::endl;
    assert(i == row.id);
  };
  assert(i == expectedRowCount);

  auto preparedSelectAll = db.prepare(sqlpp::select(all_of(tab)).from(tab).unconditionally());
  std::cerr << "--------------------------------------" << std::endl;
  i = 0;
  for (const auto& row : db(preparedSelectAll))
  {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", >>> row.intN: " << row.intN << ", row.textN: " << row.textN
              << ", row.boolN: " << row.boolN << std::endl;
    assert(i == row.id);
  };
  assert(i == expectedRowCount);

  // Try running the same prepared statement again
  std::cerr << "--------------------------------------" << std::endl;
  i = 0;
  for (const auto& row : db(preparedSelectAll))
  {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", >>> row.intN: " << row.intN << ", row.textN: " << row.textN
              << ", row.boolN: " << row.boolN << std::endl;
    assert(i == row.id);
  };
  assert(i == expectedRowCount);
  std::cerr << "--------------------------------------" << std::endl;
}

int Select(int, char*[])
{
  try
  {
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
    const auto test_size = db(select(all_of(tab)).from(tab).unconditionally());
    assert(test_size.size() == 3ull);

    // test functions and operators
    db(select(all_of(tab)).from(tab).where(tab.intN.is_null()));
    db(select(all_of(tab)).from(tab).where(tab.intN.is_not_null()));
    db(select(all_of(tab)).from(tab).where(tab.intN.in(1, 2, 3)));
    db(select(all_of(tab)).from(tab).where(tab.intN.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));
    db(select(all_of(tab)).from(tab).where(tab.intN.not_in(1, 2, 3)));
    db(select(all_of(tab)).from(tab).where(tab.intN.not_in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));
    db(select(count(tab.intN)).from(tab).unconditionally());
    db(select(avg(tab.intN)).from(tab).unconditionally());
    db(select(max(tab.intN)).from(tab).unconditionally());
    db(select(min(tab.intN)).from(tab).unconditionally());
    db(select(exists(select(tab.intN).from(tab).where(tab.intN > 7))).from(tab).unconditionally());
    db(select(all_of(tab)).from(tab).where(tab.intN == any(select(tab.intN).from(tab).where(tab.intN < 3))));
    db(select(all_of(tab)).from(tab).where(tab.intN == some(select(tab.intN).from(tab).where(tab.intN < 3))));

    db(select(all_of(tab)).from(tab).where(tab.intN + tab.intN > 3));
    db(select(all_of(tab)).from(tab).where((tab.textN + tab.textN) == ""));
    db(select(all_of(tab)).from(tab).where((tab.textN + tab.textN).like("%'\"%")));

    // insert
    db(insert_into(tab).set(tab.boolN = true));

    // update
    db(update(tab).set(tab.boolN = false).where(tab.intN.in(1)));
    db(update(tab).set(tab.boolN = false).where(tab.intN.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));

    // remove
    {
    db(remove_from(tab).where(tab.intN == tab.intN + 3));

    auto result = db(select(all_of(tab)).from(tab).unconditionally());
    std::cerr << "Accessing a field directly from the result (using the current row): " << result.begin()->intN
              << std::endl;
    std::cerr << "Can do that again, no problem: " << result.begin()->intN << std::endl;
    }

    // transaction
    {
      auto tx = start_transaction(db);
      auto result = db(select(all_of(tab), select(max(tab.intN)).from(tab)).from(tab).unconditionally());
      if (const auto& row = *result.begin())
      {
        sqlpp::compat::optional<long> a = row.intN;
        sqlpp::compat::optional<long> m = row.max;
        std::cerr << "-----------------------------" << a << ", " << m << std::endl;
      }
      tx.commit();
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

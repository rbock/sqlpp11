/*
 * Copyright (c) 2013 - 2016, Roland Bock
 * Copyright (c) 2017, Juan Dent
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

#include "TabSample.h"
#include <cassert>
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlite3/connection.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/update.h>

#include <iostream>
#include <vector>

namespace sql = sqlpp::sqlite3;
const auto tab = TabSample{};

void testSelectAll(sql::connection& db, size_t expectedRowCount)
{
  std::cerr << "--------------------------------------" << std::endl;
  size_t i = 0;
  for (const auto& row : db(sqlpp::select(all_of(tab)).from(tab).unconditionally()))
  {
    ++i;
    std::cerr << ">>> row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma
              << std::endl;
    assert(static_cast<size_t>(row.alpha) == i);
  };
  assert(i == expectedRowCount);

  auto preparedSelectAll = db.prepare(sqlpp::select(all_of(tab)).from(tab).unconditionally());
  i = 0;
  for (const auto& row : db(preparedSelectAll))
  {
    ++i;
    std::cerr << ">>> row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma
              << std::endl;
    assert(static_cast<size_t>(row.alpha) == i);
  };
  assert(i == expectedRowCount);
  std::cerr << "--------------------------------------" << std::endl;
}

namespace string_util
{
  std::string ltrim(std::string str, const std::string& chars = "\t\n\v\f\r ")
  {
    str.erase(0, str.find_first_not_of(chars));
    return str;
  }

  std::string rtrim(std::string str, const std::string& chars = "\t\n\v\f\r ")
  {
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
  }

  std::string trim(std::string str, const std::string& chars = "\t\n\v\f\r ")
  {
    return ltrim(rtrim(str, chars), chars);
  }
}

int Select(int, char*[])
{
  sql::connection db({":memory:", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, "", true});
  db.execute(R"(CREATE TABLE tab_sample (
		alpha INTEGER PRIMARY KEY,
			beta varchar(255) DEFAULT NULL,
			gamma bool DEFAULT NULL
			))");

  testSelectAll(db, 0);
  db(insert_into(tab).default_values());
  testSelectAll(db, 1);
  db(insert_into(tab).set(tab.gamma = true, tab.beta = " cheesecake "));
  testSelectAll(db, 2);
  db(insert_into(tab).set(tab.gamma = true, tab.beta = " cheesecake "));
  testSelectAll(db, 3);

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
  db(select(trim(tab.beta)).from(tab).unconditionally());

  // db(select(not_exists(select(tab.alpha).from(tab).where(tab.alpha > 7))).from(tab));
  // db(select(all_of(tab)).from(tab).where(tab.alpha == any(select(tab.alpha).from(tab).where(tab.alpha < 3))));

  db(select(all_of(tab)).from(tab).where((tab.alpha + tab.alpha) > 3));
  db(select(all_of(tab)).from(tab).where((tab.beta + tab.beta) == ""));
  db(select(all_of(tab)).from(tab).where((tab.beta + tab.beta).like(R"(%'\"%)")));

  // update
  db(update(tab).set(tab.gamma = false).where(tab.alpha.in(1)));
  db(update(tab).set(tab.gamma = false).where(tab.alpha.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));

  // remove
  db(remove_from(tab).where(tab.alpha == tab.alpha + 3));

  auto result = db(select(all_of(tab)).from(tab).unconditionally());
  std::cerr << "Accessing a field directly from the result (using the current row): " << result.begin()->alpha
            << std::endl;
  std::cerr << "Can do that again, no problem: " << result.begin()->alpha << std::endl;

  std::cerr << "--------------------------------------" << std::endl;
  auto tx = start_transaction(db);
  for (const auto& row : db(select(all_of(tab), select(max(tab.alpha)).from(tab)).from(tab).unconditionally()))
  {
    const int64_t x = row.alpha;
    const int64_t a = row.max;
    std::cout << ">>>" << x << ", " << a << std::endl;
  }
  for (const auto& row :
       db(select(tab.alpha, tab.beta, tab.gamma, trim(tab.beta))
              .from(tab)
              .unconditionally()))
  {
    std::cerr << ">>> row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma
              << ", row.trim: '" << row.trim << "'" << std::endl;
    // check trim
    assert(string_util::trim(row.beta.value()) == row.trim.value());
    // end
  };

  for (const auto& row : db(select(all_of(tab), select(trim(tab.beta)).from(tab)).from(tab).unconditionally()))
  {
    const int64_t x = row.alpha;
    const std::string a = row.trim;
    std::cout << ">>>" << x << ", " << a << std::endl;
  }

  tx.commit();
  std::cerr << "--------------------------------------" << std::endl;

  return 0;
}

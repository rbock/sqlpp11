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

#include "Tables.h"
#include <cassert>
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/clause/insert.h>
#include <sqlpp11/clause/remove.h>
#include <sqlpp11/clause/select.h>
#include <sqlpp11/sqlite3/database/connection.h>
#include <sqlpp11/database/transaction.h>
#include <sqlpp11/clause/update.h>

#include <iostream>
#include <vector>

namespace sql = sqlpp::sqlite3;
const auto tab = test::TabSample{};

template <typename T>
std::ostream& operator<<(std::ostream& os, const sqlpp::compat::optional<T>& t) {
  if (not t)
    return os << "NULL";
  return os << t.value();
}

void testSelectAll(sql::connection& db, size_t expectedRowCount)
{
  std::cerr << "--------------------------------------" << std::endl;
  size_t i = 0;
  for (const auto& row : db(sqlpp::select(all_of(tab)).from(tab).unconditionally()))
  {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", row.alpha: " << row.alpha << ", row.beta: " << row.beta
              << ", row.gamma: " << row.gamma << std::endl;
    assert(row.id == static_cast<int64_t>(i));
  };
  assert(i == expectedRowCount);

  auto preparedSelectAll = db.prepare(sqlpp::select(all_of(tab)).from(tab).unconditionally());
  i = 0;
  for (const auto& row : db(preparedSelectAll))
  {
    ++i;
    std::cerr << ">>> row.id: " << row.id << ", row.alpha: " << row.alpha << ", row.beta: " << row.beta
              << ", row.gamma: " << row.gamma << std::endl;
    assert(row.id == static_cast<int64_t>(i));
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
  test::createTabSample(db);

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
    const auto x = row.alpha;
    const auto a = row.max;
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
    assert((not row.beta and not row.trim) || string_util::trim(std::string(row.beta.value())) == row.trim.value());
    // end
  };

  for (const auto& row : db(select(all_of(tab), select(trim(tab.beta)).from(tab)).from(tab).unconditionally()))
  {
    const sqlpp::compat::optional<int64_t> x = row.alpha;
    const sqlpp::compat::optional<sqlpp::compat::string_view> a = row.trim;
    std::cout << ">>>" << x << ", " << a << std::endl;
  }

  tx.commit();
  std::cerr << "--------------------------------------" << std::endl;

  return 0;
}

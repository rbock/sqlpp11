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

#include "TabSample.h"
#include <sqlpp11/custom_query.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <cassert>
#include <iostream>
#include <vector>

SQLPP_ALIAS_PROVIDER(pragma)
SQLPP_ALIAS_PROVIDER(sub)

namespace sql = sqlpp::sqlite3;
int Sample(int, char*[])
{
  sql::connection_config config;
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  config.debug = true;

  sql::connection db(config);
  db.execute(R"(CREATE TABLE tab_sample (
		alpha INTEGER PRIMARY KEY,
			beta varchar(255) DEFAULT NULL,
			gamma bool DEFAULT NULL
			))");
  db.execute(R"(CREATE TABLE tab_foo (
		omega bigint(20) DEFAULT NULL
			))");

  const auto tab = TabSample{};

  // clear the table
  db(remove_from(tab).unconditionally());

  // explicit all_of(tab)
  for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
  {
    std::cerr << "row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma << std::endl;
  };
  std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
  // selecting a table implicitly expands to all_of(tab)
  for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
  {
    std::cerr << "row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma << std::endl;
  };
  // insert
  std::cerr << "no of required columns: " << TabSample::_required_insert_columns::size::value << std::endl;
  db(insert_into(tab).default_values());
  std::cout << "Last Insert ID: " << db.last_insert_id() << "\n";
  db(insert_into(tab).set(tab.gamma = true));
  std::cout << "Last Insert ID: " << db.last_insert_id() << "\n";
  auto di = dynamic_insert_into(db, tab).dynamic_set(tab.gamma = true);
  di.insert_list.add(tab.beta = "");
  db(di);

  // update
  db(update(tab).set(tab.gamma = false).where(tab.alpha.in(1)));
  db(update(tab).set(tab.gamma = false).where(tab.alpha.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));

  // remove
  db(remove_from(tab).where(tab.alpha == tab.alpha + 3));

  auto result = db(select(all_of(tab)).from(tab).unconditionally());
  std::cerr << "Accessing a field directly from the result (using the current row): " << result.begin()->alpha
            << std::endl;
  std::cerr << "Can do that again, no problem: " << result.begin()->alpha << std::endl;

  auto tx = start_transaction(db);
  TabFoo foo;
  for (const auto& row : db(select(all_of(tab), select(max(foo.omega)).from(foo).where(foo.omega > tab.alpha))
                                .from(tab)
                                .unconditionally()))
  {
    int64_t x = row.alpha;
    int64_t a = row.max;
    std::cout << x << ", " << a << std::endl;
  }
  tx.commit();

  for (const auto& row : db(select(tab.alpha).from(tab.join(foo).on(tab.alpha == foo.omega)).unconditionally()))
  {
    std::cerr << row.alpha << std::endl;
  }

  for (const auto& row :
       db(select(tab.alpha).from(tab.left_outer_join(foo).on(tab.alpha == foo.omega)).unconditionally()))
  {
    std::cerr << row.alpha << std::endl;
  }

  auto ps = db.prepare(select(all_of(tab))
                           .from(tab)
                           .where(tab.alpha != parameter(tab.alpha) and tab.beta != parameter(tab.beta) and
                                  tab.gamma != parameter(tab.gamma)));
  ps.params.alpha = 7;
  ps.params.beta = "wurzelbrunft";
  ps.params.gamma = true;
  for (const auto& row : db(ps))
  {
    std::cerr << "bound result: alpha: " << row.alpha << std::endl;
    std::cerr << "bound result: beta: " << row.beta << std::endl;
    std::cerr << "bound result: gamma: " << row.gamma << std::endl;
  }

  std::cerr << "--------" << std::endl;
  ps.params.alpha = sqlpp::eval<sqlpp::integer>(db, "last_insert_rowid()");
  ps.params.gamma = false;
  for (const auto& row : db(ps))
  {
    std::cerr << "bound result: alpha: " << row.alpha << std::endl;
    std::cerr << "bound result: beta: " << row.beta << std::endl;
    std::cerr << "bound result: gamma: " << row.gamma << std::endl;
  }

  std::cerr << "--------" << std::endl;
  ps.params.beta = "kaesekuchen";
  for (const auto& row : db(ps))
  {
    std::cerr << "bound result: alpha: " << row.alpha << std::endl;
    std::cerr << "bound result: beta: " << row.beta << std::endl;
    std::cerr << "bound result: gamma: " << row.gamma << std::endl;
  }

  auto pi = db.prepare(insert_into(tab).set(tab.beta = parameter(tab.beta), tab.gamma = true));
  pi.params.beta = "prepared cake";
  std::cerr << "Inserted: " << db(pi) << std::endl;

  auto pu = db.prepare(update(tab).set(tab.gamma = parameter(tab.gamma)).where(tab.beta == "prepared cake"));
  pu.params.gamma = false;
  std::cerr << "Updated: " << db(pu) << std::endl;

  auto pr = db.prepare(remove_from(tab).where(tab.beta != parameter(tab.beta)));
  pr.params.beta = "prepared cake";
  std::cerr << "Deleted lines: " << db(pr) << std::endl;

  // Check that a prepared select is default-constructible
  {
    auto s = select(all_of(tab))
                 .from(tab)
                 .where((tab.beta.like(parameter(tab.beta)) and tab.alpha == parameter(tab.alpha)) or
                        tab.gamma != parameter(tab.gamma));
    using P = decltype(db.prepare(s));
    P p;  // You must not use this one yet!
    p = db.prepare(s);
  }

  auto i = db(sqlpp::sqlite3::insert_or_replace_into(tab).set(tab.beta = "test", tab.gamma = true));
  std::cerr << i << std::endl;

  i = db(sqlpp::sqlite3::insert_or_ignore_into(tab).set(tab.beta = "test", tab.gamma = true));
  std::cerr << i << std::endl;

  assert(db(select(count(tab.alpha)).from(tab).unconditionally()).begin()->count);
  assert(
      db(select(all_of(tab)).from(tab).where(tab.alpha.not_in(select(tab.alpha).from(tab).unconditionally()))).empty());

  auto x = custom_query(sqlpp::verbatim("PRAGMA user_version = "), 1);
  db(x);
  const int64_t pragmaValue =
      db(custom_query(sqlpp::verbatim("PRAGMA user_version")).with_result_type_of(select(sqlpp::value(1).as(pragma))))
          .front()
          .pragma;
  std::cerr << pragmaValue << std::endl;

  // Testing sub select tables and unconditional joins
  const auto subQuery = select(tab.alpha).from(tab).unconditionally().as(sub);
  for (const auto& row : db(select(subQuery.alpha).from(subQuery).unconditionally()))
  {
    std::cerr << row.alpha;
  }

  for (const auto& row : db(select(subQuery.alpha).from(tab.inner_join(subQuery).unconditionally()).unconditionally()))
  {
    std::cerr << row.alpha;
  }

  return 0;
}

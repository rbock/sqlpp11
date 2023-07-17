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
#include "TabSample.h"
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>

#include <cassert>
#include <iostream>
#include <vector>

namespace sql = sqlpp::mysql;
int Sample(int, char*[])
{
  sql::global_library_init();
  try
  {
    auto db = sql::make_test_connection();
    db.execute(R"(DROP TABLE IF EXISTS tab_sample)");
    db.execute(R"(CREATE TABLE tab_sample (
			alpha bigint(20) AUTO_INCREMENT,
			beta varchar(255) DEFAULT NULL,
			gamma bool DEFAULT NULL,
			PRIMARY KEY (alpha)
			))");
    db.execute(R"(DROP TABLE IF EXISTS tab_foo)");
    db.execute(R"(CREATE TABLE tab_foo (
		omega bigint(20) DEFAULT NULL
			))");

    assert(not db(select(sqlpp::value(false).as(sqlpp::alias::a))).front().a);

    const auto tab = TabSample{};
    // clear the table
    db(remove_from(tab).unconditionally());

    // Several ways of ensuring that tab is empty
    assert(not db(select(exists(select(tab.alpha).from(tab).unconditionally())))
                   .front()
                   .exists);  // this is probably the fastest
    assert(not db(select(count(tab.alpha)).from(tab).unconditionally()).front().count);
    assert(db(select(tab.alpha).from(tab).unconditionally()).empty());

    // explicit all_of(tab)
    std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
    select(all_of(tab)).from(tab);
    std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
    db(select(all_of(tab)).from(tab).unconditionally());
    std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
      std::cerr << "row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma
                << std::endl;
    };
    // insert
    db(insert_into(tab).default_values());
    const auto x = select(all_of(tab)).from(tab).unconditionally();
    const auto y = db.prepare(x);
    for (const auto& row : db(db.prepare(select(all_of(tab)).from(tab).unconditionally())))
    {
      std::cerr << "alpha: " << row.alpha.is_null() << std::endl;
      std::cerr << "beta: " << row.beta.is_null() << std::endl;
      std::cerr << "gamma: " << row.gamma.is_null() << std::endl;
    }
    db(insert_into(tab).set(tab.beta = "kaesekuchen", tab.gamma = true));
    db(insert_into(tab).default_values());
    db(insert_into(tab).set(tab.beta = "", tab.gamma = true));

    // update
    db(update(tab).set(tab.gamma = false).where(tab.alpha.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));
    db(update(tab).set(tab.gamma = true).where(tab.alpha.in(1)));

    // dynamic insert
    auto dynin = dynamic_insert_into(db, tab).dynamic_set(tab.gamma = true);
    dynin.insert_list.add(tab.beta = "cheesecake");
    db(dynin);

    // remove
    {
      db(remove_from(tab).where(tab.alpha == tab.alpha + 3));

      std::cerr << "+++++++++++++++++++++++++++" << std::endl;
      for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
      {
        std::cerr << __LINE__ << " row.beta: " << row.beta << std::endl;
      }
      std::cerr << "+++++++++++++++++++++++++++" << std::endl;
      decltype(db(select(all_of(tab)).from(tab).unconditionally())) result;
      result = db(select(all_of(tab)).from(tab).unconditionally());
      std::cerr << "Accessing a field directly from the result (using the current row): " << result.begin()->alpha
                << std::endl;
      std::cerr << "Can do that again, no problem: " << result.begin()->alpha << std::endl;
    }

    // transaction
    {
      auto tx = start_transaction(db);
      auto result = db(select(all_of(tab), select(max(tab.alpha)).from(tab)).from(tab).unconditionally());
      if (const auto& row = *result.begin())
      {
        long a = row.alpha;
        long m = row.max;
        std::cerr << __LINE__ << " row.alpha: " << a << ", row.max: " << m << std::endl;
      }
      tx.commit();
    }

    TabFoo foo;
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

    for (const auto& row : db(select(case_when(tab.gamma).then(tab.alpha).else_(foo.omega).as(tab.alpha))
                                  .from(tab.cross_join(foo))
                                  .unconditionally()))
    {
      std::cerr << row.alpha << std::endl;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

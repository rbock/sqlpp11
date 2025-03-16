/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include <sqlpp23/sqlpp23.h>
#include <sqlpp23/tests/core/MockDb.h>
#include <sqlpp23/tests/core/result_helpers.h>
#include <sqlpp23/tests/core/tables.h>
#include "is_regular.h"

int With(int, char*[]) {
  MockDb db;
  MockDb::_context_t printer = {};

  const auto t = test::TabBar{};

  auto x =
      sqlpp::cte(sqlpp::alias::x).as(select(all_of(t)).from(t).where(true));

  db(with(x)(select(x.id).from(x).where(true)));

  auto y0 =
      sqlpp::cte(sqlpp::alias::y).as(select(all_of(t)).from(t).where(true));
  auto y = y0.union_all(select(all_of(y0)).from(y0).where(true));

  std::cout << to_sql_string(printer, y) << std::endl;
  std::cout << to_sql_string(printer, from(y)) << std::endl;

  db(with(y)(select(y.id).from(y).where(true)));

  using ::sqlpp::alias::a;
  using ::sqlpp::alias::b;
  const auto c =
      sqlpp::cte(b).as(select(t.id.as(a))
                           .from(t)
                           .where(true)
                           .union_all(select(sqlpp::value(123).as(a))));
  db(with(c)(select(all_of(c)).from(c).where(true)));

  // recursive CTE with join
  {
    const auto selectBase = select(t.id, t.intN).from(t).where(t.id > 17);
    const auto initialCte = ::sqlpp::cte(sqlpp::alias::a).as(selectBase);
    const auto recursiveCte = initialCte.union_all(
        select(t.id, t.intN)
            .from(t.join(initialCte).on(t.id == initialCte.intN))
            .where(true));
    const auto query = with(recursiveCte)(
        select(recursiveCte.id).from(recursiveCte).where(true));

    const auto serializedQuery = to_sql_string(printer, query);
    std::cout << serializedQuery << '\n';

    for (const auto& row : db(query)) {
      std::cout << row.id;
    }
  }

  return 0;
}

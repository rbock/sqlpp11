/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/tests/core/MockDb.h>
#include <sqlpp11/tests/core/result_helpers.h>
#include <sqlpp11/tests/core/tables.h>

int Interpret(int, char *[]) {
  MockDb db = {};
  MockDb::_context_t ctx = {};

  const auto f = test::TabFoo{};
  const auto t = test::TabBar{};
  select(t.id.as(t.textN));

  to_sql_string(ctx, insert_into(t).columns(t.textN, t.boolNn));
  {
    auto i = insert_into(t).columns(t.boolNn, t.textN);
    i.add_values(t.boolNn = true, t.textN = "cheesecake");
    to_sql_string(ctx, i);
    i.add_values(t.boolNn = false, t.textN = std::nullopt);
    to_sql_string(ctx, i);
  }

  to_sql_string(ctx, t.id);
  to_sql_string(ctx, -t.id);
  to_sql_string(ctx, -(t.id + 7));
  to_sql_string(ctx, t.id = 0);
  to_sql_string(ctx, t.id == 0);
  to_sql_string(ctx, t.id != 0);
  to_sql_string(ctx, t.id == 7);
  to_sql_string(ctx, t.textN + "kaesekuchen");

  to_sql_string(ctx, sqlpp::select());
  to_sql_string(ctx, sqlpp::select().flags(sqlpp::distinct));
  to_sql_string(ctx, select(t.id, t.textN).flags(sqlpp::distinct));
  to_sql_string(ctx, select(t.id, t.textN));
  to_sql_string(ctx, select(t.id, t.textN).from(t));
  to_sql_string(ctx, select(t.id, t.textN).from(t).where(t.id == 3));
  to_sql_string(
      ctx, select(t.id, t.textN).from(t).where(t.id == 3).group_by(t.boolNn));
  to_sql_string(ctx, select(t.id, t.textN)
                         .from(t)
                         .where(t.id == 3)
                         .group_by(t.boolNn)
                         .having(t.textN.like("%kuchen")));
  to_sql_string(ctx, select(t.id, t.textN)
                         .from(t)
                         .where(t.id == 3)
                         .group_by(t.boolNn)
                         .having(t.textN.like("%kuchen"))
                         .order_by(t.textN.asc()));
  to_sql_string(ctx, select(t.id, t.textN)
                         .from(t)
                         .where(t.id == 3)
                         .group_by(t.boolNn)
                         .having(t.textN.like("%kuchen"))
                         .order_by(t.textN.asc())
                         .limit(17u)
                         .offset(3u));

  to_sql_string(ctx, parameter(sqlpp::integral(), t.id));
  to_sql_string(ctx, parameter(t.id));
  to_sql_string(ctx, t.id == parameter(t.id));
  to_sql_string(ctx, t.id == parameter(t.id) and
                         (t.textN + "gimmick").like(parameter(t.textN)));

  to_sql_string(ctx, insert_into(t));
  to_sql_string(ctx, insert_into(f).default_values());
  to_sql_string(ctx, insert_into(t).set(t.boolNn = true));

  to_sql_string(ctx, update(t));
  to_sql_string(ctx, update(t).set(t.boolNn = true));
  to_sql_string(ctx, update(t)
                         .set(t.boolNn = true)
                         .where(t.textN.in("kaesekuchen", "cheesecake")));

  to_sql_string(ctx, delete_from(t));

  // functions
  to_sql_string(ctx, sqlpp::value(7));
  to_sql_string(ctx, sqlpp::verbatim<sqlpp::integral>("something integral"));
  to_sql_string(ctx, t.id.in(std::vector<int>({1, 2, 3, 4, 5, 6, 8})));
  to_sql_string(ctx, sqlpp::in(t.id, std::vector<int>({1, 2, 3, 4, 5, 6, 8})));
  to_sql_string(ctx, exists(select(t.id).from(t).where(true)));
  to_sql_string(ctx, any(select(t.id).from(t).where(true)));
  to_sql_string(ctx, count(t.id));
  to_sql_string(ctx, min(t.id));
  to_sql_string(ctx, max(t.id));
  to_sql_string(ctx, avg(t.id));
  to_sql_string(ctx, sum(t.id));
  to_sql_string(ctx, sqlpp::verbatim_table("whatever"));

  // alias
  to_sql_string(ctx, t.as(t.id));
  to_sql_string(ctx, t.as(t.id).textN);

  // select alias
  to_sql_string(
      ctx, select(t.id).from(t).where(t.textN > "kaesekuchen").as(t.boolNn));

  // Comparison to null
  static_assert(not sqlpp::can_be_null<decltype(t.id)>::value,
                "expected id cannot be null");
  static_assert(not sqlpp::can_be_null<decltype(f.textNnD)>::value,
                "expected intN cannot be null");
  to_sql_string(ctx, t.id.is_null());
  to_sql_string(ctx, f.textNnD.is_null());

  // join
  to_sql_string(ctx, t.inner_join(t.as(t.id)).on(t.textN == t.as(t.id).textN));
  {
    auto inner = t.inner_join(t.as(t.id)).on(t.textN == t.as(t.id).textN);
    to_sql_string(ctx, select(t.id).from(inner));
  }

  // distinct aggregate
  to_sql_string(ctx, count(sqlpp::distinct, t.id % 7));
  to_sql_string(ctx, avg(sqlpp::distinct, t.id - 7));
  to_sql_string(ctx, sum(sqlpp::distinct, t.id + 7));

  to_sql_string(ctx, select(all_of(t)).from(t).where(true));

  for (const auto &row : db(select(all_of(t)).from(t).where(true))) {
    to_sql_string(ctx, t.id == row.id);
    to_sql_string(ctx, t.textN == row.textN.value());
    to_sql_string(ctx, t.boolNn == row.boolNn);
  }

  get_sql_name(t);
  get_sql_name(t.id);

  flatten(ctx, t.id == 7);

  std::cerr << to_sql_string(ctx, select(all_of(t)).from(t).where(t.id.in(
                                      select(f.intN).from(f).where(true))))
            << std::endl;

  std::cerr << to_sql_string(ctx, select(all_of(t)).from(t).where(t.id.in(7)))
            << std::endl;

  std::cerr << to_sql_string(ctx,
                             select(all_of(t)).from(t).where(t.id.not_in(7)))
            << std::endl;

  auto schema = db.attach("lorem");
  auto s = schema_qualified_table(schema, t).as(sqlpp::alias::x);

  std::cerr << to_sql_string(ctx, select(all_of(s)).from(s).where(true))
            << std::endl;

  std::cerr << to_sql_string(
                   ctx, sqlpp::case_when(true).then(t.id).else_(t.id + 1).as(
                            t.textN))
            << std::endl;

  return 0;
}

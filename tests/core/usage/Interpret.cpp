/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include "MockDb.h"
#include "Sample.h"
#include <sqlpp11/sqlpp11.h>

#include "../../include/test_helpers.h"

int Interpret(int, char* [])
{
  MockDb db = {};
  MockDb::_serializer_context_t printer = {};

  const auto f = test::TabFoo{};
  const auto t = test::TabBar{};
  select(t.id.as(t.textN));

  to_sql_string(printer, insert_into(t).columns(t.textN, t.boolNn)).str();
  {
    auto i = insert_into(t).columns(t.boolNn, t.textN);
    i.add_values(t.boolNn = true, t.textN = "cheesecake");
    to_sql_string(printer, i).str();
    i.add_values(t.boolNn = false, t.textN = ::sqlpp::nullopt);
    to_sql_string(printer, i).str();
  }

  to_sql_string(printer, t.id).str();
  to_sql_string(printer, -t.id).str();
  to_sql_string(printer, -(t.id + 7)).str();
  to_sql_string(printer, t.id = 0).str();
  to_sql_string(printer, t.id == 0).str();
  to_sql_string(printer, t.id != 0).str();
  to_sql_string(printer, t.id == 7).str();
  to_sql_string(printer, t.textN + "kaesekuchen").str();

  to_sql_string(printer, sqlpp::select()).str();
  to_sql_string(printer, sqlpp::select().flags(sqlpp::distinct)).str();
  to_sql_string(printer, select(t.id, t.textN).flags(sqlpp::distinct)).str();
  to_sql_string(printer, select(t.id, t.textN)).str();
  to_sql_string(printer, select(t.id, t.textN).from(t)).str();
  to_sql_string(printer, select(t.id, t.textN).from(t).where(t.id == 3)).str();
  to_sql_string(printer, select(t.id, t.textN).from(t).where(t.id == 3).group_by(t.boolNn)).str();
  to_sql_string(printer, select(t.id, t.textN).from(t).where(t.id == 3).group_by(t.boolNn).having(t.textN.like("%kuchen")))
      .str();
  to_sql_string(printer, select(t.id, t.textN)
                .from(t)
                .where(t.id == 3)
                .group_by(t.boolNn)
                .having(t.textN.like("%kuchen"))
                .order_by(t.textN.asc()))
      .str();
  to_sql_string(printer, select(t.id, t.textN)
                .from(t)
                .where(t.id == 3)
                .group_by(t.boolNn)
                .having(t.textN.like("%kuchen"))
                .order_by(t.textN.asc())
                .limit(17u)
                .offset(3u))
      .str();

  to_sql_string(printer, parameter(sqlpp::integral(), t.id)).str();
  to_sql_string(printer, parameter(t.id)).str();
  to_sql_string(printer, t.id == parameter(t.id)).str();
  to_sql_string(printer, t.id == parameter(t.id) and (t.textN + "gimmick").like(parameter(t.textN))).str();

  to_sql_string(printer, insert_into(t)).str();
  to_sql_string(printer, insert_into(f).default_values()).str();
  to_sql_string(printer, insert_into(t).set(t.boolNn = true)).str();

  to_sql_string(printer, update(t)).str();
  to_sql_string(printer, update(t).set(t.boolNn = true)).str();
  to_sql_string(printer, update(t).set(t.boolNn = true).where(t.textN.in("kaesekuchen", "cheesecake"))).str();

  to_sql_string(printer, remove_from(t)).str();
  to_sql_string(printer, remove_from(t).using_(t)).str();

  // functions
  to_sql_string(printer, sqlpp::value(7)).str();
  to_sql_string(printer, sqlpp::verbatim<sqlpp::integral>("something integral")).str();
  to_sql_string(printer, t.id.in(std::vector<int>({1, 2, 3, 4, 5, 6, 8}))).str();
  to_sql_string(printer, sqlpp::in(t.id, std::vector<int>({1, 2, 3, 4, 5, 6, 8}))).str();
  to_sql_string(printer, exists(select(t.id).from(t))).str();
  to_sql_string(printer, any(select(t.id).from(t))).str();
  to_sql_string(printer, count(t.id)).str();
  to_sql_string(printer, min(t.id)).str();
  to_sql_string(printer, max(t.id)).str();
  to_sql_string(printer, avg(t.id)).str();
  to_sql_string(printer, sum(t.id)).str();
  to_sql_string(printer, sqlpp::verbatim_table("whatever")).str();

  // alias
  to_sql_string(printer, t.as(t.id)).str();
  to_sql_string(printer, t.as(t.id).textN).str();

  // select alias
  to_sql_string(printer, select(t.id).from(t).where(t.textN > "kaesekuchen").as(t.boolNn)).str();

  // Comparison to null
  static_assert(not sqlpp::can_be_null<decltype(t.id)>::value, "expected id cannot be null");
  static_assert(not sqlpp::can_be_null<decltype(f.textNnD)>::value, "expected intN cannot be null");
  to_sql_string(printer, t.id.is_null()).str();
  to_sql_string(printer, f.textNnD.is_null()).str();

  // join
  to_sql_string(printer, t.inner_join(t.as(t.id)).on(t.textN == t.as(t.id).textN)).str();
  {
    auto inner = t.inner_join(t.as(t.id)).on(t.textN == t.as(t.id).textN);
    to_sql_string(printer, select(t.id).from(inner)).str();
  }

  // distinct aggregate
  to_sql_string(printer, count(sqlpp::distinct, t.id % 7)).str();
  to_sql_string(printer, avg(sqlpp::distinct, t.id - 7)).str();
  to_sql_string(printer, sum(sqlpp::distinct, t.id + 7)).str();

  to_sql_string(printer, select(all_of(t)).from(t).unconditionally()).str();

  for (const auto& row : db(select(all_of(t)).from(t).unconditionally()))
  {
    to_sql_string(printer, t.id == row.id);
    to_sql_string(printer, t.textN == row.textN.value());
    to_sql_string(printer, t.boolNn == row.boolNn);
  }

  get_sql_name(t);
  get_sql_name(t.id);

  flatten(t.id == 7, db);

  printer.reset();
  std::cerr << to_sql_string(printer, select(all_of(t)).from(t).where(t.id.in(select(f.intN).from(f).unconditionally())))
                   .str()
            << std::endl;

  printer.reset();
  std::cerr << to_sql_string(printer, select(all_of(t)).from(t).where(t.id.in(7))).str() << std::endl;

  printer.reset();
  std::cerr << to_sql_string(printer, select(all_of(t)).from(t).where(t.id.not_in(7))).str() << std::endl;

#warning: reactivate test
  /*
  auto schema = db.attach("lorem");
  auto s = schema_qualified_table(schema, t).as(sqlpp::alias::x);

  printer.reset();
  std::cerr << to_sql_string(printer, select(all_of(s)).from(s).unconditionally()).str() << std::endl;
*/

  printer.reset();
  std::cerr << to_sql_string(printer, sqlpp::case_when(true).then(t.id).else_(t.id + 1).as(t.textN)).str()
            << std::endl;

  return 0;
}

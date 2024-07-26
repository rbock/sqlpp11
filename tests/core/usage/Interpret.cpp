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

  serialize(printer, insert_into(t).columns(t.textN, t.boolNn)).str();
  {
    auto i = insert_into(t).columns(t.boolNn, t.textN);
    i.add_values(t.boolNn = true, t.textN = "cheesecake");
    serialize(printer, i).str();
    i.add_values(t.boolNn = false, t.textN = sqlpp::compat::nullopt);
    serialize(printer, i).str();
  }

  serialize(printer, t.id).str();
  serialize(printer, -t.id).str();
  serialize(printer, -(t.id + 7)).str();
  serialize(printer, t.id = 0).str();
  serialize(printer, t.id == 0).str();
  serialize(printer, t.id != 0).str();
  serialize(printer, t.id == 7).str();
  serialize(printer, t.textN + "kaesekuchen").str();

  serialize(printer, sqlpp::select()).str();
  serialize(printer, sqlpp::select().flags(sqlpp::distinct)).str();
  serialize(printer, select(t.id, t.textN).flags(sqlpp::distinct)).str();
  serialize(printer, select(t.id, t.textN)).str();
  serialize(printer, select(t.id, t.textN).from(t)).str();
  serialize(printer, select(t.id, t.textN).from(t).where(t.id == 3)).str();
  serialize(printer, select(t.id, t.textN).from(t).where(t.id == 3).group_by(t.boolNn)).str();
  serialize(printer, select(t.id, t.textN).from(t).where(t.id == 3).group_by(t.boolNn).having(t.textN.like("%kuchen")))
      .str();
  serialize(printer, select(t.id, t.textN)
                .from(t)
                .where(t.id == 3)
                .group_by(t.boolNn)
                .having(t.textN.like("%kuchen"))
                .order_by(t.textN.asc()))
      .str();
  serialize(printer, select(t.id, t.textN)
                .from(t)
                .where(t.id == 3)
                .group_by(t.boolNn)
                .having(t.textN.like("%kuchen"))
                .order_by(t.textN.asc())
                .limit(17u)
                .offset(3u))
      .str();

  serialize(printer, parameter(sqlpp::integral(), t.id)).str();
  serialize(printer, parameter(t.id)).str();
  serialize(printer, t.id == parameter(t.id)).str();
  serialize(printer, t.id == parameter(t.id) and (t.textN + "gimmick").like(parameter(t.textN))).str();

  serialize(printer, insert_into(t)).str();
  serialize(printer, insert_into(f).default_values()).str();
  serialize(printer, insert_into(t).set(t.boolNn = true)).str();

  serialize(printer, update(t)).str();
  serialize(printer, update(t).set(t.boolNn = true)).str();
  serialize(printer, update(t).set(t.boolNn = true).where(t.textN.in("kaesekuchen", "cheesecake"))).str();

  serialize(printer, remove_from(t)).str();
  serialize(printer, remove_from(t).using_(t)).str();

  // functions
  serialize(printer, sqlpp::value(7)).str();
  serialize(printer, sqlpp::verbatim<sqlpp::integral>("something integral")).str();
  serialize(printer, t.id.in(std::vector<int>({1, 2, 3, 4, 5, 6, 8}))).str();
  serialize(printer, sqlpp::in(t.id, std::vector<int>({1, 2, 3, 4, 5, 6, 8}))).str();
  serialize(printer, exists(select(t.id).from(t))).str();
  serialize(printer, any(select(t.id).from(t))).str();
  serialize(printer, count(t.id)).str();
  serialize(printer, min(t.id)).str();
  serialize(printer, max(t.id)).str();
  serialize(printer, avg(t.id)).str();
  serialize(printer, sum(t.id)).str();
  serialize(printer, sqlpp::verbatim_table("whatever")).str();

  // alias
  serialize(printer, t.as(t.id)).str();
  serialize(printer, t.as(t.id).textN).str();

  // select alias
  serialize(printer, select(t.id).from(t).where(t.textN > "kaesekuchen").as(t.boolNn)).str();

  // Comparison to null
  static_assert(not sqlpp::can_be_null<decltype(t.id)>::value, "expected id cannot be null");
  static_assert(not sqlpp::can_be_null<decltype(f.textNnD)>::value, "expected intN cannot be null");
  serialize(printer, t.id.is_null()).str();
  serialize(printer, f.textNnD.is_null()).str();

  // join
  serialize(printer, t.inner_join(t.as(t.id)).on(t.textN == t.as(t.id).textN)).str();
  {
    auto inner = t.inner_join(t.as(t.id)).on(t.textN == t.as(t.id).textN);
    serialize(printer, select(t.id).from(inner)).str();
  }

  // distinct aggregate
  serialize(printer, count(sqlpp::distinct, t.id % 7)).str();
  serialize(printer, avg(sqlpp::distinct, t.id - 7)).str();
  serialize(printer, sum(sqlpp::distinct, t.id + 7)).str();

  serialize(printer, select(all_of(t)).from(t).unconditionally()).str();

  for (const auto& row : db(select(all_of(t)).from(t).unconditionally()))
  {
    serialize(printer, t.id == row.id);
    serialize(printer, t.textN == row.textN.value());
    serialize(printer, t.boolNn == row.boolNn);
  }

  get_sql_name(t);
  get_sql_name(t.id);

  flatten(t.id == 7, db);

  printer.reset();
  std::cerr << serialize(printer, select(all_of(t)).from(t).where(t.id.in(select(f.intN).from(f).unconditionally())))
                   .str()
            << std::endl;

  printer.reset();
  std::cerr << serialize(printer, select(all_of(t)).from(t).where(t.id.in(7))).str() << std::endl;

  printer.reset();
  std::cerr << serialize(printer, select(all_of(t)).from(t).where(t.id.not_in(7))).str() << std::endl;

#warning: reactivate test
  /*
  auto schema = db.attach("lorem");
  auto s = schema_qualified_table(schema, t).as(sqlpp::alias::x);

  printer.reset();
  std::cerr << serialize(printer, select(all_of(s)).from(s).unconditionally()).str() << std::endl;
*/

  printer.reset();
  std::cerr << serialize(printer, sqlpp::case_when(true).then(t.id).else_(t.id + 1).as(t.textN)).str()
            << std::endl;

  return 0;
}

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

  serialize(insert_into(t).columns(t.textN, t.boolNn), printer).str();
  {
    auto i = insert_into(t).columns(t.boolNn, t.textN);
    i.add_values(t.boolNn = true, t.textN = "cheesecake");
    serialize(i, printer).str();
    i.add_values(t.boolNn = sqlpp::default_value, t.textN = sqlpp::null);
    serialize(i, printer).str();
  }

  serialize(t.id, printer).str();
  serialize(-t.id, printer).str();
  serialize(+t.id, printer).str();
  serialize(-(t.id + 7), printer).str();
  serialize(t.id = 0, printer).str();
  serialize(t.id == 0, printer).str();
  serialize(t.id != 0, printer).str();
  serialize(t.id == 7, printer).str();
  serialize(t.textN + "kaesekuchen", printer).str();

  serialize(sqlpp::select(), printer).str();
  serialize(sqlpp::select().flags(sqlpp::distinct), printer).str();
  serialize(select(t.id, t.textN).flags(sqlpp::distinct), printer).str();
  serialize(select(t.id, t.textN), printer).str();
  serialize(select(t.id, t.textN).from(t), printer).str();
  serialize(select(t.id, t.textN).from(t).where(t.id == 3), printer).str();
  serialize(select(t.id, t.textN).from(t).where(t.id == 3).group_by(t.boolNn), printer).str();
  serialize(select(t.id, t.textN).from(t).where(t.id == 3).group_by(t.boolNn).having(t.textN.like("%kuchen")),
            printer)
      .str();
  serialize(select(t.id, t.textN)
                .from(t)
                .where(t.id == 3)
                .group_by(t.boolNn)
                .having(t.textN.like("%kuchen"))
                .order_by(t.textN.asc()),
            printer)
      .str();
  serialize(select(t.id, t.textN)
                .from(t)
                .where(t.id == 3)
                .group_by(t.boolNn)
                .having(t.textN.like("%kuchen"))
                .order_by(t.textN.asc())
                .limit(17u)
                .offset(3u),
            printer)
      .str();

  serialize(parameter(sqlpp::bigint(), t.id), printer).str();
  serialize(parameter(t.id), printer).str();
  serialize(t.id == parameter(t.id), printer).str();
  serialize(t.id == parameter(t.id) and (t.textN + "gimmick").like(parameter(t.textN)), printer).str();

  serialize(insert_into(t), printer).str();
  serialize(insert_into(f).default_values(), printer).str();
  serialize(insert_into(t).set(t.boolNn = true), printer).str();

  serialize(update(t), printer).str();
  serialize(update(t).set(t.boolNn = true), printer).str();
  serialize(update(t).set(t.boolNn = true).where(t.textN.in("kaesekuchen", "cheesecake")), printer).str();
  serialize(update(t).set(t.boolNn = true).where(t.textN.in()), printer).str();

  serialize(remove_from(t), printer).str();
  serialize(remove_from(t).using_(t), printer).str();

  // functions
  serialize(sqlpp::value(7), printer).str();
  serialize(sqlpp::verbatim<sqlpp::integral>("irgendwas integrales"), printer).str();
  serialize(sqlpp::value_list(std::vector<int>({1, 2, 3, 4, 5, 6, 8})), printer).str();
  serialize(exists(select(t.id).from(t)), printer).str();
  serialize(any(select(t.id).from(t)), printer).str();
  serialize(some(select(t.id).from(t)), printer).str();
  serialize(count(t.id), printer).str();
  serialize(min(t.id), printer).str();
  serialize(max(t.id), printer).str();
  serialize(avg(t.id), printer).str();
  serialize(sum(t.id), printer).str();
  serialize(sqlpp::verbatim_table("whatever"), printer).str();

  // alias
  serialize(t.as(t.id), printer).str();
  serialize(t.as(t.id).textN, printer).str();

  // select alias
  serialize(select(t.id).from(t).where(t.textN > "kaesekuchen").as(t.boolNn), printer).str();

  // Comparison to null
  static_assert(not sqlpp::can_be_null_t<decltype(t.id)>::value, "expected id cannot be null");
  static_assert(not sqlpp::can_be_null_t<decltype(f.textNnD)>::value, "expected intN cannot be null");
  serialize(t.id.is_null(), printer).str();
  serialize(f.textNnD.is_null(), printer).str();

  // join
  serialize(t.inner_join(t.as(t.id)).on(t.textN == t.as(t.id).textN), printer).str();
  {
    auto inner = t.inner_join(t.as(t.id)).on(t.textN == t.as(t.id).textN);
    serialize(select(t.id).from(inner), printer).str();
  }

  // distinct aggregate
  serialize(count(sqlpp::distinct, t.id % 7), printer).str();
  serialize(avg(sqlpp::distinct, t.id - 7), printer).str();
  serialize(sum(sqlpp::distinct, t.id + 7), printer).str();

  serialize(select(all_of(t)).from(t).unconditionally(), printer).str();

  for (const auto& row : db(select(all_of(t)).from(t).unconditionally()))
  {
    serialize(t.id == row.id, printer);
    serialize(t.textN == row.textN.value(), printer);
    serialize(t.boolNn == row.boolNn, printer);
  }

  get_sql_name(t);
  get_sql_name(t.id);

  flatten(t.id == 7, db);

  printer.reset();
  std::cerr << serialize(select(all_of(t)).from(t).where(t.id.in(select(f.intN).from(f).unconditionally())),
                         printer)
                   .str()
            << std::endl;

  printer.reset();
  std::cerr << serialize(select(all_of(t)).from(t).where(t.id.in()), printer).str() << std::endl;

  printer.reset();
  std::cerr << serialize(select(all_of(t)).from(t).where(t.id.not_in()), printer).str() << std::endl;

  auto schema = db.attach("lorem");
  auto s = schema_qualified_table(schema, t).as(sqlpp::alias::x);

  printer.reset();
  std::cerr << serialize(select(all_of(s)).from(s).unconditionally(), printer).str() << std::endl;

  printer.reset();
  std::cerr << serialize(sqlpp::case_when(true).then(t.id).else_(t.id + 1).as(t.textN), printer).str()
            << std::endl;

  return 0;
}

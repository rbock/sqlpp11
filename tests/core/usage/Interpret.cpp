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
  select(t.alpha.as(t.beta));

  serialize(insert_into(t).columns(t.beta, t.gamma), printer).str();
  {
    auto i = insert_into(t).columns(t.gamma, t.beta);
    i.add_values(t.gamma = true, t.beta = "cheesecake");
    serialize(i, printer).str();
    i.add_values(t.gamma = sqlpp::default_value, t.beta = sqlpp::null);
    serialize(i, printer).str();
  }

  serialize(t.alpha = sqlpp::null, printer).str();
  serialize(t.alpha = sqlpp::default_value, printer).str();
  serialize(t.alpha, printer).str();
  serialize(-t.alpha, printer).str();
  serialize(+t.alpha, printer).str();
  serialize(-(t.alpha + 7), printer).str();
  serialize(t.alpha = 0, printer).str();
  serialize(t.alpha == 0, printer).str();
  serialize(t.alpha != 0, printer).str();
  serialize(t.alpha == 7, printer).str();
  serialize(t.beta + "kaesekuchen", printer).str();

  serialize(sqlpp::select(), printer).str();
  serialize(sqlpp::select().flags(sqlpp::distinct), printer).str();
  serialize(select(t.alpha, t.beta).flags(sqlpp::distinct), printer).str();
  serialize(select(t.alpha, t.beta), printer).str();
  serialize(select(t.alpha, t.beta).from(t), printer).str();
  serialize(select(t.alpha, t.beta).from(t).where(t.alpha == 3), printer).str();
  serialize(select(t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma), printer).str();
  serialize(select(t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma).having(t.beta.like("%kuchen")),
            printer)
      .str();
  serialize(select(t.alpha, t.beta)
                .from(t)
                .where(t.alpha == 3)
                .group_by(t.gamma)
                .having(t.beta.like("%kuchen"))
                .order_by(t.beta.asc()),
            printer)
      .str();
  serialize(select(t.alpha, t.beta)
                .from(t)
                .where(t.alpha == 3)
                .group_by(t.gamma)
                .having(t.beta.like("%kuchen"))
                .order_by(t.beta.asc())
                .limit(17u)
                .offset(3u),
            printer)
      .str();

  serialize(parameter(sqlpp::bigint(), t.alpha), printer).str();
  serialize(parameter(t.alpha), printer).str();
  serialize(t.alpha == parameter(t.alpha), printer).str();
  serialize(t.alpha == parameter(t.alpha) and (t.beta + "gimmick").like(parameter(t.beta)), printer).str();

  serialize(insert_into(t), printer).str();
  serialize(insert_into(f).default_values(), printer).str();
  serialize(insert_into(t).set(t.gamma = true), printer).str();

  serialize(update(t), printer).str();
  serialize(update(t).set(t.gamma = true), printer).str();
  serialize(update(t).set(t.gamma = true).where(t.beta.in("kaesekuchen", "cheesecake")), printer).str();
  serialize(update(t).set(t.gamma = true).where(t.beta.in()), printer).str();

  serialize(remove_from(t), printer).str();
  serialize(remove_from(t).using_(t), printer).str();

  // functions
  serialize(sqlpp::value(7), printer).str();
  serialize(sqlpp::verbatim<sqlpp::integral>("irgendwas integrales"), printer).str();
  serialize(sqlpp::value_list(std::vector<int>({1, 2, 3, 4, 5, 6, 8})), printer).str();
  serialize(exists(select(t.alpha).from(t)), printer).str();
  serialize(any(select(t.alpha).from(t)), printer).str();
  serialize(some(select(t.alpha).from(t)), printer).str();
  serialize(count(t.alpha), printer).str();
  serialize(min(t.alpha), printer).str();
  serialize(max(t.alpha), printer).str();
  serialize(avg(t.alpha), printer).str();
  serialize(sum(t.alpha), printer).str();
  serialize(sqlpp::verbatim_table("whatever"), printer).str();

  // alias
  serialize(t.as(t.alpha), printer).str();
  serialize(t.as(t.alpha).beta, printer).str();

  // select alias
  serialize(select(t.alpha).from(t).where(t.beta > "kaesekuchen").as(t.gamma), printer).str();

  // Comparison to null
  static_assert(sqlpp::can_be_null_t<decltype(t.alpha)>::value, "expected alpha can be null");
  static_assert(not sqlpp::can_be_null_t<decltype(f.delta)>::value, "expected delta cannot be null");
  serialize(t.alpha.is_null(), printer).str();
  serialize(f.delta.is_null(), printer).str();

  // join
  serialize(t.inner_join(t.as(t.alpha)).on(t.beta == t.as(t.alpha).beta), printer).str();
  {
    auto inner = t.inner_join(t.as(t.alpha)).on(t.beta == t.as(t.alpha).beta);
    serialize(select(t.alpha).from(inner), printer).str();
  }

  // distinct aggregate
  serialize(count(sqlpp::distinct, t.alpha % 7), printer).str();
  serialize(avg(sqlpp::distinct, t.alpha - 7), printer).str();
  serialize(sum(sqlpp::distinct, t.alpha + 7), printer).str();

  serialize(select(all_of(t)).from(t).unconditionally(), printer).str();

  for (const auto& row : db(select(all_of(t)).from(t).unconditionally()))
  {
    serialize(t.alpha == row.alpha.value(), printer);
    serialize(t.beta == row.beta.value(), printer);
    serialize(t.gamma == row.gamma, printer);
  }

  get_sql_name(t);
  get_sql_name(t.alpha);

  flatten(t.alpha == 7, db);

  printer.reset();
  std::cerr << serialize(select(all_of(t)).from(t).where(t.alpha.in(select(f.epsilon).from(f).unconditionally())),
                         printer)
                   .str()
            << std::endl;

  printer.reset();
  std::cerr << serialize(select(all_of(t)).from(t).where(t.alpha.in()), printer).str() << std::endl;

  printer.reset();
  std::cerr << serialize(select(all_of(t)).from(t).where(t.alpha.not_in()), printer).str() << std::endl;

  auto schema = db.attach("lorem");
  auto s = schema_qualified_table(schema, t).as(sqlpp::alias::x);

  printer.reset();
  std::cerr << serialize(select(all_of(s)).from(s).unconditionally(), printer).str() << std::endl;

  printer.reset();
  std::cerr << serialize(sqlpp::case_when(true).then(t.alpha).else_(t.alpha + 1).as(t.beta), printer).str()
            << std::endl;

  return 0;
}

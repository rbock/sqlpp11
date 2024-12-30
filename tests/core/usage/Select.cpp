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

#include <sqlpp11/tests/core/MockDb.h>
#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/tests/core/result_helpers.h>
#include "is_regular.h"
#include <algorithm>
#include <iostream>
#include <sqlpp11/sqlpp11.h>

struct to_cerr
{
  template <typename Field>
  auto operator()(const Field& field) const -> void
  {
    std::cerr << field << std::endl;
  }
};

template <typename Row>
void print_row(Row const& row)
{
  const ::sqlpp::optional<int64_t> a = row.id;
  const ::sqlpp::optional<::sqlpp::string_view> b = row.textN;
  std::cout << a << ", " << b << std::endl;
}

SQLPP_CREATE_NAME_TAG(param2);
SQLPP_CREATE_NAME_TAG(cheese);
SQLPP_CREATE_NAME_TAG(average);
SQLPP_CREATE_NAME_TAG(N);


int Select(int, char*[])
{
  MockDb db = {};
  MockDb::_context_t printer = {};

  const auto f = test::TabFoo{};
  const auto t = test::TabBar{};
  const auto tab_a = f.as(sqlpp::alias::a);

  select(count(t.id).as(N));
  select(sqlpp::count(1).as(N));
  select(count(sqlpp::value(1)).as(N));

  std::cerr << to_sql_string(printer, select(sqlpp::value(false).as(sqlpp::alias::a))) << std::endl;
  for (const auto& row : db(select(sqlpp::value(false).as(sqlpp::alias::a))))
  {
    std::cout << row.a << std::endl;
  }

  {
    // using stl algorithms
    auto rows = db(select(all_of(t)).from(t).unconditionally());
    // nicer in C++14
    std::for_each(rows.begin(), rows.end(), &print_row<decltype(*rows.begin())>);
  }

  for (const auto& row : db(select(all_of(t)).from(t).unconditionally()))
  {
    const ::sqlpp::optional<int64_t> a = row.id;
    const ::sqlpp::optional<::sqlpp::string_view> b = row.textN;
    std::cout << a << ", " << b << std::endl;
  }

  for (const auto& row :
       db(select(all_of(t), t.boolNn.as(t)).from(t).where(t.id > 7 and trim(t.textN) == "test").for_update()))
  {
    const ::sqlpp::optional<int64_t> a = row.id;
    const ::sqlpp::optional<::sqlpp::string_view> b = row.textN;
    const bool g = row.tabBar;
    std::cout << a << ", " << b << ", " << g << std::endl;
  }

  for (const auto& row :
       db(select(all_of(t), f.textNnD).from(t.join(f).on(t.id > f.doubleN and not t.boolNn)).unconditionally()))
  {
    std::cout << row.id << std::endl;
  }

  for (const auto& row : db(select(all_of(t), f.textNnD)
                                .from(t.join(f).on(t.id > f.doubleN).join(tab_a).on(t.id == tab_a.doubleN))
                                .unconditionally()))
  {
    std::cout << row.id << std::endl;
  }

  for (const auto& row : db(select(sqlpp::count(1).as(N), avg(t.id).as(average)).from(t).unconditionally()))
  {
    std::cout << row.N << std::endl;
  }

  for (const auto& row : db(select(count(t.id).as(N), avg(t.id).as(average)).from(t).where(t.id == 0)))
  {
    std::cout << row.N << std::endl;
  }

  auto stat = sqlpp::select()
                  .columns(all_of(t))
                  .flags(sqlpp::all)
                  .from(t)
                  .where(t.id > 0)
                  .group_by(t.id)
                  .order_by(t.boolNn.asc())
                  .having(max(t.boolNn) > 0)
                  .offset(19u)
                  .limit(7u);
  std::cerr << to_sql_string(printer, stat) << std::endl;

  auto s = sqlpp::select()
               .columns(t.id)
               .flags(sqlpp::distinct)
               .from(t)
               .where(t.id > 3)
               .group_by(t.id)
               .order_by(t.id.asc())
               .having(sum(t.id) > parameter(t.intN))
               .limit(32u)
               .offset(7u);
#warning add tests for optional everything
  for (const auto& row : db(db.prepare(s)))
  {
    const ::sqlpp::optional<int64_t> a = row.id;
    std::cout << a << std::endl;
  }

  std::cerr << to_sql_string(printer, s) << std::endl;

  select(sqlpp::value(7).as(t.id));

  for (const auto& row :
       db(select(sqlpp::case_when(true).then(t.textN).else_(::sqlpp::nullopt).as(t.textN)).from(t).unconditionally()))
  {
    std::cerr << row.textN << std::endl;
  }

  for (const auto& row : db(select(all_of(t)).from(t).unconditionally()))
  {
    for_each_field(row, to_cerr{});
  }

  {
    auto transaction = start_transaction(db, sqlpp::isolation_level::read_committed);
    if (db._mock_data._last_isolation_level != sqlpp::isolation_level::read_committed)
    {
      std::cout << "Error: transaction isolation level does not match expected level" << std::endl;
    }
  }
  db.set_default_isolation_level(sqlpp::isolation_level::read_uncommitted);
  {
    auto transaction = start_transaction(db);
    if (db._mock_data._last_isolation_level != sqlpp::isolation_level::read_uncommitted)
    {
      std::cout << "Error: transaction isolation level does not match default level" << std::endl;
    }
  }

  // Move to type tests?
  for (const auto& row :
       db(select(f.doubleN, value(select(count(t.id).as(N)).from(t).unconditionally()).as(cheese)).from(f).unconditionally()))
  {
    std::cout << row.doubleN << " " << row.cheese << std::endl;
  }

  // checking #584
  auto abs = db.prepare(select(t.id).from(t).where(sqlpp::parameterized_verbatim<sqlpp::unsigned_integral>(
                 "ABS(field1 -", sqlpp::parameter(t.id), ")") <=
             sqlpp::parameter(sqlpp::unsigned_integral(), param2)));
  abs.params.id = 7;
  abs.params.param2 = 7;

  return 0;
}

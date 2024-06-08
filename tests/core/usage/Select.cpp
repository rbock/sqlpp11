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
#include "is_regular.h"
#include <algorithm>
#include <iostream>
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>
#include <sqlpp11/without_table_check.h>
#include "../../include/test_helpers.h"

template <typename Db, typename Column>
int64_t getColumn(Db&& db, const Column& column)
{
  auto result = db(select(column.as(sqlpp::alias::a)).from(column.table()).unconditionally());
  if (not result.empty() and result.front().a.has_value())
    return result.front().a.value();
  else
    return 0;
}

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
  const sqlpp::optional<int64_t> a = row.alpha;
  const sqlpp::optional<sqlpp::string_view> b = row.beta;
  std::cout << a << ", " << b << std::endl;
}

SQLPP_ALIAS_PROVIDER(cheese)

int Select(int, char*[])
{
  MockDb db = {};
  MockDb::_serializer_context_t printer = {};

  const auto f = test::TabFoo{};
  const auto t = test::TabBar{};
  const auto tab_a = f.as(sqlpp::alias::a);

  getColumn(db, t.alpha);
  select(count(t.alpha));

  std::cerr << serialize(select(sqlpp::value(false).as(sqlpp::alias::a)), printer).str() << std::endl;
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
    const sqlpp::optional<int64_t> a = row.alpha;
    const sqlpp::optional<sqlpp::string_view> b = row.beta;
    std::cout << a << ", " << b << std::endl;
  }

  for (const auto& row :
       db(select(all_of(t), t.gamma.as(t)).from(t).where(t.alpha > 7 and trim(t.beta) == "test").for_update()))
  {
    const sqlpp::optional<int64_t> a = row.alpha;
    const sqlpp::optional<sqlpp::string_view> b = row.beta;
    const bool g = row.tabBar;
    std::cout << a << ", " << b << ", " << g << std::endl;
  }

  for (const auto& row :
       db(select(all_of(t), all_of(f)).from(t.join(f).on(t.alpha > f.omega and not t.gamma)).unconditionally()))
  {
    std::cout << row.alpha << std::endl;
  }

  for (const auto& row : db(select(all_of(t), all_of(f))
                                .from(t.join(f).on(t.alpha > f.omega).join(tab_a).on(t.alpha == tab_a.omega))
                                .unconditionally()))
  {
    std::cout << row.alpha << std::endl;
  }

  for (const auto& row : db(select(count(t.alpha), avg(t.alpha)).from(t).unconditionally()))
  {
    std::cout << row.count << std::endl;
  }

  for (const auto& row : db(select(count(t.alpha), avg(t.alpha)).from(t).where(t.alpha == 0)))
  {
    std::cout << row.count << std::endl;
  }

  auto stat = sqlpp::select()
                  .columns(all_of(t))
                  .flags(sqlpp::all)
                  .from(t)
                  .where(t.alpha > 0)
                  .group_by(t.alpha)
                  .order_by(t.gamma.asc())
                  .having(t.gamma)
                  .offset(19u)
                  .limit(7u);
  printer.reset();
  std::cerr << serialize(stat, printer).str() << std::endl;

  auto s = sqlpp::select()
               .columns(t.alpha)
               .flags(sqlpp::distinct)
               .from(t)
               .where(t.alpha > 3)
               .group_by(t.alpha)
               .order_by(t.beta.asc())
               .having(sum(t.alpha) > parameter(t.delta))
               .limit(32u)
               .offset(7u);
#warning add tests for optional everything
  for (const auto& row : db(db.prepare(s)))
  {
    const sqlpp::optional<int64_t> a = row.alpha;
    std::cout << a << std::endl;
  }

  printer.reset();
  std::cerr << serialize(s, printer).str() << std::endl;

  select(sqlpp::value(7).as(t.alpha));

  for (const auto& row :
       db(select(sqlpp::case_when(true).then(t.beta).else_(sqlpp::null).as(t.beta)).from(t).unconditionally()))
  {
    std::cerr << row.beta << std::endl;
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

  for (const auto& row :
       db(select(f.omega, select(count(t.alpha)).from(t).unconditionally().as(cheese)).from(f).unconditionally()))
  {
    std::cout << row.omega << " " << row.cheese << std::endl;
  }

  return 0;
}

/*
 * Copyright (c) 2013-2015, Roland Bock
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
#include <iostream>
#include <sqlpp11/core/compat/string_view.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/core/clause/insert.h>
#include <sqlpp11/core/clause/select.h>

int Insert(int, char*[])
{
  MockDb db = {};
  MockDb::_serializer_context_t printer = {};
  const auto t = test::TabBar{};
  const auto tabDateTime = test::TabDateTime{};
  const auto u = test::TabFoo{};

  {
    using T = decltype(insert_into(t));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  {
    using T = decltype(insert_into(t).set(t.textN = "kirschauflauf"));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  db(insert_into(t).default_values());
  db(insert_into(t).set(t.boolNn = true, t.textN = "kirschauflauf"));
  db(insert_into(t).set(t.boolNn = false, t.textN = ::sqlpp::make_optional("pie"),
                        t.intN = ::sqlpp::nullopt));

  to_sql_string(printer, insert_into(t).default_values()).str();

  to_sql_string(printer, insert_into(t)).str();
  to_sql_string(printer, insert_into(t).set(t.boolNn = true, t.textN = "kirschauflauf")).str();
  to_sql_string(printer, insert_into(t).columns(t.boolNn, t.textN)).str();
  auto multi_insert = insert_into(t).columns(t.boolNn, t.textN, t.intN);
  multi_insert.add_values(t.boolNn = true, t.textN = "cheesecake", t.intN = 1);
  multi_insert.add_values(t.boolNn = false, t.textN = sqlpp::default_value,
                          t.intN = sqlpp::default_value);
  multi_insert.add_values(t.boolNn = true, t.textN = ::sqlpp::make_optional("pie"),
                          t.intN = ::sqlpp::nullopt);
  printer.reset();
  std::cerr << to_sql_string(printer, multi_insert).str() << std::endl;

  // Beware, you need exact types for inserted values in multi_insert
  insert_into(tabDateTime)
      .set(tabDateTime.timePointN = std::chrono::system_clock::now());

  auto multi_time_insert = insert_into(tabDateTime).columns(tabDateTime.timePointN);
  multi_time_insert.add_values(tabDateTime.timePointN = std::chrono::time_point_cast<std::chrono::microseconds>(
                                   std::chrono::system_clock::now()));

#warning add tests with optional

  db(multi_insert);

  auto values = [&t]() { return std::make_tuple(t.boolNn = true, t.textN = ::sqlpp::nullopt); };

  db(insert_into(t).set(t.boolNn = true, t.intN = sqlpp::verbatim<sqlpp::integral>("17+4")));
  db(insert_into(t).set(t.boolNn = true, t.intN = ::sqlpp::nullopt));
  db(insert_into(t).set(t.boolNn = true, t.intN = sqlpp::default_value));
  db(insert_into(t).set(t.boolNn = true, t.intN = 0));
  db(insert_into(t).set(values()));

  db(insert_into(t).set(t.boolNn = true, t.intN = 0, t.textN = select(u.textNnD).from(u).unconditionally()));

  auto prepared_insert = db.prepare(insert_into(t).set(t.boolNn = parameter(t.boolNn), t.intN = parameter(t.intN)));
  prepared_insert.params.boolNn = true;
  prepared_insert.params.intN = ::sqlpp::nullopt;
  prepared_insert.params.intN = 17;
  prepared_insert.params.intN = ::sqlpp::nullopt;
  prepared_insert.params.intN = ::sqlpp::make_optional(17);
  db(prepared_insert);

  auto prepared_insert_sv = db.prepare(insert_into(t).set(t.boolNn = parameter(t.boolNn), t.intN = parameter(t.intN), t.textN = parameter(t.textN)));
  prepared_insert_sv.params.boolNn = true;
  prepared_insert_sv.params.intN = 17;
  prepared_insert_sv.params.textN = ::sqlpp::string_view("string_view");;
  db(prepared_insert_sv);

  return 0;
}

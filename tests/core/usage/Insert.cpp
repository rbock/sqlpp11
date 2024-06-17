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
#if __cplusplus >= 201703L
#include <string_view>
#endif
#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/select.h>

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
    using T = decltype(insert_into(t).set(t.beta = "kirschauflauf"));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  {
    using T = decltype(dynamic_insert_into(db, t).dynamic_set());
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  db(insert_into(t).default_values());
  db(insert_into(t).set(t.gamma = true, t.beta = "kirschauflauf"));
  db(insert_into(t).set(t.gamma = sqlpp::default_value, t.beta = sqlpp::value_or_null("pie"),
                        t.delta = sqlpp::value_or_null<sqlpp::integer>(sqlpp::null)));

  serialize(insert_into(t).default_values(), printer).str();

  serialize(insert_into(t), printer).str();
  serialize(insert_into(t).set(t.gamma = true, t.beta = "kirschauflauf"), printer).str();
  serialize(insert_into(t).columns(t.gamma, t.beta), printer).str();
  auto multi_insert = insert_into(t).columns(t.gamma, t.beta, t.delta);
  multi_insert.values.add(t.gamma = true, t.beta = "cheesecake", t.delta = 1);
  multi_insert.values.add(t.gamma = sqlpp::default_value, t.beta = sqlpp::default_value,
                          t.delta = sqlpp::default_value);
  multi_insert.values.add(t.gamma = sqlpp::value_or_null(true), t.beta = sqlpp::value_or_null("pie"),
                          t.delta = sqlpp::value_or_null<sqlpp::integer>(sqlpp::null));
  printer.reset();
  std::cerr << serialize(multi_insert, printer).str() << std::endl;

  // Beware, you need exact types for inserted values in multi_insert
  insert_into(tabDateTime)
      .set(tabDateTime.colTimePoint = std::chrono::system_clock::now());

  auto multi_time_insert = insert_into(tabDateTime).columns(tabDateTime.colTimePoint);
  multi_time_insert.values.add(tabDateTime.colTimePoint = std::chrono::time_point_cast<std::chrono::microseconds>(
                                   std::chrono::system_clock::now()));

  auto i = dynamic_insert_into(db, t).dynamic_set();
  i.insert_list.add(t.beta = "kirschauflauf");
  printer.reset();
  std::cerr << serialize(i, printer).str() << std::endl;

  db(multi_insert);

  auto values = [&t]() { return std::make_tuple(t.gamma = true, t.beta = sqlpp::null); };

  db(insert_into(t).set(t.gamma = true, t.delta = sqlpp::verbatim<sqlpp::integer>("17+4")));
  db(insert_into(t).set(t.gamma = true, t.delta = sqlpp::null));
  db(insert_into(t).set(t.gamma = true, t.delta = sqlpp::default_value));
  db(insert_into(t).set(t.gamma = true, t.delta = 0));
  db(insert_into(t).set(values()));

  db(insert_into(t).set(t.gamma = true, t.delta = 0, t.beta = select(u.delta).from(u).unconditionally()));

  auto prepared_insert = db.prepare(insert_into(t).set(t.gamma = parameter(t.gamma), t.delta = parameter(t.delta)));
  prepared_insert.params.gamma = true;
  prepared_insert.params.delta = sqlpp::null;
  prepared_insert.params.delta = 17;
  prepared_insert.params.delta = sqlpp::value_or_null<sqlpp::integer>(sqlpp::null);
  prepared_insert.params.delta = sqlpp::value_or_null(17);
  db(prepared_insert);

#if __cplusplus >= 201703L
  auto prepared_insert_sv = db.prepare(insert_into(t).set(t.gamma = parameter(t.gamma), t.delta = parameter(t.delta), t.beta = parameter(t.beta)));
  prepared_insert_sv.params.gamma = true;
  prepared_insert_sv.params.delta = 17;
  std::string_view sv = "string_view";
  prepared_insert_sv.params.beta = sv;
  db(prepared_insert_sv);
#endif

  return 0;
}

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
#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>

int Insert(int, char* [])
{
  MockDb db = {};
  MockDb::_serializer_context_t printer = {};
  const auto t = test::TabBar{};
  // test::TabFoo f;

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

  serialize(insert_into(t).default_values(), printer).str();

  serialize(insert_into(t), printer).str();
  serialize(insert_into(t).set(t.gamma = true, t.beta = "kirschauflauf"), printer).str();
  serialize(insert_into(t).columns(t.gamma, t.beta), printer).str();
  auto multi_insert = insert_into(t).columns(t.gamma, t.beta, t.delta);
  multi_insert.values.add(t.gamma = true, t.beta = "cheesecake", t.delta = 1);
  multi_insert.values.add(t.gamma = sqlpp::default_value, t.beta = sqlpp::default_value,
                          t.delta = sqlpp::default_value);
  auto i = dynamic_insert_into(db, t).dynamic_set();
  i.insert_list.add(t.beta = "kirschauflauf");
  printer.reset();
  std::cerr << serialize(i, printer).str() << std::endl;

  db(multi_insert);

  db(insert_into(t).set(t.gamma = true, t.delta = sqlpp::verbatim<sqlpp::integer>("17+4")));
  db(insert_into(t).set(t.gamma = true, t.delta = sqlpp::null));
  db(insert_into(t).set(t.gamma = true, t.delta = sqlpp::default_value));
  db(insert_into(t).set(t.gamma = true, t.delta = sqlpp::tvin(0)));

  return 0;
}

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
#include <sqlpp11/sqlpp11.h>

int Update(int, char* [])
{
  MockDb db;
  MockDb::_serializer_context_t printer = {};

  const auto t = test::TabBar{};

  {
    using T = decltype(update(t));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  {
    using T = decltype(update(t).set(t.gamma = false).where(t.beta != "transparent"));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  {
    using T = decltype(dynamic_update(db, t).dynamic_set(t.gamma = false).dynamic_where());
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  serialize(update(t), printer).str();
  serialize(update(t).set(t.gamma = false), printer).str();
  serialize(update(t).set(t.gamma = false).where(t.beta != "transparent"), printer).str();
  serialize(update(t).set(t.beta = "opaque").where(t.beta != t.beta + "this is nonsense"), printer).str();
  auto u = dynamic_update(db, t).dynamic_set(t.gamma = false).dynamic_where();
  u.assignments.add(t.beta = "cannot update gamma a second time");
  u.where.add(t.gamma != false);
  printer.reset();
  std::cerr << serialize(u, printer).str() << std::endl;

  db(u);

  db(update(t).set(t.delta = sqlpp::verbatim<sqlpp::integer>("17+4")).unconditionally());
  db(update(t).set(t.delta = sqlpp::null).unconditionally());
  db(update(t).set(t.delta = sqlpp::default_value).unconditionally());

  db(update(t).set(t.delta += t.alpha * 2, t.beta += " and cake").unconditionally());

  return 0;
}

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

#include <iostream>
#include "Sample.h"
#include "MockDb.h"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/custom_query.h>

int CustomQuery(int, char* [])
{
  MockDb db = {};
  MockDb::_serializer_context_t printer = {};

  const auto f = test::TabFoo{};
  const auto t = test::TabBar{};

  // A void custom query
  printer.reset();
  auto x = custom_query(sqlpp::verbatim("PRAGMA writeable_schema = "), true);
  std::cerr << serialize(x, printer).str() << std::endl;
  db(x);

  // Syntactically, it is possible to use this void query as a prepared statement, too, not sure, whether this makes
  // sense very often...
  db(db.prepare(x));

  // A prepared custom select
  // The return type of the custom query is determined from the first argument which does have a return type, in this
  // case the select
  auto p = db.prepare(custom_query(select(all_of(t)).from(t), where(t.alpha > sqlpp::parameter(t.alpha))));
  p.params.alpha = 8;
  for (const auto& row : db(p))
  {
    std::cerr << row.alpha << std::endl;
  }

  // Create a custom "insert or ignore"
  db(custom_query(sqlpp::insert(), sqlpp::verbatim(" OR IGNORE"), into(t),
                  insert_set(t.beta = "sample", t.gamma = true)));

  // A custom (select ... into) with adjusted return type
  // The first argument with a return type is the select, but the custom query is really an insert. Thus, we tell it so.
  printer.reset();
  auto c = custom_query(select(all_of(t)).from(t), into(f)).with_result_type_of(insert_into(f));
  std::cerr << serialize(c, printer).str() << std::endl;
  auto i = db(c);
  static_assert(std::is_integral<decltype(i)>::value, "insert yields an integral value");

  return 0;
}

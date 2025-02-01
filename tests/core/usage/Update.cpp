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

#include <sqlpp11/tests/core/MockDb.h>
#include <sqlpp11/tests/core/tables.h>
#include "is_regular.h"
#include <iostream>
#include <sqlpp11/sqlpp11.h>

int Update(int, char*[])
{
  const auto maybe = true;
  MockDb db;
  MockDb::_context_t printer = {};

  const auto t = test::TabBar{};

  {
    using T = decltype(update(t));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  {
    using T = decltype(update(t).set(t.boolNn = false).where(t.textN != "transparent"));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  to_sql_string(printer, update(t));
  to_sql_string(printer, update(t).set(t.boolNn = false));
  to_sql_string(printer, update(t).set(t.boolNn = false).where(t.textN != "transparent"));
  to_sql_string(printer, update(t).set(t.textN = "opaque").where(t.textN != t.textN + "this is nonsense"));
  to_sql_string(printer, update(t).set(t.textN = "opaque").where(t.textN != t.textN + "this is nonsense"));

  to_sql_string(printer, update(t).set(dynamic(maybe, t.textN = "opaque")).where(dynamic(maybe, t.textN != t.textN + "this is nonsense")));

  db(update(t).set(t.intN = sqlpp::verbatim<sqlpp::integral>("17+4")).unconditionally());
  db(update(t)
         .set(t.intN = sqlpp::verbatim<sqlpp::integral>("17+4"))
         .where(sqlpp::verbatim<sqlpp::text>("'hansi'") == "hansi"));
  db(update(t).set(t.intN = ::sqlpp::nullopt).unconditionally());
  db(update(t).set(t.intN = sqlpp::default_value).unconditionally());

  db(update(t).set(t.intN += t.id * 2, t.textN += " and cake").unconditionally());
  db(update(t).set(t.intN += t.id * 2, dynamic(maybe, t.textN += " and cake")).unconditionally());

  return 0;
}

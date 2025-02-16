/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/tests/core/serialize_helpers.h>
#include <sqlpp11/sqlpp11.h>

namespace
{
  SQLPP_CREATE_NAME_TAG(something);
}

int main(int, char* [])
{
  const auto val = sqlpp::value(17);
  const auto expr = sqlpp::value(17) + 4;

  const auto foo = test::TabFoo{};

  // Empty.
  SQLPP_COMPARE(sqlpp::select(), "SELECT ");

  // SELECT a value.
  SQLPP_COMPARE(sqlpp::select(expr.as(foo.id)), "SELECT (17 + 4) AS id");

  // SELECT FROM.
  SQLPP_COMPARE(select(foo.id).from(foo).where(true), "SELECT tab_foo.id FROM tab_foo");
  SQLPP_COMPARE(select(foo.id, foo.textNnD).from(foo).where(true), "SELECT tab_foo.id, tab_foo.text_nn_d FROM tab_foo");
  SQLPP_COMPARE(sqlpp::select().columns(foo.id).from(foo).where(true), "SELECT tab_foo.id FROM tab_foo");
  SQLPP_COMPARE(sqlpp::select().flags(sqlpp::all).columns(foo.id).from(foo).where(true), "SELECT ALL tab_foo.id FROM tab_foo");

  // SELECT FROM WHERE.
  SQLPP_COMPARE(select(foo.id).from(foo).where(true), "SELECT tab_foo.id FROM tab_foo WHERE 1");
  SQLPP_COMPARE(select(foo.id).from(foo).where(foo.intN > 17), "SELECT tab_foo.id FROM tab_foo WHERE tab_foo.int_n > 17");
  SQLPP_COMPARE(select(foo.id).from(foo).where(dynamic(false,foo.intN > 17)), "SELECT tab_foo.id FROM tab_foo");

  // SELECT FROM WHERE GROUP BY HAVING.
  SQLPP_COMPARE(select(count(foo.id).as(something)).from(foo).where(true).group_by(foo.intN).having(max(foo.id) < 100), "SELECT COUNT(tab_foo.id) AS something FROM tab_foo WHERE 1 GROUP BY tab_foo.int_n HAVING MAX(tab_foo.id) < 100");

  // SELECT FROM WHERE GROUP BY HAVING ORDER BY LIMIT OFFSET
  SQLPP_COMPARE(select(count(foo.id).as(something)).from(foo).where(true).group_by(foo.intN).having(max(foo.id) < 100).order_by(foo.intN.asc()).limit(10).offset(3), "SELECT COUNT(tab_foo.id) AS something FROM tab_foo WHERE 1 GROUP BY tab_foo.int_n HAVING MAX(tab_foo.id) < 100 ORDER BY tab_foo.int_n ASC LIMIT 10 OFFSET 3");

  return 0;
}

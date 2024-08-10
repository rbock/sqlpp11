/*
 * Copyright (c) 2016-2022, Roland Bock
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

#include "Sample.h"
#include "compare.h"
#include <sqlpp11/sqlpp11.h>

#include <iostream>

SQLPP_ALIAS_PROVIDER(pragma);

int CustomQuery(int, char*[])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Unconditionally
  SQLPP_COMPARE(custom_query(sqlpp::select(), select_flags(sqlpp::distinct), select_columns(foo.doubleN), from(foo),
                             sqlpp::unconditionally()),
                "SELECT  DISTINCT  tab_foo.double_n  FROM tab_foo ");

  // A full select statement made individual clauses
  SQLPP_COMPARE(
      custom_query(sqlpp::select(), select_flags(sqlpp::distinct), select_columns(foo.doubleN),
                   from(foo.join(bar).on(foo.doubleN == bar.id)), where(bar.id > 17), group_by(foo.doubleN),
                   having(avg(bar.id) > 19), order_by(foo.doubleN.asc()), sqlpp::limit(10u), sqlpp::offset(100u)),
      "SELECT  DISTINCT  tab_foo.double_n  FROM tab_foo INNER JOIN tab_bar ON tab_foo.double_n = tab_bar.id  WHERE "
      "tab_bar.id > 17  GROUP BY tab_foo.double_n  HAVING AVG(tab_bar.id) > 19  ORDER BY tab_foo.double_n ASC  "
      "LIMIT 10  OFFSET 100");

  // A full select statement made individual clauses
  SQLPP_COMPARE(
      custom_query(sqlpp::select(), select_flags(sqlpp::distinct), select_columns(foo.doubleN),
                   from(foo.join(bar).on(foo.doubleN == bar.id)), where(bar.id > 17), group_by(foo.doubleN),
                   having(avg(bar.id) > 19), order_by(foo.doubleN.asc(), foo.uIntN.order(sqlpp::sort_type::desc)),
                   sqlpp::limit(7u), sqlpp::offset(3u)),
      "SELECT  DISTINCT  tab_foo.double_n  FROM tab_foo INNER JOIN tab_bar ON tab_foo.double_n = tab_bar.id  WHERE "
      "tab_bar.id > 17  GROUP BY tab_foo.double_n  HAVING AVG(tab_bar.id) > 19  ORDER BY tab_foo.double_n "
      "ASC,tab_foo.u_int_n DESC  LIMIT 7  OFFSET 3");

  // A pragma query/query for sqlite
  SQLPP_COMPARE(
      custom_query(sqlpp::verbatim("PRAGMA user_version")).with_result_type_of(select(sqlpp::value(1).as(pragma))),
      " PRAGMA user_version");

  // An insert from select for postgresql
  const auto x = 17;
  SQLPP_COMPARE(custom_query(insert_into(foo).columns(foo.doubleN),
                             select(sqlpp::value(x).as(foo.doubleN))
                                 .from(foo)
                                 .where(not exists(select(foo.doubleN).from(foo).where(foo.doubleN == x)))),
                "INSERT INTO tab_foo (double_n) "
                "SELECT 17 AS double_n FROM tab_foo "
                "WHERE NOT EXISTS (SELECT tab_foo.double_n FROM tab_foo WHERE tab_foo.double_n = 17)");

#warning : reactivate
#if 0
  // A multi-row "insert or ignore"
  auto batch = insert_columns(bar.textN, bar.boolNn);
  batch.add_values(bar.textN = "sample", bar.boolNn = true);
  batch.add_values(bar.textN = "ample", bar.boolNn = false);
  SQLPP_COMPARE( custom_query(sqlpp::insert(), sqlpp::verbatim(" OR IGNORE"), into(bar), batch),
          "INSERT  OR IGNORE  INTO tab_bar  (text_n,bool_nn) VALUES ('sample',1),('ample',0)");
#endif

  return 0;
}

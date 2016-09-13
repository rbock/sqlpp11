/*
 * Copyright (c) 2016-2016, Roland Bock
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

SQLPP_ALIAS_PROVIDER(pragma)

int CustomQuery(int, char* [])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};
  auto db = MockDb{};

  // Unconditionally
  compare(__LINE__, custom_query(sqlpp::select(), select_flags(sqlpp::distinct), select_columns(foo.omega), from(foo),
                                 sqlpp::unconditionally()),
          "SELECT  DISTINCT  tab_foo.omega  FROM tab_foo ");

  // A full select statement made individual clauses
  compare(__LINE__,
          custom_query(sqlpp::select(), select_flags(sqlpp::distinct), select_columns(foo.omega),
                       from(foo.join(bar).on(foo.omega == bar.alpha)), where(bar.alpha > 17), group_by(foo.omega),
                       having(avg(bar.alpha) > 19), order_by(foo.omega.asc()), sqlpp::limit(10u), sqlpp::offset(100u)),
          "SELECT  DISTINCT  tab_foo.omega  FROM tab_foo INNER JOIN tab_bar ON (tab_foo.omega=tab_bar.alpha)  WHERE "
          "(tab_bar.alpha>17)  GROUP BY tab_foo.omega  HAVING (AVG(tab_bar.alpha)>19)  ORDER BY tab_foo.omega ASC  "
          "LIMIT 10  OFFSET 100");

  // A full select statement made individual clauses
  compare(
      __LINE__,
      custom_query(sqlpp::select(), dynamic_select_flags(db, sqlpp::distinct), dynamic_select_columns(db, foo.omega),
                   dynamic_from(db, foo.join(bar).on(foo.omega == bar.alpha)), dynamic_where(db, bar.alpha > 17),
                   dynamic_group_by(db, foo.omega), dynamic_having(db, avg(bar.alpha) > 19),
                   dynamic_order_by(db, foo.omega.asc()), sqlpp::dynamic_limit(db), sqlpp::dynamic_offset(db)),
      "SELECT  DISTINCT  tab_foo.omega  FROM tab_foo INNER JOIN tab_bar ON (tab_foo.omega=tab_bar.alpha)  WHERE "
      "(tab_bar.alpha>17)  GROUP BY tab_foo.omega  HAVING (AVG(tab_bar.alpha)>19)  ORDER BY tab_foo.omega ASC  ");

  // A pragma query for sqlite
  compare(__LINE__,
          custom_query(sqlpp::verbatim("PRAGMA user_version")).with_result_type_of(select(sqlpp::value(1).as(pragma))),
          " PRAGMA user_version");

  return 0;
}

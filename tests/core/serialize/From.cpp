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

namespace
{
  MockDb db = {};
}

int From(int, char* [])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};
  const auto aFoo = foo.as(sqlpp::alias::a);
  const auto bFoo = foo.as(sqlpp::alias::b);
  const auto cFoo = foo.as(sqlpp::alias::c);

  // Single table
  SQLPP_COMPARE(from(foo), " FROM tab_foo");
  SQLPP_COMPARE(from(bar), " FROM tab_bar");

  // Static joins
  SQLPP_COMPARE(from(foo.cross_join(bar)), " FROM tab_foo CROSS JOIN tab_bar");
  SQLPP_COMPARE(from(foo.join(bar).on(foo.doubleN > bar.id)),
          " FROM tab_foo INNER JOIN tab_bar ON tab_foo.double_n > tab_bar.id");
  SQLPP_COMPARE(from(foo.inner_join(bar).on(foo.doubleN > bar.id)),
          " FROM tab_foo INNER JOIN tab_bar ON tab_foo.double_n > tab_bar.id");
  SQLPP_COMPARE(from(foo.full_outer_join(bar).on(foo.doubleN > bar.id)),
          " FROM tab_foo OUTER JOIN tab_bar ON tab_foo.double_n > tab_bar.id");
  SQLPP_COMPARE(from(foo.left_outer_join(bar).on(foo.doubleN > bar.id)),
          " FROM tab_foo LEFT OUTER JOIN tab_bar ON tab_foo.double_n > tab_bar.id");
  SQLPP_COMPARE(from(foo.right_outer_join(bar).on(foo.doubleN > bar.id)),
          " FROM tab_foo RIGHT OUTER JOIN tab_bar ON tab_foo.double_n > tab_bar.id");
  SQLPP_COMPARE(from(aFoo.join(bFoo).on(aFoo.doubleN > bFoo.doubleN)),
          " FROM tab_foo AS a INNER JOIN tab_foo AS b ON a.double_n > b.double_n");
  /*
  SQLPP_COMPARE(from(aFoo.join(bFoo).on(aFoo.doubleN > bFoo.doubleN).join(cFoo).on(bFoo.doubleN > cFoo.doubleN)),
      " FROM tab_foo AS a INNER JOIN tab_foo AS b ON a.double_n > b.double_n INNER JOIN tab_foo AS c ON b.double_n > c.double_n");
  SQLPP_COMPARE(from(foo.join(bar).unconditionally()), " FROM tab_foo INNER JOIN tab_bar");

  // Static joins involving verbatim tables
  SQLPP_COMPARE(from(aFoo.join(sqlpp::verbatim_table("unknown_table"))
                             .on(aFoo.doubleN > sqlpp::verbatim<sqlpp::floating_point>("unknown_table.column_x"))),
          " FROM tab_foo AS a INNER JOIN unknown_table ON a.double_n > unknown_table.column_x");
  SQLPP_COMPARE(from(sqlpp::verbatim_table("unknown_table")
                             .join(aFoo)
                             .on(aFoo.doubleN > sqlpp::verbatim<sqlpp::floating_point>("unknown_table.column_x"))),
          " FROM unknown_table INNER JOIN tab_foo AS a ON a.double_n > unknown_table.column_x");
  SQLPP_COMPARE(from(sqlpp::verbatim_table("unknown_table")
                             .as(sqlpp::alias::a)
                             .join(sqlpp::verbatim_table("another_table"))
                             .on(sqlpp::verbatim<sqlpp::boolean>("a.column_x > another_table.x"))),
          " FROM unknown_table AS a INNER JOIN another_table ON a.column_x > another_table.x");

#warning add tests for dynamic joins
*/
  return 0;
}

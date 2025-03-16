/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp23/sqlpp23.h>
#include <sqlpp23/tests/core/serialize_helpers.h>
#include <sqlpp23/tests/core/tables.h>

SQLPP_CREATE_NAME_TAG(something);

int main(int, char*[]) {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  const auto aFoo = foo.as(sqlpp::alias::a);
  const auto bFoo = foo.as(sqlpp::alias::b);

  // Join with two tables
  SQLPP_COMPARE(foo.join(bar).on(foo.id == bar.id),
                "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.inner_join(bar).on(foo.id == bar.id),
                "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.left_outer_join(bar).on(foo.id == bar.id),
                "tab_foo LEFT OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.right_outer_join(bar).on(foo.id == bar.id),
                "tab_foo RIGHT OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.full_outer_join(bar).on(foo.id == bar.id),
                "tab_foo FULL OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");

  SQLPP_COMPARE(foo.cross_join(bar), "tab_foo CROSS JOIN tab_bar");

  // Join with table aliases.
  SQLPP_COMPARE(foo.join(bFoo).on(foo.id == bFoo.id),
                "tab_foo INNER JOIN tab_foo AS b ON tab_foo.id = b.id");
  SQLPP_COMPARE(aFoo.join(foo).on(aFoo.id == foo.id),
                "tab_foo AS a INNER JOIN tab_foo ON a.id = tab_foo.id");
  SQLPP_COMPARE(aFoo.join(bFoo).on(aFoo.id == bFoo.id),
                "tab_foo AS a INNER JOIN tab_foo AS b ON a.id = b.id");

  // Static joins involving verbatim tables
  SQLPP_COMPARE(
      aFoo.join(sqlpp::verbatim_table("unknown_table"))
          .on(aFoo.id ==
              sqlpp::verbatim<sqlpp::floating_point>("unknown_table.column_x")),
      "tab_foo AS a INNER JOIN unknown_table ON a.id = unknown_table.column_x");
  SQLPP_COMPARE(
      sqlpp::verbatim_table("unknown_table")
          .join(aFoo)
          .on(aFoo.id ==
              sqlpp::verbatim<sqlpp::floating_point>("unknown_table.column_x")),
      "unknown_table INNER JOIN tab_foo AS a ON a.id = unknown_table.column_x");
  SQLPP_COMPARE(
      sqlpp::verbatim_table("unknown_table")
          .as(sqlpp::alias::a)
          .join(sqlpp::verbatim_table("another_table"))
          .on(sqlpp::verbatim<sqlpp::boolean>("a.column_x = another_table.x")),
      "unknown_table AS a INNER JOIN another_table ON a.column_x = "
      "another_table.x");

  // Join with dynamic table
  SQLPP_COMPARE(foo.join(dynamic(true, bar)).on(foo.id == bar.id),
                "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.inner_join(dynamic(true, bar)).on(foo.id == bar.id),
                "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.left_outer_join(dynamic(true, bar)).on(foo.id == bar.id),
                "tab_foo LEFT OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.right_outer_join(dynamic(true, bar)).on(foo.id == bar.id),
                "tab_foo RIGHT OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.full_outer_join(dynamic(true, bar)).on(foo.id == bar.id),
                "tab_foo FULL OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");

  SQLPP_COMPARE(foo.cross_join(dynamic(true, bar)),
                "tab_foo CROSS JOIN tab_bar");

  SQLPP_COMPARE(foo.join(dynamic(false, bar)).on(foo.id == bar.id), "tab_foo");
  SQLPP_COMPARE(foo.inner_join(dynamic(false, bar)).on(foo.id == bar.id),
                "tab_foo");
  SQLPP_COMPARE(foo.left_outer_join(dynamic(false, bar)).on(foo.id == bar.id),
                "tab_foo");
  SQLPP_COMPARE(foo.right_outer_join(dynamic(false, bar)).on(foo.id == bar.id),
                "tab_foo");
  SQLPP_COMPARE(foo.full_outer_join(dynamic(false, bar)).on(foo.id == bar.id),
                "tab_foo");

  SQLPP_COMPARE(foo.cross_join(dynamic(false, bar)), "tab_foo");

  // Joining three tables
  SQLPP_COMPARE(
      foo.join(bar).on(foo.id == bar.id).join(aFoo).on(bar.id == aFoo.id),
      "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id INNER JOIN "
      "tab_foo AS a ON tab_bar.id = a.id");
  SQLPP_COMPARE(foo.join(bar)
                    .on(foo.id == bar.id)
                    .join(dynamic(true, aFoo))
                    .on(bar.id == aFoo.id),
                "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id INNER "
                "JOIN tab_foo AS a ON tab_bar.id = a.id");
  SQLPP_COMPARE(foo.join(bar)
                    .on(foo.id == bar.id)
                    .join(dynamic(false, aFoo))
                    .on(bar.id == aFoo.id),
                "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id");

  SQLPP_COMPARE(foo.join(dynamic(true, bar))
                    .on(foo.id == bar.id)
                    .join(aFoo)
                    .on(foo.id == aFoo.id),
                "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id INNER "
                "JOIN tab_foo AS a ON tab_foo.id = a.id");
  SQLPP_COMPARE(foo.join(dynamic(false, bar))
                    .on(foo.id == bar.id)
                    .join(aFoo)
                    .on(foo.id == aFoo.id),
                "tab_foo INNER JOIN tab_foo AS a ON tab_foo.id = a.id");

  // Joining sub selects
  const auto s = select(all_of(foo)).from(foo).where(true).as(something);
  auto ctx = MockDb::_context_t{};
  const auto s_string = to_sql_string(ctx, s);

  SQLPP_COMPARE(foo.cross_join(s), "tab_foo CROSS JOIN " + s_string);
  SQLPP_COMPARE(foo.cross_join(dynamic(true, s)),
                "tab_foo CROSS JOIN " + s_string);
  SQLPP_COMPARE(s.cross_join(foo), s_string + " CROSS JOIN tab_foo");
  SQLPP_COMPARE(s.cross_join(dynamic(true, foo)),
                s_string + " CROSS JOIN tab_foo");

  // Joining sub ctes
  const auto c_ref = cte(something);
  const auto c = c_ref.as(select(all_of(foo)).from(foo).where(true));
  const auto c_string = to_sql_string(ctx, c_ref);

  SQLPP_COMPARE(foo.cross_join(c), "tab_foo CROSS JOIN " + c_string);
  SQLPP_COMPARE(foo.cross_join(dynamic(true, c)),
                "tab_foo CROSS JOIN " + c_string);
  SQLPP_COMPARE(c.cross_join(foo), c_string + " CROSS JOIN tab_foo");
  SQLPP_COMPARE(c.cross_join(dynamic(true, foo)),
                c_string + " CROSS JOIN tab_foo");

  return 0;
}

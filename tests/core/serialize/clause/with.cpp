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

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/tests/core/serialize_helpers.h>
#include <sqlpp11/tests/core/tables.h>

int main(int, char *[]) {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // No expression (not super useful).
  SQLPP_COMPARE(cte(sqlpp::alias::x), "x");

  // WITH simple CTE: X AS SELECT
  {
    const auto x =
        cte(sqlpp::alias::x).as(select(foo.id).from(foo).where(true));
    SQLPP_COMPARE(with(x),
                  "WITH x AS (SELECT tab_foo.id FROM tab_foo WHERE 1) ");
  }

  // WITH non-recursive union CTE: X AS SELECT ... UNION ALL SELECT ...
  {
    const auto x = cte(sqlpp::alias::x)
                       .as(select(foo.id).from(foo).where(true).union_all(
                           select(bar.id).from(bar).where(true)));
    SQLPP_COMPARE(with(x),
                  "WITH x AS (SELECT tab_foo.id FROM tab_foo WHERE 1 UNION ALL "
                  "SELECT tab_bar.id FROM tab_bar WHERE 1) ");
  }

  // WITH recursive union CTE: X AS SELECT ... UNION ALL SELECT ... FROM X ...
  {
    const auto x_base =
        cte(sqlpp::alias::x).as(select(sqlpp::value(0).as(sqlpp::alias::a)));
    const auto x = x_base.union_all(select((x_base.a + 1).as(sqlpp::alias::a))
                                        .from(x_base)
                                        .where(x_base.a < 10));

    SQLPP_COMPARE(with(x), "WITH RECURSIVE x AS (SELECT 0 AS a UNION ALL "
                           "SELECT (x.a + 1) AS a FROM x WHERE x.a < 10) ");
  }

  // WITH two CTEs, no recursive
  {
    const auto x =
        cte(sqlpp::alias::x).as(select(foo.id).from(foo).where(true));
    const auto y =
        cte(sqlpp::alias::y).as(select(foo.id).from(foo).where(true));

    SQLPP_COMPARE(with(x, y),
                  "WITH x AS (SELECT tab_foo.id FROM tab_foo WHERE 1), y AS "
                  "(SELECT tab_foo.id FROM tab_foo WHERE 1) ");
    SQLPP_COMPARE(with(y, x),
                  "WITH y AS (SELECT tab_foo.id FROM tab_foo WHERE 1), x AS "
                  "(SELECT tab_foo.id FROM tab_foo WHERE 1) ");
  }

  // WITH two CTEs, one of them recursive
  {
    const auto x_base =
        cte(sqlpp::alias::x).as(select(sqlpp::value(0).as(sqlpp::alias::a)));
    const auto x = x_base.union_all(select((x_base.a + 1).as(sqlpp::alias::a))
                                        .from(x_base)
                                        .where(x_base.a < 10));
    const auto y =
        cte(sqlpp::alias::y).as(select(foo.id).from(foo).where(true));

    SQLPP_COMPARE(with(x, y), "WITH RECURSIVE x AS (SELECT 0 AS a UNION ALL "
                              "SELECT (x.a + 1) AS a FROM x WHERE x.a < 10), y "
                              "AS (SELECT tab_foo.id FROM tab_foo WHERE 1) ");
    SQLPP_COMPARE(with(y, x), "WITH RECURSIVE y AS (SELECT tab_foo.id FROM "
                              "tab_foo WHERE 1), x AS (SELECT 0 AS a UNION ALL "
                              "SELECT (x.a + 1) AS a FROM x WHERE x.a < 10) ");
  }

  // WITH two CTEs, second depends on first
  {
    const auto x =
        cte(sqlpp::alias::x).as(select(foo.id).from(foo).where(true));
    const auto y = cte(sqlpp::alias::y).as(select(x.id).from(x).where(true));

    SQLPP_COMPARE(with(x, y), "WITH x AS (SELECT tab_foo.id FROM tab_foo WHERE "
                              "1), y AS (SELECT x.id FROM x WHERE 1) ");
  }

  return 0;
}

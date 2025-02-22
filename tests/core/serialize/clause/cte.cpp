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

int main(int, char *[]) {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // No expression (not super useful).
  SQLPP_COMPARE(cte(sqlpp::alias::x), "x");

  // Simple CTE: X AS SELECT
  {
    using S = decltype(select(foo.id).from(foo).where(true));
    static_assert(sqlpp::has_result_row<S>::value, "");
    const auto x =
        cte(sqlpp::alias::x).as(select(foo.id).from(foo).where(true));
    const auto a = x.as(sqlpp::alias::a);
    SQLPP_COMPARE(x, "x AS (SELECT tab_foo.id FROM tab_foo WHERE 1)");
    SQLPP_COMPARE(make_table_ref(x), "x");
    SQLPP_COMPARE(x.id, "x.id");
    SQLPP_COMPARE(a, "x AS a");
    SQLPP_COMPARE(a.id, "a.id");
    SQLPP_COMPARE(select(all_of(x)), "SELECT x.id");
    SQLPP_COMPARE(select(all_of(a)), "SELECT a.id");
  }

  // Non-recursive union CTE: X AS SELECT ... UNION ALL SELECT ...
  {
    const auto x = cte(sqlpp::alias::x)
                       .as(select(foo.id).from(foo).where(true).union_all(
                           select(bar.id).from(bar).where(true)));
    const auto a = x.as(sqlpp::alias::a);
    SQLPP_COMPARE(x, "x AS (SELECT tab_foo.id FROM tab_foo WHERE 1 UNION ALL "
                     "SELECT tab_bar.id FROM tab_bar WHERE 1)");
    SQLPP_COMPARE(make_table_ref(x), "x");
    SQLPP_COMPARE(x.id, "x.id");
    SQLPP_COMPARE(a, "x AS a");
    SQLPP_COMPARE(a.id, "a.id");
    SQLPP_COMPARE(select(all_of(x)), "SELECT x.id");
    SQLPP_COMPARE(select(all_of(a)), "SELECT a.id");
  }

  // Recursive CTE: X AS SELECT ... UNION ALL SELECT ... FROM X ...
  {
    const auto x_base =
        cte(sqlpp::alias::x).as(select(sqlpp::value(0).as(sqlpp::alias::a)));
    const auto x = x_base.union_all(select((x_base.a + 1).as(sqlpp::alias::a))
                                        .from(x_base)
                                        .where(x_base.a < 10));
    const auto y = x.as(sqlpp::alias::y);
    SQLPP_COMPARE(x, "x AS (SELECT 0 AS a UNION ALL SELECT (x.a + 1) AS a FROM "
                     "x WHERE x.a < 10)");
    SQLPP_COMPARE(make_table_ref(x), "x");
    SQLPP_COMPARE(x.a, "x.a");
    SQLPP_COMPARE(y, "x AS y");
    SQLPP_COMPARE(y.a, "y.a");
    SQLPP_COMPARE(select(all_of(x)), "SELECT x.a");
    SQLPP_COMPARE(select(all_of(y)), "SELECT y.a");
  }

  // A CTE depending on another CTE
  {
    const auto x =
        cte(sqlpp::alias::x).as(select(foo.id).from(foo).where(true));
    const auto y = cte(sqlpp::alias::y)
                       .as(select(x.id, sqlpp::value(7).as(sqlpp::alias::a))
                               .from(x)
                               .where(true));
    const auto z = y.as(sqlpp::alias::z);
    SQLPP_COMPARE(y, "y AS (SELECT x.id, 7 AS a FROM x WHERE 1)");
    SQLPP_COMPARE(make_table_ref(y), "y");
    SQLPP_COMPARE(y.id, "y.id");
    SQLPP_COMPARE(z, "y AS z");
    SQLPP_COMPARE(z.id, "z.id");
    SQLPP_COMPARE(select(all_of(y)), "SELECT y.id, y.a");
    SQLPP_COMPARE(select(all_of(z)), "SELECT z.id, z.a");
  }

  // Dynamically recursive CTE: X AS SELECT ... UNION ALL SELECT ... FROM X ...
  {
    const auto x_base =
        cte(sqlpp::alias::x).as(select(sqlpp::value(0).as(sqlpp::alias::a)));
    auto x = x_base.union_all(
        dynamic(true, select((x_base.a + 1).as(sqlpp::alias::a))
                          .from(x_base)
                          .where(x_base.a < 10)));

    SQLPP_COMPARE(x, "x AS (SELECT 0 AS a UNION ALL SELECT (x.a + 1) AS a FROM "
                     "x WHERE x.a < 10)");

    x = x_base.union_all(
        dynamic(false, select((x_base.a + 1).as(sqlpp::alias::a))
                           .from(x_base)
                           .where(x_base.a < 10)));
    SQLPP_COMPARE(x, "x AS (SELECT 0 AS a)");
  }

  // Dynamically recursive CTE: X AS SELECT ... UNION DISTINCT SELECT ... FROM X
  // ...
  {
    const auto x_base =
        cte(sqlpp::alias::x).as(select(sqlpp::value(0).as(sqlpp::alias::a)));
    auto x = x_base.union_distinct(
        dynamic(true, select((x_base.a + 1).as(sqlpp::alias::a))
                          .from(x_base)
                          .where(x_base.a < 10)));

    SQLPP_COMPARE(x, "x AS (SELECT 0 AS a UNION DISTINCT SELECT (x.a + 1) AS a "
                     "FROM x WHERE x.a < 10)");

    x = x_base.union_distinct(
        dynamic(false, select((x_base.a + 1).as(sqlpp::alias::a))
                           .from(x_base)
                           .where(x_base.a < 10)));
    SQLPP_COMPARE(x, "x AS (SELECT 0 AS a)");
  }

  return 0;
}

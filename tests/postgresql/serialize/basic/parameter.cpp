/*
 * Copyright (c) 2025, Roland Bock
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

#include <sqlpp23/postgresql/postgresql.h>
#include <sqlpp23/sqlpp23.h>

#include <sqlpp23/tests/postgresql/serialize_helpers.h>
#include <sqlpp23/tests/postgresql/tables.h>

namespace {
SQLPP_CREATE_NAME_TAG(something);
SQLPP_CREATE_NAME_TAG(other);
} // namespace

int main() {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  SQLPP_COMPARE(parameter(foo.doubleN), "$1");
  SQLPP_COMPARE(bar.id > parameter(foo.doubleN), "tab_bar.id > $1");

  SQLPP_COMPARE(parameter(sqlpp::integral{}, something), "$1");

  SQLPP_COMPARE(parameter(sqlpp::integral{}, something) >
                    parameter(sqlpp::integral{}, other),
                "$1 > $2");
  SQLPP_COMPARE(parameter(sqlpp::integral{}, something) +
                    parameter(sqlpp::integral{}, other),
                "$1 + $2");
  SQLPP_COMPARE(parameter(sqlpp::integral{}, something) |
                    parameter(sqlpp::integral{}, other),
                "$1 | $2");
  SQLPP_COMPARE(parameter(sqlpp::integral{}, something)
                    .between(parameter(sqlpp::integral{}, other),
                             parameter(sqlpp::integral{}, sqlpp::alias::a)),
                "$1 BETWEEN $2 AND $3");

  SQLPP_COMPARE(parameter(sqlpp::integral{}, something) +
                    parameter(sqlpp::integral{}, other) +
                    parameter(sqlpp::integral{}, sqlpp::alias::a),
                "($1 + $2) + $3");
  SQLPP_COMPARE(parameter(sqlpp::boolean{}, something) and
                    parameter(sqlpp::boolean{}, other) and
                    parameter(sqlpp::boolean{}, sqlpp::alias::a),
                "$1 AND $2 AND $3");

  {
    auto s =
        select(parameter(foo.id).as(something), parameter(bar.textN).as(other),
               parameter(foo.doubleN).as(foo.doubleN));

    SQLPP_COMPARE(s, "SELECT $1 AS something, $2 AS other, $3 AS double_n");
  }

  {
    auto left = select(parameter(foo.id).as(something)).as(sqlpp::alias::left);
    auto right =
        select(parameter(bar.textN).as(something)).as(sqlpp::alias::right);

    SQLPP_COMPARE(left.join(right).on(parameter(foo.doubleN) > 7),
                  "(SELECT $1 AS something) AS left INNER JOIN (SELECT $2 AS "
                  "something) AS right ON $3 > 7");
    SQLPP_COMPARE(left.cross_join(right),
                  "(SELECT $1 AS something) AS left CROSS JOIN (SELECT $2 AS "
                  "something) AS right");
  }

  {
    auto left = select(parameter(foo.id).as(something));
    auto right = select(parameter(bar.id).as(something));

    SQLPP_COMPARE(left.union_all(right),
                  "SELECT $1 AS something UNION ALL SELECT $2 AS something");
  }

  {
    auto cwt = case_when(parameter(foo.boolN))
                   .then(parameter(foo.id))
                   .else_(parameter(bar.intN));

    SQLPP_COMPARE(cwt, "CASE WHEN $1 THEN $2 ELSE $3 END");
  }

  SQLPP_COMPARE(
      sqlpp::postgresql::on_conflict(foo.id).do_update(
          foo.id = parameter(foo.id), foo.textNnD = parameter(foo.textNnD)),
      " ON CONFLICT (id) DO UPDATE SET id = $1, text_nn_d = $2");

  return 0;
}

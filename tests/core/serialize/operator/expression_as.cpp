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

SQLPP_CREATE_NAME_TAG(v);

int main(int, char *[]) {
  const auto val = sqlpp::value(17);
  const auto expr = sqlpp::value(17) + 4;

  const auto col_id = test::TabFoo{}.id;

  SQLPP_COMPARE(val.as(v), "17 AS v");
  SQLPP_COMPARE(expr.as(v), "(17 + 4) AS v");
  SQLPP_COMPARE(count(val).as(v), "COUNT(17) AS v");

  SQLPP_COMPARE(select_columns(dynamic(false, val.as(v))), "NULL AS v");
  SQLPP_COMPARE(select_columns(dynamic(false, expr.as(v))), "NULL AS v");
  SQLPP_COMPARE(select_columns(dynamic(false, count(val).as(v))), "NULL AS v");
  SQLPP_COMPARE(select_columns(dynamic(false, col_id.as(v))), "NULL AS v");

  return 0;
}

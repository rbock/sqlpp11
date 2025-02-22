/*
 * Copyright (c) 2023, Roland Bock
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
  const auto bar = test::TabBar{};

  // Single column.
  SQLPP_COMPARE(sum(bar.id), "SUM(tab_bar.id)");
  SQLPP_COMPARE(sum(sqlpp::distinct, bar.id), "SUM(DISTINCT tab_bar.id)");

  // Expression.
  // Note that the inner parens aren't necessary.
  SQLPP_COMPARE(sum(bar.id + 7), "SUM(tab_bar.id + 7)");
  SQLPP_COMPARE(sum(sqlpp::distinct, bar.id + 7),
                "SUM(DISTINCT tab_bar.id + 7)");

  // With sub select.
  SQLPP_COMPARE(sum(select(sqlpp::value(7).as(sqlpp::alias::a))),
                "SUM(SELECT 7 AS a)");
  SQLPP_COMPARE(
      sum(sqlpp::distinct, select(sqlpp::value(7).as(sqlpp::alias::a))),
      "SUM(DISTINCT SELECT 7 AS a)");

  return 0;
}

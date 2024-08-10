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

#include "../compare.h"
#include <sqlpp11/sqlpp11.h>

SQLPP_ALIAS_PROVIDER(v);

int main(int, char* [])
{
  const auto val = sqlpp::value(17);
  const auto expr = sqlpp::value(17) + 4;
  using expr_t = typename std::decay<decltype(expr)>::type;

  // IN expression with single select or other singe expression: No extra parentheses.
  SQLPP_COMPARE(val.in(val), "17 IN (17)");
  SQLPP_COMPARE(val.in(expr), "17 IN (17 + 4)");
  SQLPP_COMPARE(val.in(select(val.as(v))), "17 IN (SELECT 17 AS v)");

  SQLPP_COMPARE(val.not_in(val), "17 NOT IN (17)");
  SQLPP_COMPARE(val.not_in(expr), "17 NOT IN (17 + 4)");
  SQLPP_COMPARE(val.not_in(select(val.as(v))), "17 NOT IN (SELECT 17 AS v)");

  // IN expressions with multiple arguments require inner parentheses.
  SQLPP_COMPARE(val.in(1, select(val.as(v)), 23), "17 IN (1, (SELECT 17 AS v), 23)");
  SQLPP_COMPARE(val.in(std::vector<int>{17, 18, 19}), "17 IN (17, 18, 19)");
  SQLPP_COMPARE(val.in(std::vector<expr_t>{expr, expr, expr}), "17 IN ((17 + 4), (17 + 4), (17 + 4))");

  SQLPP_COMPARE(val.not_in(1, select(val.as(v))), "17 NOT IN (1, (SELECT 17 AS v))");
  SQLPP_COMPARE(val.not_in(std::vector<int>{17, 18, 19}), "17 NOT IN (17, 18, 19)");
  SQLPP_COMPARE(val.not_in(std::vector<expr_t>{expr, expr, expr}), "17 NOT IN ((17 + 4), (17 + 4), (17 + 4))");

  // IN expressions with no arguments would be an error in SQL, but the library interprets the intent gracefully.
  SQLPP_COMPARE(val.in(std::vector<expr_t>{}), "0");
  SQLPP_COMPARE(val.not_in(std::vector<expr_t>{}), "1");

  return 0;
}

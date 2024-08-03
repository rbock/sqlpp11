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

int main(int, char* [])
{
  const auto val = sqlpp::value(1);
  const auto expr = sqlpp::value(17) + 4;

  // Operands are enclosed in parentheses where required.
  SQLPP_COMPARE(val < val, "1 < 1");
  SQLPP_COMPARE(val <= val, "1 <= 1");
  SQLPP_COMPARE(val == val, "1 = 1");
  SQLPP_COMPARE(val != val, "1 <> 1");
  SQLPP_COMPARE(val >= val, "1 >= 1");
  SQLPP_COMPARE(val > val, "1 > 1");
  SQLPP_COMPARE(val.is_distinct_from(val), "1 IS DISTINCT FROM 1");
  SQLPP_COMPARE(val.is_not_distinct_from(val), "1 IS NOT DISTINCT FROM 1");

  SQLPP_COMPARE(val < expr, "1 < (17 + 4)");
  SQLPP_COMPARE(val <= expr, "1 <= (17 + 4)");
  SQLPP_COMPARE(val == expr, "1 = (17 + 4)");
  SQLPP_COMPARE(val != expr, "1 <> (17 + 4)");
  SQLPP_COMPARE(val >= expr, "1 >= (17 + 4)");
  SQLPP_COMPARE(val > expr, "1 > (17 + 4)");
  SQLPP_COMPARE(val.is_distinct_from(expr), "1 IS DISTINCT FROM (17 + 4)");
  SQLPP_COMPARE(val.is_not_distinct_from(expr), "1 IS NOT DISTINCT FROM (17 + 4)");

  SQLPP_COMPARE(expr < val, "(17 + 4) < 1");
  SQLPP_COMPARE(expr <= val, "(17 + 4) <= 1");
  SQLPP_COMPARE(expr == val, "(17 + 4) = 1");
  SQLPP_COMPARE(expr != val, "(17 + 4) <> 1");
  SQLPP_COMPARE(expr >= val, "(17 + 4) >= 1");
  SQLPP_COMPARE(expr > val, "(17 + 4) > 1");
  SQLPP_COMPARE(expr.is_distinct_from(val), "(17 + 4) IS DISTINCT FROM 1");
  SQLPP_COMPARE(expr.is_not_distinct_from(val), "(17 + 4) IS NOT DISTINCT FROM 1");

  SQLPP_COMPARE(expr < expr, "(17 + 4) < (17 + 4)");
  SQLPP_COMPARE(expr <= expr, "(17 + 4) <= (17 + 4)");
  SQLPP_COMPARE(expr == expr, "(17 + 4) = (17 + 4)");
  SQLPP_COMPARE(expr != expr, "(17 + 4) <> (17 + 4)");
  SQLPP_COMPARE(expr >= expr, "(17 + 4) >= (17 + 4)");
  SQLPP_COMPARE(expr > expr, "(17 + 4) > (17 + 4)");
  SQLPP_COMPARE(expr.is_distinct_from(expr), "(17 + 4) IS DISTINCT FROM (17 + 4)");
  SQLPP_COMPARE(expr.is_not_distinct_from(expr), "(17 + 4) IS NOT DISTINCT FROM (17 + 4)");

  // Same for unary operators
  SQLPP_COMPARE(val.is_null(), "1 IS NULL");
  SQLPP_COMPARE(val.is_not_null(), "1 IS NOT NULL");

  SQLPP_COMPARE(expr.is_null(), "(17 + 4) IS NULL");
  SQLPP_COMPARE(expr.is_not_null(), "(17 + 4) IS NOT NULL");

  return 0;
}

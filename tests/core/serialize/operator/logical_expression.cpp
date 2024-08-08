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
  const auto val = sqlpp::value(true);
  const auto expr = sqlpp::value(17) > 15;

  // Operands are enclosed in parenheses where required
  SQLPP_COMPARE(val and val, "1 AND 1");
  SQLPP_COMPARE(val and expr, "1 AND (17 > 15)");
  SQLPP_COMPARE(expr and val, "(17 > 15) AND 1");
  SQLPP_COMPARE(expr and expr, "(17 > 15) AND (17 > 15)");

  SQLPP_COMPARE(val or val, "1 OR 1");
  SQLPP_COMPARE(val or expr, "1 OR (17 > 15)");
  SQLPP_COMPARE(expr or val, "(17 > 15) OR 1");
  SQLPP_COMPARE(expr or expr, "(17 > 15) OR (17 > 15)");

  SQLPP_COMPARE(not val, "NOT 1");
  SQLPP_COMPARE(not expr, "NOT (17 > 15)");

  // Combined logical expression.
  SQLPP_COMPARE(not val and not expr, "(NOT 1) AND (NOT (17 > 15))");
  SQLPP_COMPARE(not val or not expr, "(NOT 1) OR (NOT (17 > 15))");
  SQLPP_COMPARE(not (val and expr), "NOT (1 AND (17 > 15))");
  SQLPP_COMPARE(not (val or expr), "NOT (1 OR (17 > 15))");

  // Chains are not nested in parentheses.
  SQLPP_COMPARE(val and val and val and val and val, "1 AND 1 AND 1 AND 1 AND 1");
  SQLPP_COMPARE(val or val or val or val or val, "1 OR 1 OR 1 OR 1 OR 1");

  // Broken chains use parentheses for the respective blocks.
  SQLPP_COMPARE((val and val and val) or (val and val), "(1 AND 1 AND 1) OR (1 AND 1)");
  SQLPP_COMPARE((val or val or val) and (val or val), "(1 OR 1 OR 1) AND (1 OR 1)");

  // NOT is not chained gracefully, but hey, don't do that anyways.
  SQLPP_COMPARE(not not not val, "NOT (NOT (NOT 1))");

  // Operands are enclosed in parenheses where required or completely dropped if inactive
  SQLPP_COMPARE(val and dynamic(true, val), "1 AND 1");
  SQLPP_COMPARE(val and dynamic(true, expr), "1 AND (17 > 15)");
  SQLPP_COMPARE(expr and dynamic(true, val), "(17 > 15) AND 1");
  SQLPP_COMPARE(expr and dynamic(true, expr), "(17 > 15) AND (17 > 15)");

  SQLPP_COMPARE(val or dynamic(true, val), "1 OR 1");
  SQLPP_COMPARE(val or dynamic(true, expr), "1 OR (17 > 15)");
  SQLPP_COMPARE(expr or dynamic(true, val), "(17 > 15) OR 1");
  SQLPP_COMPARE(expr or dynamic(true, expr), "(17 > 15) OR (17 > 15)");

  SQLPP_COMPARE(val and dynamic(false, val), "1");
  SQLPP_COMPARE(val and dynamic(false, expr), "1");
  SQLPP_COMPARE(expr and dynamic(false, val), "17 > 15");
  SQLPP_COMPARE(expr and dynamic(false, expr), "17 > 15");

  SQLPP_COMPARE(val or dynamic(false, val), "1");
  SQLPP_COMPARE(val or dynamic(false, expr), "1");
  SQLPP_COMPARE(expr or dynamic(false, val), "17 > 15");
  SQLPP_COMPARE(expr or dynamic(false, expr), "17 > 15");

  // Chained partially dynamic expressions
  SQLPP_COMPARE(val and dynamic(true, val) and expr, "1 AND 1 AND (17 > 15)");
  SQLPP_COMPARE(val and dynamic(false, val) and expr, "1 AND (17 > 15)");

  SQLPP_COMPARE(val or dynamic(true, val) or expr, "1 OR 1 OR (17 > 15)");
  SQLPP_COMPARE(val or dynamic(false, val) or expr, "1 OR (17 > 15)");

  return 0;
}

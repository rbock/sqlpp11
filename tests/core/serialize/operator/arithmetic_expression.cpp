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

int main(int, char *[]) {
  const auto val = sqlpp::value(1);
  const auto expr = sqlpp::value(17) + 4;

  // Operands are enclosed in parenheses where required.
  SQLPP_COMPARE(val + val, "1 + 1");
  SQLPP_COMPARE(val - val, "1 - 1");
  SQLPP_COMPARE(val * val, "1 * 1");
  SQLPP_COMPARE(val / val, "1 / 1");
  SQLPP_COMPARE(val % val, "1 % 1");

  SQLPP_COMPARE(val + expr, "1 + (17 + 4)");
  SQLPP_COMPARE(val - expr, "1 - (17 + 4)");
  SQLPP_COMPARE(val * expr, "1 * (17 + 4)");
  SQLPP_COMPARE(val / expr, "1 / (17 + 4)");
  SQLPP_COMPARE(val % expr, "1 % (17 + 4)");

  SQLPP_COMPARE(expr + val, "(17 + 4) + 1");
  SQLPP_COMPARE(expr - val, "(17 + 4) - 1");
  SQLPP_COMPARE(expr * val, "(17 + 4) * 1");
  SQLPP_COMPARE(expr / val, "(17 + 4) / 1");
  SQLPP_COMPARE(expr % val, "(17 + 4) % 1");

  SQLPP_COMPARE(expr + expr, "(17 + 4) + (17 + 4)");
  SQLPP_COMPARE(expr - expr, "(17 + 4) - (17 + 4)");
  SQLPP_COMPARE(expr * expr, "(17 + 4) * (17 + 4)");
  SQLPP_COMPARE(expr / expr, "(17 + 4) / (17 + 4)");
  SQLPP_COMPARE(expr % expr, "(17 + 4) % (17 + 4)");

  // Same for unary expressions.
  SQLPP_COMPARE(-val, "-1");
  SQLPP_COMPARE(-val + val, "(-1) + 1");
  SQLPP_COMPARE(-expr, "-(17 + 4)");

  const auto text = sqlpp::value("a");
  const auto text_expr = sqlpp::value("b") + "c";

  // Same for concatenation.
  SQLPP_COMPARE(text + text, "CONCAT('a', 'a')");
  SQLPP_COMPARE(text + text_expr, "CONCAT('a', CONCAT('b', 'c'))");
  SQLPP_COMPARE(text_expr + text, "CONCAT(CONCAT('b', 'c'), 'a')");
  SQLPP_COMPARE(text_expr + text_expr,
                "CONCAT(CONCAT('b', 'c'), CONCAT('b', 'c'))");

  // Arithmetic expressions can be named with AS
  SQLPP_COMPARE((val + val).as(sqlpp::alias::a), "(1 + 1) AS a");
  SQLPP_COMPARE((val - val).as(sqlpp::alias::a), "(1 - 1) AS a");
  SQLPP_COMPARE((val * val).as(sqlpp::alias::a), "(1 * 1) AS a");
  SQLPP_COMPARE((val / val).as(sqlpp::alias::a), "(1 / 1) AS a");
  SQLPP_COMPARE((val % val).as(sqlpp::alias::a), "(1 % 1) AS a");

  // Arithmetic expressions can be compared
  SQLPP_COMPARE((val + val) < 17, "(1 + 1) < 17");
  SQLPP_COMPARE((val - val) < 17, "(1 - 1) < 17");
  SQLPP_COMPARE((val * val) < 17, "(1 * 1) < 17");
  SQLPP_COMPARE((val / val) < 17, "(1 / 1) < 17");
  SQLPP_COMPARE((val % val) < 17, "(1 % 1) < 17");
  SQLPP_COMPARE(-val < 17, "(-1) < 17");
  SQLPP_COMPARE((text + text) < "z", "CONCAT('a', 'a') < 'z'");

  return 0;
}

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

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/tests/core/serialize_helpers.h>
#include <sqlpp11/tests/core/tables.h>

int main() {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};
  const auto aFoo = foo.as(sqlpp::alias::a);

  const auto x = cte(sqlpp::alias::x).as(select(foo.id).from(foo).where(true));
  const auto xa = x.as(sqlpp::alias::a);

  // Single table
  SQLPP_COMPARE(using_(foo), " USING tab_foo");
  SQLPP_COMPARE(using_(aFoo), " USING tab_foo AS a");
  SQLPP_COMPARE(using_(dynamic(true, foo)), " USING tab_foo");
  SQLPP_COMPARE(using_(dynamic(false, foo)), "");

  // Static joins
  SQLPP_COMPARE(using_(foo.cross_join(bar)),
                " USING tab_foo CROSS JOIN tab_bar");
  SQLPP_COMPARE(using_(dynamic(true, foo.cross_join(bar))),
                " USING tab_foo CROSS JOIN tab_bar");
  SQLPP_COMPARE(using_(dynamic(false, foo.cross_join(bar))), "");

  // CTE
  SQLPP_COMPARE(using_(x), " USING x");
  SQLPP_COMPARE(using_(xa), " USING x AS a");
  SQLPP_COMPARE(using_(dynamic(true, x)), " USING x");
  SQLPP_COMPARE(using_(dynamic(false, x)), "");

  return 0;
}

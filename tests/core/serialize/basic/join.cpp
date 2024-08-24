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

#include "Sample.h"
#include "../compare.h"
#include <sqlpp11/sqlpp11.h>

SQLPP_ALIAS_PROVIDER(v);

int main(int, char* [])
{
  const auto val = sqlpp::value(17);
  const auto expr = sqlpp::value(17) + 4;

  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Join with two tables
  SQLPP_COMPARE(foo.join(bar).on(foo.id == bar.id), "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.inner_join(bar).on(foo.id == bar.id), "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.left_outer_join(bar).on(foo.id == bar.id), "tab_foo LEFT OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.right_outer_join(bar).on(foo.id == bar.id), "tab_foo RIGHT OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.full_outer_join(bar).on(foo.id == bar.id), "tab_foo FULL OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");

  SQLPP_COMPARE(foo.cross_join(bar), "tab_foo CROSS JOIN tab_bar");

  // Join with dynamic table
  SQLPP_COMPARE(foo.join(dynamic(true, bar)).on(foo.id == bar.id), "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.inner_join(dynamic(true, bar)).on(foo.id == bar.id), "tab_foo INNER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.left_outer_join(dynamic(true, bar)).on(foo.id == bar.id), "tab_foo LEFT OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.right_outer_join(dynamic(true, bar)).on(foo.id == bar.id), "tab_foo RIGHT OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");
  SQLPP_COMPARE(foo.full_outer_join(dynamic(true, bar)).on(foo.id == bar.id), "tab_foo FULL OUTER JOIN tab_bar ON tab_foo.id = tab_bar.id");

  SQLPP_COMPARE(foo.cross_join(dynamic(true, bar)), "tab_foo CROSS JOIN tab_bar");

  SQLPP_COMPARE(foo.join(dynamic(false, bar)).on(foo.id == bar.id), "tab_foo");
  SQLPP_COMPARE(foo.inner_join(dynamic(false, bar)).on(foo.id == bar.id), "tab_foo");
  SQLPP_COMPARE(foo.left_outer_join(dynamic(false, bar)).on(foo.id == bar.id), "tab_foo");
  SQLPP_COMPARE(foo.right_outer_join(dynamic(false, bar)).on(foo.id == bar.id), "tab_foo");
  SQLPP_COMPARE(foo.full_outer_join(dynamic(false, bar)).on(foo.id == bar.id), "tab_foo");

  SQLPP_COMPARE(foo.cross_join(dynamic(false, bar)), "tab_foo");

#warning: Need to add tests with 3 tables

#warning: Need to add tests with CTEs
  return 0;
}

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

#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/tests/core/serialize_helpers.h>
#include <sqlpp11/sqlpp11.h>

int main(int, char* [])
{
  const auto t = test::TabBar{};
  const auto f = test::TabFoo{};

  // Using member function
  SQLPP_COMPARE(select(t.id)
         .from(t)
         .unconditionally()
         .union_all(select(f.id).from(f).unconditionally()), "SELECT tab_bar.id FROM tab_bar UNION ALL SELECT tab_foo.id FROM tab_foo");

  SQLPP_COMPARE(select(t.id)
         .from(t)
         .unconditionally()
         .union_distinct(select(f.id).from(f).unconditionally()), "SELECT tab_bar.id FROM tab_bar UNION DISTINCT SELECT tab_foo.id FROM tab_foo");

  SQLPP_COMPARE(select(t.intN.as(f.id))
                    .from(t)
                    .unconditionally()
                    .union_distinct(select(f.id).from(f).unconditionally())
                    .union_all(select(t.id).from(t).unconditionally()),
                "SELECT tab_bar.int_n AS id FROM tab_bar UNION DISTINCT SELECT tab_foo.id FROM tab_foo UNION ALL "
                "SELECT tab_bar.id FROM tab_bar");

  // Using free function
  SQLPP_COMPARE(union_all(select(t.id)
         .from(t)
         .unconditionally(),
         select(f.id).from(f).unconditionally()), "SELECT tab_bar.id FROM tab_bar UNION ALL SELECT tab_foo.id FROM tab_foo");

  SQLPP_COMPARE(union_distinct(select(t.id)
         .from(t)
         .unconditionally(),
         select(f.id).from(f).unconditionally()), "SELECT tab_bar.id FROM tab_bar UNION DISTINCT SELECT tab_foo.id FROM tab_foo");

  SQLPP_COMPARE(union_all(union_distinct(select(t.intN.as(f.id))
                    .from(t)
                    .unconditionally(),
                    select(f.id).from(f).unconditionally()),
                    select(t.id).from(t).unconditionally()),
                "SELECT tab_bar.int_n AS id FROM tab_bar UNION DISTINCT SELECT tab_foo.id FROM tab_foo UNION ALL "
                "SELECT tab_bar.id FROM tab_bar");

  return 0;
}

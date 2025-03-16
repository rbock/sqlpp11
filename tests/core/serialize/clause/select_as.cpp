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
#include <sqlpp23/tests/core/tables.h>

int main(int, char*[]) {
  const auto foo = test::TabFoo{};

  // SELECT a single value and use that as a table.
  {
    const auto s = sqlpp::select(foo.id).from(foo).where(true);
    const auto a = s.as(sqlpp::alias::a);
    SQLPP_COMPARE(s, "SELECT tab_foo.id FROM tab_foo WHERE 1");
    SQLPP_COMPARE(a, "(SELECT tab_foo.id FROM tab_foo WHERE 1) AS a");
    SQLPP_COMPARE(a.id, "a.id");
    SQLPP_COMPARE(select(all_of(a)), "SELECT a.id");
  }

  // SELECT a multiple values and use that as a table.
  {
    const auto s = sqlpp::select(foo.id, foo.intN).from(foo).where(true);
    const auto a = s.as(sqlpp::alias::a);
    SQLPP_COMPARE(s, "SELECT tab_foo.id, tab_foo.int_n FROM tab_foo WHERE 1");
    SQLPP_COMPARE(
        a, "(SELECT tab_foo.id, tab_foo.int_n FROM tab_foo WHERE 1) AS a");
    SQLPP_COMPARE(a.id, "a.id");
    SQLPP_COMPARE(a.intN, "a.int_n");
    SQLPP_COMPARE(select(all_of(a)), "SELECT a.id, a.int_n");
  }

  return 0;
}

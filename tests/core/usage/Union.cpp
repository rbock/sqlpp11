/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include "is_regular.h"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/tests/core/MockDb.h>
#include <sqlpp11/tests/core/result_helpers.h>
#include <sqlpp11/tests/core/tables.h>

namespace greek {
SQLPP_CREATE_NAME_TAG(id);
SQLPP_CREATE_NAME_TAG(textN);
} // namespace greek

int Union(int, char *[]) {
  MockDb db;

  const auto t = test::TabBar{};
  const auto f = test::TabFoo{};

  db(select(t.id).from(t).where(true).union_distinct(
      select(f.intN.as(t.id)).from(f).where(true)));
  db(select(t.id).from(t).where(true).union_all(
      select(f.intN.as(t.id)).from(f).where(true)));

  // t.id can be null, a given value cannot
  db(select(t.id).from(t).where(true).union_all(
      select(sqlpp::value(1).as(t.id))));
  db(select(t.id).from(t).where(true).union_all(
      select(sqlpp::value(1).as(greek::id))));

  // t.textN can be null, f.textNnD cannot
  static_assert(
      sqlpp::is_optional<sqlpp::value_type_of_t<decltype(t.textN)>>::value, "");
  static_assert(not sqlpp::is_optional<
                    sqlpp::value_type_of_t<decltype(f.textNnD)>>::value,
                "");
  db(select(t.textN).from(t).where(true).union_all(
      select(f.textNnD.as(greek::textN)).from(f).where(true)));

  auto u = select(t.id)
               .from(t)
               .where(true)
               .union_all(select(f.intN.as(t.id)).from(f).where(true))
               .as(sqlpp::alias::u);

  db(select(all_of(u)).from(u).where(true).union_all(
      select(t.intN.as(t.id)).from(t).where(true)));
  db(select(u.id).from(u).where(true).union_all(
      select(t.intN.as(t.id)).from(t).where(true)));

  db(select(t.id)
         .from(t)
         .where(true)
         .union_all(select(t.id).from(t).where(true))
         .union_all(select(t.id).from(t).where(true)));

  return 0;
}

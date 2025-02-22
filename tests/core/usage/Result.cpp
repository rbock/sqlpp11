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
#include <sqlpp23/sqlpp23.h>
#include <sqlpp23/tests/core/MockDb.h>
#include <sqlpp23/tests/core/result_helpers.h>
#include <sqlpp23/tests/core/tables.h>

namespace {
SQLPP_CREATE_NAME_TAG(something);
}

int Result(int, char *[]) {
  MockDb db = {};

  const auto t = test::TabBar{};

  static_assert(not sqlpp::is_optional<decltype(t.id)>::value,
                "t.id cannot be null");

  // Using a non-enforcing db
  for (const auto &row : db(select(all_of(t), t.textN.like("").as(something))
                                .from(t)
                                .where(true))) {
    static_assert(not sqlpp::is_optional<decltype(row.id)>::value,
                  "row.id cannot be null");

    for (const auto &sub :
         db(select(all_of(t)).from(t).where(t.id == row.id))) {
      std::cerr << sub.id << std::endl;
    }
    db(insert_into(t).set(t.textN = row.textN.value(), t.boolNn = false));
  }

  sqlpp::select((t.id + 1).as(t.id)).flags(sqlpp::all).from(t);
  for (const auto &row : db(select(all_of(t)).from(t).where(true))) {
    static_assert(not sqlpp::is_optional<decltype(row.id)>::value,
                  "row.id cannot be null");
  }

  for (const auto &row : db(select(all_of(t)).from(t).where(true))) {
    static_assert(not sqlpp::is_optional<decltype(row.id)>::value,
                  "row.id cannot be null");
  }

  sqlpp::select((t.id + 1).as(t.id)).flags(sqlpp::all).from(t);

  return 0;
}

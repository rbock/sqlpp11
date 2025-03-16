/*
 * Copyright (c) 2013-2016, Roland Bock
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
#include <sqlpp23/tests/core/MockDb.h>
#include <sqlpp23/tests/core/tables.h>
#include <iostream>
#include "is_regular.h"

int Remove(int, char*[]) {
  MockDb db = {};
  MockDb::_context_t printer = {};

  const auto maybe = true;
  const auto t = test::TabBar{};
  const auto f = test::TabFoo{};

  {
    using T = decltype(delete_from(t));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  {
    using T = decltype(delete_from(t).where(t.textN != "transparent"));
    auto x = delete_from(t).where(t.textN != "transparent");
    [[maybe_unused]] T y(x);
    [[maybe_unused]] T z(std::move(x));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  to_sql_string(printer, delete_from(t));
  to_sql_string(printer, delete_from(t).where(t.textN != "transparent"));
  std::cerr << to_sql_string(printer, delete_from(t).where(true)) << std::endl;

  db(delete_from(t).where(t.textN.in(select(f.textNnD).from(f).where(true))));
  db(delete_from(t).where(
      dynamic(maybe, t.textN.in(select(f.textNnD).from(f).where(true)))));

  return 0;
}

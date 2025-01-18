/*
 * Copyright (c) 2025, Roland Bock
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

// We need to include this here to change the sqlite3 version number for this test (if necessary)
#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#if SQLITE_VERSION_NUMBER >= 3039000
#undef SQLITE_VERSION_NUMBER
#define SQLITE_VERSION_NUMBER 3038999
#endif
#include <sqlpp11/tests/core/constraints_helpers.h>
#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/core/compat/type_traits.h>
#include <sqlpp11/sqlite3/sqlite3.h>

int main()
{
  auto ctx = sqlpp::sqlite3::context_t{};

  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // OK
  std::ignore = to_sql_string(ctx, foo.join(bar).on(true));

  // sqlite3 does not support full outer join before 3.39.0
  // See https://www.sqlite.org/changes.html
  SQLPP_CHECK_STATIC_ASSERT(to_sql_string(ctx, foo.full_outer_join(bar).on(true)),
                            "Sqlite3: No support for full outer join");

  // sqlite3 does not support right outer join before 3.39.0
  SQLPP_CHECK_STATIC_ASSERT(to_sql_string(ctx, foo.right_outer_join(bar).on(true)),
                            "Sqlite3: No support for right outer join");
}


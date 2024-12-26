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

#include <sqlpp11/tests/core/constraints_helpers.h>
#include <sqlpp11/tests/core/tables.h>

int main()
{
  const auto maybe = true;
  const auto bar = test::TabBar{};
  const auto foo = test::TabFoo{};

    // OK
  update_set(bar.boolNn = true);
  update_set(bar.boolNn = true, bar.textN = "");

  update_set(dynamic(maybe, bar.boolNn = true));
  update_set(dynamic(maybe, bar.boolNn = true), bar.textN = "");
  update_set(bar.boolNn = true, dynamic(maybe, bar.textN = ""));
  update_set(dynamic(maybe, bar.boolNn = true), dynamic(maybe, bar.textN = ""));

  // Try to update nothing
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::update_set(), "at least one assignment expression required in set()");

#warning : Is there a good way to test this (fail with enable_if since argument is not an assignment)?
  // update_set(bar.boolNn == true);

  // Try duplicate columns
  SQLPP_CHECK_STATIC_ASSERT(update_set(bar.boolNn = true, bar.boolNn = false), "at least one duplicate column detected in set()");
  SQLPP_CHECK_STATIC_ASSERT(update_set(bar.boolNn = true, bar.textN = "", bar.boolNn = false), "at least one duplicate column detected in set()");
  SQLPP_CHECK_STATIC_ASSERT(update_set(dynamic(maybe, bar.boolNn = true), bar.boolNn = false), "at least one duplicate column detected in set()");
  SQLPP_CHECK_STATIC_ASSERT(update_set(bar.boolNn = true, dynamic(maybe, bar.boolNn = false)), "at least one duplicate column detected in set()");
  SQLPP_CHECK_STATIC_ASSERT(update_set(dynamic(maybe, bar.boolNn = true), dynamic(maybe, bar.boolNn = false)), "at least one duplicate column detected in set()");

  // Try to update multiple tables at once
  SQLPP_CHECK_STATIC_ASSERT(update_set(bar.boolNn = true, foo.doubleN = 7), "set() contains assignments for columns from more than one table");
  SQLPP_CHECK_STATIC_ASSERT(update_set(dynamic(maybe, bar.boolNn = true), foo.doubleN = 7), "set() contains assignments for columns from more than one table");
  SQLPP_CHECK_STATIC_ASSERT(update_set(bar.boolNn = true, dynamic(maybe, foo.doubleN = 7)), "set() contains assignments for columns from more than one table");
  SQLPP_CHECK_STATIC_ASSERT(update_set(dynamic(maybe, bar.boolNn = true), dynamic(maybe, foo.doubleN = 7)), "set() contains assignments for columns from more than one table");

  {
  auto u = update(bar);
  using U = decltype(u);
  static_assert(std::is_same<sqlpp::statement_consistency_check_t<U>, sqlpp::assert_update_assignments_t>::value, "");
  }
}


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
#include <sqlpp11/sqlpp11.h>

void test_group_by()
{
  auto v = sqlpp::value(17);
  auto foo = test::TabFoo{};
  auto bar = test::TabBar{};

  // Pre-join
  static_assert(not sqlpp::is_table<decltype(foo.join(bar))>::value, "");

  // Join
  {
    using J = decltype(foo.join(bar).on(foo.id == bar.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(
        std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<test::TabFoo, test::TabBar>>::value, "");
    static_assert(
        std::is_same<sqlpp::provided_outer_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
#warning: test the provided dynamic tables of?
  }

  {
    using J = decltype(foo.outer_join(bar).on(foo.id == bar.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(
        std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<test::TabFoo, test::TabBar>>::value, "");
    static_assert(
        std::is_same<sqlpp::provided_outer_tables_of_t<J>, sqlpp::detail::type_set<test::TabFoo, test::TabBar>>::value, "");
#warning: test the provided dynamic tables of?
  }

  // Join with dynamic table
  {
    using J = decltype(foo.join(dynamic(true, bar)).on(foo.id == bar.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(
        std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<test::TabFoo, test::TabBar>>::value, "");
#warning: OUTER is the wrong term. In a left-outer join, the *right* table is the one with optional rows.
    static_assert(
        std::is_same<sqlpp::provided_outer_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
#warning: test the provided dynamic tables of?
  }


#warning: Need to add tests with 3 tables

#warning: Need to add tests with CTEs
}

int main()
{
  void test_group_by();
}



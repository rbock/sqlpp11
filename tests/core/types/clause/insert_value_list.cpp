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
#include "../../../include/test_helpers.h"

void test_all_columns_have_default_values()
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  static_assert(not sqlpp::all_columns_have_default_values<int>::value, "");
  static_assert(not sqlpp::all_columns_have_default_values<sqlpp::detail::statement_policies_t<>>::value, "");

  static_assert(not test::TabBar::_required_insert_columns::empty(), "");
  static_assert(not sqlpp::all_columns_have_default_values<sqlpp::detail::type_set<test::TabBar>>::value, "");

  {
    using I = decltype(insert_into(foo).default_values());
    using Policies = I::_policies_t;
    static_assert(test::TabFoo::_required_insert_columns::empty(), "");
    static_assert(sqlpp::all_columns_have_default_values<sqlpp::detail::type_set<test::TabFoo>>::value, "");
    static_assert(sqlpp::all_columns_have_default_values<Policies>::value, "");
  }

  {
    using I = decltype(insert_into(bar).default_values());
    using Policies = I::_policies_t;
    static_assert(test::TabFoo::_required_insert_columns::empty(), "");
    static_assert(not sqlpp::all_columns_have_default_values<sqlpp::detail::type_set<test::TabBar>>::value, "");
    static_assert(not sqlpp::all_columns_have_default_values<Policies>::value, "");
  }
}

void test_insert_value_list()
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

#warning: Need to add tests
  {
  }
}

int main()
{
  void test_all_columns_have_default_values();
  void test_insert_value_list();
}


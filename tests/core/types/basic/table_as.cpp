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
#include <sqlpp23/tests/core/tables.h>

void test_table() {
  auto foo = test::TabFoo{};
  auto bar = test::TabBar{};
  using FooBar = decltype(foo.as(bar));
  using Id = decltype(foo.as(bar).id);

  static_assert(
      std::is_same<FooBar,
                   sqlpp::table_as_t<test::TabFoo_,
                                     test::TabBar_::_sqlpp_name_tag>>::value,
      "");
  static_assert(sqlpp::is_table<FooBar>::value, "");
  static_assert(not sqlpp::is_raw_table<FooBar>::value, "");
  static_assert(std::is_same<sqlpp::name_tag_of_t<FooBar>,
                             test::TabBar_::_sqlpp_name_tag>::value,
                "");
  static_assert(std::is_same<sqlpp::provided_tables_of_t<FooBar>,
                             sqlpp::detail::type_set<FooBar>>::value,
                "");
  static_assert(std::is_same<sqlpp::provided_static_tables_of_t<FooBar>,
                             sqlpp::provided_tables_of_t<FooBar>>::value,
                "");
  static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<FooBar>,
                             sqlpp::detail::type_set<>>::value,
                "");
  static_assert(std::is_same<sqlpp::required_tables_of_t<FooBar>,
                             sqlpp::detail::type_set<>>::value,
                "");
  static_assert(std::is_same<sqlpp::required_static_tables_of_t<FooBar>,
                             sqlpp::required_tables_of_t<FooBar>>::value,
                "");

  static_assert(not sqlpp::is_table<Id>::value, "");
  static_assert(std::is_same<sqlpp::name_tag_of_t<Id>,
                             test::TabFoo_::Id::_sqlpp_name_tag>::value,
                "");
  static_assert(std::is_same<sqlpp::provided_tables_of_t<Id>,
                             sqlpp::detail::type_set<>>::value,
                "");
  static_assert(std::is_same<sqlpp::provided_static_tables_of_t<Id>,
                             sqlpp::provided_tables_of_t<Id>>::value,
                "");
  static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<Id>,
                             sqlpp::provided_tables_of_t<Id>>::value,
                "");
  static_assert(std::is_same<sqlpp::required_tables_of_t<Id>,
                             sqlpp::detail::type_set<FooBar>>::value,
                "");
  static_assert(std::is_same<sqlpp::required_static_tables_of_t<Id>,
                             sqlpp::required_tables_of_t<Id>>::value,
                "");
}

int main() { void test_table(); }

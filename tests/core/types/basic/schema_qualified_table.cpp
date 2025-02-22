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

int main(int, char *[]) {
  // A schema-qualified table cannot be used without AS.
  auto major = schema_qualified_table(sqlpp::schema("major"), test::TabFoo{});
  using M = decltype(major);

  static_assert(not sqlpp::is_table<M>::value, "");

  // A schema-qualified table can be used as table with AS:
  auto major_foo = major.as(test::TabFoo{});
  using MF = decltype(major_foo);

  static_assert(sqlpp::is_table<MF>::value, "");
  static_assert(sqlpp::has_enabled_join<MF>::value, "");
  static_assert(std::is_same<sqlpp::provided_tables_of_t<MF>,
                             sqlpp::detail::type_set<MF>>::value,
                "");
  static_assert(std::is_same<sqlpp::required_tables_of_t<MF>,
                             sqlpp::detail::type_set<>>::value,
                "");

  // Columns of a schema-qualified table work just like columns of unqualified
  // tables.
  using MFI = decltype(major_foo.id);
  static_assert(sqlpp::is_integral<MFI>::value, "");
  static_assert(sqlpp::has_name_tag<MFI>::value, "");

  static_assert(std::is_same<sqlpp::provided_tables_of_t<MFI>,
                             sqlpp::detail::type_set<>>::value,
                "");
  static_assert(std::is_same<sqlpp::required_tables_of_t<MFI>,
                             sqlpp::detail::type_set<MF>>::value,
                "");

  return 0;
}

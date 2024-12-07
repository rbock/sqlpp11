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

#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/sqlpp11.h>

namespace
{
  template <typename... T>
  auto known_aggregate_columns_as_expected(T... t)
      -> std::is_same<sqlpp::known_aggregate_columns_of_t<decltype(group_by(std::move(t)...))>,
                      sqlpp::detail::type_set<T...>>;
}

void test_group_by()
{
  auto v = sqlpp::value(17);
  auto col_int = test::TabFoo{}.id;
  auto col_txt = test::TabFoo{}.textNnD;

  // Test that declared group by columns are considered group by columns
  static_assert(sqlpp::is_group_by_column<decltype(declare_group_by_column(v))>::value, "");

  // Constant values are neutral and therefore considered neither aggregate and non-aggregate.
  static_assert(decltype(known_aggregate_columns_as_expected(col_int))::value, "");
  static_assert(decltype(known_aggregate_columns_as_expected(col_int, col_txt))::value, "");

  // Dynamic columns are listed as dynamic columns in known_aggregate_columns_of_t, since they need to be dynamic in
  // select, too, and we need to check that there.
  static_assert(decltype(known_aggregate_columns_as_expected(dynamic(true, col_int)))::value, "");
  static_assert(decltype(known_aggregate_columns_as_expected(col_int, dynamic(true, col_txt)))::value, "");

  // Declared columns are listed as declared columns in known_aggregate_columns_of_t, since they need to be declared in
  // select, too, and we need to check that there.
  static_assert(decltype(known_aggregate_columns_as_expected(declare_group_by_column(v)))::value, "");
  static_assert(decltype(known_aggregate_columns_as_expected(col_int, declare_group_by_column(v)))::value, "");

  // Declared columns can be dynamic, too.
  static_assert(decltype(known_aggregate_columns_as_expected(dynamic(true, declare_group_by_column(v))))::value, "");
  static_assert(decltype(known_aggregate_columns_as_expected(col_int, dynamic(true, declare_group_by_column(v))))::value, "");

#warning: need to test dynamic and declared columns in select, too
}

int main()
{
  void test_group_by();
}


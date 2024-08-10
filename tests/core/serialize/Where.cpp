/*
 * Copyright (c) 2016-2016, Roland Bock
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

#include "compare.h"
#include "Sample.h"
#include <sqlpp11/sqlpp11.h>

#include <iostream>

namespace
{
  auto getTrue() -> std::string
  {
    MockDb::_serializer_context_t printer = {};
    return to_sql_string(printer, sqlpp::value(true)).str();
  }

  auto getFalse() -> std::string
  {
    MockDb::_serializer_context_t printer = {};
    return to_sql_string(printer, sqlpp::value(false)).str();
  }
}  // namespace

int Where(int, char*[])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Unconditionally
  SQLPP_COMPARE(select(foo.doubleN).from(foo).unconditionally(), "SELECT tab_foo.double_n FROM tab_foo");
  SQLPP_COMPARE(remove_from(foo).unconditionally(), "DELETE FROM tab_foo");
  SQLPP_COMPARE(update(foo).set(foo.doubleN = 42).unconditionally(), "UPDATE tab_foo SET double_n = 42");
  static_assert(
      sqlpp::is_numeric<decltype(foo.doubleN)>::value,
      "");
  static_assert(
      sqlpp::is_numeric<decltype(foo.doubleN - 1)>::value,
      "");
  static_assert(
      sqlpp::values_are_comparable<decltype(foo.doubleN), decltype(foo.doubleN - -1)>::value,
      "");
  static_assert(
      sqlpp::values_are_comparable<decltype(foo.doubleN), decltype(-1)>::value,
      "");
  SQLPP_COMPARE(update(foo).set(foo.doubleN = foo.doubleN - -1).unconditionally(),
          "UPDATE tab_foo SET double_n = (tab_foo.double_n - -1)");
  SQLPP_COMPARE(where(sqlpp::value(true)), " WHERE " + getTrue());

  // Never
  SQLPP_COMPARE(where(sqlpp::value(false)), " WHERE " + getFalse());

  // Sometimes
  SQLPP_COMPARE(where(bar.boolNn), " WHERE tab_bar.bool_nn");
  SQLPP_COMPARE(where(bar.boolNn == false), " WHERE (tab_bar.bool_nn = " + getFalse() + ")");
  SQLPP_COMPARE(where(bar.textN.is_null()), " WHERE (tab_bar.text_n IS NULL)");
  SQLPP_COMPARE(where(bar.textN == "SQL"), " WHERE (tab_bar.text_n = 'SQL')");
  SQLPP_COMPARE(where(is_not_distinct_from(bar.textN, ::sqlpp::make_optional("SQL"))),
          " WHERE (tab_bar.text_n IS NOT DISTINCT FROM 'SQL')");
  SQLPP_COMPARE(where(is_not_distinct_from(bar.textN, ::sqlpp::nullopt)),
          " WHERE (tab_bar.text_n IS NOT DISTINCT FROM NULL)");
  SQLPP_COMPARE(where(bar.textN.is_not_distinct_from(::sqlpp::make_optional("SQL"))),
          " WHERE (tab_bar.text_n IS NOT DISTINCT FROM 'SQL')");
  SQLPP_COMPARE(where(bar.textN.is_not_distinct_from(::sqlpp::nullopt)),
          " WHERE (tab_bar.text_n IS NOT DISTINCT FROM NULL)");

  // string argument
  SQLPP_COMPARE(where(bar.textN == std::string("SQL")), " WHERE (tab_bar.text_n = 'SQL')");

  // string_view argument
  SQLPP_COMPARE(where(bar.textN == ::sqlpp::string_view("SQL")), " WHERE (tab_bar.text_n = 'SQL')");

  return 0;
}

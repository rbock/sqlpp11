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
#include <sqlpp11/tests/core/types_helpers.h>
#include <sqlpp11/sqlpp11.h>

SQLPP_CREATE_NAME_TAG(cheese);
SQLPP_CREATE_NAME_TAG(cake);

void test_is_expression_as()
{
  auto v = sqlpp::value(17);
  auto t = sqlpp::value("");
  auto col_int = test::TabFoo{}.id;
  auto col_txt = test::TabFoo{}.textNnD;

  // Constant values are no expression alias
  static_assert(not sqlpp::is_expression_as<decltype(v)>::value, "");
  static_assert(not sqlpp::is_expression_as<decltype(v + v)>::value, "");

  // Columns are no expression alias
  static_assert(not sqlpp::is_expression_as<decltype(col_int)>::value, "");
  static_assert(not sqlpp::is_expression_as<decltype(col_int + v)>::value, "");

  // Normal functions are no expression alias
  static_assert(not sqlpp::is_expression_as<decltype(trim(t))>::value, "");
  static_assert(not sqlpp::is_expression_as<decltype(trim(col_txt))>::value, "");

  // But their alias is an expression alias (no surprise here, I guess)
  static_assert(sqlpp::is_expression_as<decltype((v).as(cheese))>::value, "");
  static_assert(sqlpp::is_expression_as<decltype((v + v).as(cheese))>::value, "");

  static_assert(sqlpp::is_expression_as<decltype((col_int).as(cheese))>::value, "");
  static_assert(sqlpp::is_expression_as<decltype((col_int + v).as(cheese))>::value, "");

  static_assert(sqlpp::is_expression_as<decltype((trim(t)).as(cheese))>::value, "");
  static_assert(sqlpp::is_expression_as<decltype((trim(col_txt)).as(cheese))>::value, "");

}

int main()
{
  void test_is_expression_as();
}


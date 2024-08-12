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

void test_contains_aggregate_function()
{
  auto v_not_null = sqlpp::value(17);

#warning: Need to test contains_aggregate_function

}

void test_contains_aggregate_expression()
{
  auto v_not_null = sqlpp::value(17);

#warning: Need to test contains_aggregate_expression

}

void test_contains_non_aggregate()
{
  auto v = sqlpp::value(17);
  auto col = test::TabFoo{}.id;

  static_assert(sqlpp::contains_non_aggregate<decltype(col)>::value, "");
  static_assert(sqlpp::contains_non_aggregate<decltype(col + v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(avg(col) + v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(count(col) + v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(min(col) + v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(max(col) + v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(sum(col) + v)>::value, "");
#warning: lets test some functions, too
#warning: lets test some clauses, too

}

int main()
{
  void test_contains_aggregate_function();
  void test_contains_aggregate_expression();
  void test_contains_non_aggregate();
}


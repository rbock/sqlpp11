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

SQLPP_ALIAS_PROVIDER(cheese);

void test_contains_aggregate_function()
{
  auto v = sqlpp::value(17);
  auto t = sqlpp::value("");
  auto col_int = test::TabFoo{}.id;
  auto col_txt = test::TabFoo{}.textNnD;

  // Values are aggregate neutral and therefore considered neither aggregate and non-aggregate context.
  static_assert(not sqlpp::contains_aggregate_function<decltype(v)>::value, "");
  static_assert(not sqlpp::contains_aggregate_function<decltype(v + v)>::value, "");

  // Columns are non-aggregates.
  static_assert(not sqlpp::contains_aggregate_function<decltype(col_int)>::value, "");
  static_assert(not sqlpp::contains_aggregate_function<decltype(col_int + v)>::value, "");

  // Normal functions of values or non-aggregates do not contain aggregate functions.
  static_assert(not sqlpp::contains_aggregate_function<decltype(trim(t))>::value, "");
  static_assert(not sqlpp::contains_aggregate_function<decltype(trim(col_txt))>::value, "");

  // Aggregate functions of non-aggregates and values contain aggregate functions.
  static_assert(not sqlpp::contains_non_aggregate<decltype(avg(v))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(count(v))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(min(v))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(max(v))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(sum(v))>::value, "");

  static_assert(sqlpp::contains_aggregate_function<decltype(avg(col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(count(col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(min(col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(max(col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(sum(col_int))>::value, "");

  static_assert(sqlpp::contains_aggregate_function<decltype(avg(col_int) + v)>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(count(col_int) + v)>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(min(col_int) + v)>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(max(col_int) + v)>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(sum(col_int) + v)>::value, "");

  static_assert(sqlpp::contains_aggregate_function<decltype(trim(max(col_txt)))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(max(trim(col_txt)))>::value, "");

  // Expressions of aggregate functions and non-aggregates contain aggregate functions.
  static_assert(sqlpp::contains_aggregate_function<decltype(avg(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(count(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(min(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(max(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(sum(col_int) + col_int)>::value, "");

  static_assert(sqlpp::contains_aggregate_function<decltype(trim(upper(lower(max(col_txt)))) + trim(col_txt))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(max(trim(col_txt)) + trim(col_txt))>::value, "");

  // Clauses expose non-aggregates (probably irrelevant)
  static_assert(not sqlpp::contains_aggregate_function<decltype(where(col_int > v))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(select_columns(max(col_int), v.as(cheese), col_int))>::value, "");
}

void test_contains_aggregate_expression()
{
  auto v_not_null = sqlpp::value(17);

  auto v = sqlpp::value(17);
  auto t = sqlpp::value("");
  auto col_int = test::TabFoo{}.id;
  auto col_txt = test::TabFoo{}.textNnD;

  using known_aggregates = sqlpp::detail::type_vector<decltype(v), decltype(col_txt), decltype(col_int + v)>;
  using unknown = sqlpp::detail::type_vector<>;

  // ...
  static_assert(sqlpp::contains_aggregate_expression<known_aggregates, decltype(v)>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<known_aggregates, decltype(v + v)>::value, "");

  // ...
  static_assert(not sqlpp::contains_aggregate_expression<unknown, decltype(v)>::value, "");
  static_assert(not sqlpp::contains_aggregate_expression<unknown, decltype(v + v)>::value, "");

#warning: activate
#if 0
  // Columns are non-aggregates.
  static_assert(not sqlpp::contains_aggregate_expression<decltype(col_int)>::value, "");
  static_assert(not sqlpp::contains_aggregate_expression<decltype(col_int + v)>::value, "");

  // Normal expressions of values or non-aggregates do not contain aggregate functions.
  static_assert(not sqlpp::contains_aggregate_expression<decltype(trim(t))>::value, "");
  static_assert(not sqlpp::contains_aggregate_expression<decltype(trim(col_txt))>::value, "");

  // Aggregate expressions of non-aggregates contain aggregate functions.
  static_assert(not sqlpp::contains_aggregate_expressions<decltype(avg(v))>::value, "");
  static_assert(not sqlpp::contains_aggregate_expressions<decltype(count(v))>::value, "");
  static_assert(not sqlpp::contains_aggregate_expressions<decltype(min(v))>::value, "");
  static_assert(not sqlpp::contains_aggregate_expressions<decltype(max(v))>::value, "");
  static_assert(not sqlpp::contains_aggregate_expressions<decltype(sum(v))>::value, "");

  static_assert(sqlpp::contains_aggregate_expression<decltype(avg(col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(count(col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(min(col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(max(col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(sum(col_int))>::value, "");

  static_assert(sqlpp::contains_aggregate_expression<decltype(avg(col_int) + v)>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(count(col_int) + v)>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(min(col_int) + v)>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(max(col_int) + v)>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(sum(col_int) + v)>::value, "");

  static_assert(sqlpp::contains_aggregate_expression<decltype(trim(max(col_txt)))>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(max(trim(col_txt)))>::value, "");

  // Expressions of aggregate expressions and non-aggregates contain aggregate functions.
  static_assert(sqlpp::contains_aggregate_expression<decltype(avg(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(count(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(min(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(max(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(sum(col_int) + col_int)>::value, "");

  static_assert(sqlpp::contains_aggregate_expression<decltype(trim(upper(lower(max(col_txt)))) + trim(col_txt))>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(max(trim(col_txt)) + trim(col_txt))>::value, "");

  // Clauses expose non-aggregates (probably irrelevant)
  static_assert(not sqlpp::contains_aggregate_expression<decltype(where(col_int > v))>::value, "");
  static_assert(sqlpp::contains_aggregate_expression<decltype(select_columns(max(col_int), v.as(cheese), col_int))>::value, "");
#endif

}

void test_contains_non_aggregate()
{
  auto v = sqlpp::value(17);
  auto t = sqlpp::value("");
  auto col_int = test::TabFoo{}.id;
  auto col_txt = test::TabFoo{}.textNnD;

  // Values are aggregate neutral and therefore considered neither aggregate and non-aggregate context.
  static_assert(not sqlpp::contains_non_aggregate<decltype(v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(v + v)>::value, "");

  // Columns are non-aggregates.
  static_assert(sqlpp::contains_non_aggregate<decltype(col_int)>::value, "");
  static_assert(sqlpp::contains_non_aggregate<decltype(col_int + v)>::value, "");

  // Functions can contain non-aggregates.
  static_assert(not sqlpp::contains_non_aggregate<decltype(trim(t))>::value, "");
  static_assert(sqlpp::contains_non_aggregate<decltype(trim(col_txt))>::value, "");

  // Aggregate functions of non-aggregates and values are aggregate functions.
  static_assert(not sqlpp::contains_non_aggregate<decltype(avg(v))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(count(v))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(min(v))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(max(v))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(sum(v))>::value, "");

  static_assert(not sqlpp::contains_non_aggregate<decltype(avg(col_int))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(count(col_int))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(min(col_int))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(max(col_int))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(sum(col_int))>::value, "");

  static_assert(not sqlpp::contains_non_aggregate<decltype(avg(col_int) + v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(count(col_int) + v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(min(col_int) + v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(max(col_int) + v)>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(sum(col_int) + v)>::value, "");

  static_assert(not sqlpp::contains_non_aggregate<decltype(trim(max(col_txt)))>::value, "");
  static_assert(not sqlpp::contains_non_aggregate<decltype(max(trim(col_txt)))>::value, "");

  // Expressions of aggregate functions and non-aggregates contain non-aggregates.
  static_assert(sqlpp::contains_non_aggregate<decltype(avg(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_non_aggregate<decltype(count(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_non_aggregate<decltype(min(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_non_aggregate<decltype(max(col_int) + col_int)>::value, "");
  static_assert(sqlpp::contains_non_aggregate<decltype(sum(col_int) + col_int)>::value, "");

  static_assert(sqlpp::contains_non_aggregate<decltype(trim(max(col_txt)) + trim(col_txt))>::value, "");
  static_assert(sqlpp::contains_non_aggregate<decltype(max(trim(col_txt)) + trim(col_txt))>::value, "");

  // Clauses expose non-aggregates (probably irrelevant)
  static_assert(sqlpp::contains_non_aggregate<decltype(where(col_int > v))>::value, "");
  static_assert(sqlpp::contains_non_aggregate<decltype(select_columns(max(col_int), v.as(cheese), col_int))>::value, "");
}

int main()
{
  void test_contains_aggregate_function();
  void test_contains_aggregate_expression();
  void test_contains_non_aggregate();
}


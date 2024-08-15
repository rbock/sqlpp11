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

void test_is_aggregate_function()
{
  auto v = sqlpp::value(17);
  auto t = sqlpp::value("");
  auto col_int = test::TabFoo{}.id;
  auto col_txt = test::TabFoo{}.textNnD;

  // Constant values are neutral and therefore considered neither aggregate and non-aggregate.
  static_assert(not sqlpp::is_aggregate_function<decltype(v)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(v + v)>::value, "");

  // Columns are not aggregate functions
  static_assert(not sqlpp::is_aggregate_function<decltype(col_int)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(col_int + v)>::value, "");

  // Normal functions of values or non-aggregates do not contain aggregate functions.
  static_assert(not sqlpp::is_aggregate_function<decltype(trim(t))>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(trim(col_txt))>::value, "");

  // Aggregate functions of non-aggregates and values are aggregate functions :-)
  static_assert(sqlpp::is_aggregate_function<decltype(avg(v))>::value, "");
  static_assert(sqlpp::is_aggregate_function<decltype(count(v))>::value, "");
  static_assert(sqlpp::is_aggregate_function<decltype(min(v))>::value, "");
  static_assert(sqlpp::is_aggregate_function<decltype(max(v))>::value, "");
  static_assert(sqlpp::is_aggregate_function<decltype(sum(v))>::value, "");

  static_assert(sqlpp::is_aggregate_function<decltype(avg(col_int))>::value, "");
  static_assert(sqlpp::is_aggregate_function<decltype(count(col_int))>::value, "");
  static_assert(sqlpp::is_aggregate_function<decltype(min(col_int))>::value, "");
  static_assert(sqlpp::is_aggregate_function<decltype(max(col_int))>::value, "");
  static_assert(sqlpp::is_aggregate_function<decltype(sum(col_int))>::value, "");

  static_assert(sqlpp::is_aggregate_function<decltype(max(trim(col_txt)))>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(trim(upper(lower(max(col_txt)))) + trim(col_txt))>::value, "");

  // Expressions containing aggregate functions are not aggregate functions.
  static_assert(not sqlpp::is_aggregate_function<decltype(avg(col_int) + v)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(count(col_int) + v)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(min(col_int) + v)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(max(col_int) + v)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(sum(col_int) + v)>::value, "");

  static_assert(not sqlpp::is_aggregate_function<decltype(trim(max(col_txt)))>::value, "");

  static_assert(not sqlpp::is_aggregate_function<decltype(avg(col_int) + col_int)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(count(col_int) + col_int)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(min(col_int) + col_int)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(max(col_int) + col_int)>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(sum(col_int) + col_int)>::value, "");

  static_assert(not sqlpp::is_aggregate_function<decltype(max(trim(col_txt)) + trim(col_txt))>::value, "");

  // Clauses do not expose aggregates functions.
  static_assert(not sqlpp::is_aggregate_function<decltype(where(col_int > v))>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(select_columns(v.as(cheese), col_int))>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(select_columns(max(col_int), v.as(cheese), col_int))>::value, "");
  static_assert(not sqlpp::is_aggregate_function<decltype(select_columns(dynamic(true, max(col_int)), v.as(cheese), col_int))>::value, "");
}

void test_contains_aggregate_function()
{
  auto v = sqlpp::value(17);
  auto t = sqlpp::value("");
  auto col_int = test::TabFoo{}.id;
  auto col_txt = test::TabFoo{}.textNnD;

  // Constant values are neutral and therefore considered neither aggregate and non-aggregate.
  static_assert(not sqlpp::contains_aggregate_function<decltype(v)>::value, "");
  static_assert(not sqlpp::contains_aggregate_function<decltype(v + v)>::value, "");

  // Columns are non-aggregates.
  static_assert(not sqlpp::contains_aggregate_function<decltype(col_int)>::value, "");
  static_assert(not sqlpp::contains_aggregate_function<decltype(col_int + v)>::value, "");

  // Normal functions of values or non-aggregates do not contain aggregate functions.
  static_assert(not sqlpp::contains_aggregate_function<decltype(trim(t))>::value, "");
  static_assert(not sqlpp::contains_aggregate_function<decltype(trim(col_txt))>::value, "");

  // Aggregate functions of non-aggregates and values contain aggregate functions.
  static_assert(sqlpp::contains_aggregate_function<decltype(avg(v))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(count(v))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(min(v))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(max(v))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(sum(v))>::value, "");

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

  // Clauses expose non-aggregates.
  static_assert(not sqlpp::contains_aggregate_function<decltype(where(col_int > v))>::value, "");
  static_assert(not sqlpp::contains_aggregate_function<decltype(select_columns(v.as(cheese), col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(select_columns(max(col_int), v.as(cheese), col_int))>::value, "");
  static_assert(sqlpp::contains_aggregate_function<decltype(select_columns(dynamic(true, max(col_int)), v.as(cheese), col_int))>::value, "");
}

void test_is_aggregate_expression()
{
  auto v_not_null = sqlpp::value(17);

  auto v = sqlpp::value(17);
  auto t = sqlpp::value("");
  auto agg_int = test::TabFoo{}.id;
  auto agg_txt = test::TabFoo{}.textNnD;

  auto col_int = test::TabBar{}.id;
  auto col_txt = test::TabBar{}.textN;

  using unknown = sqlpp::detail::type_vector<>;
  using known_aggregates = sqlpp::detail::type_vector<decltype(agg_txt), decltype(agg_int)>;

  // If there are no known aggregate expressions, then only aggregate functions will be found.
  static_assert(not sqlpp::is_aggregate_expression<unknown, decltype(v)>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<unknown, decltype(v + v)>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<unknown, decltype(col_int)>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<unknown, decltype(col_txt)>::value, "");
  static_assert(sqlpp::is_aggregate_expression<unknown, decltype(count(col_int))>::value, "");
  static_assert(sqlpp::is_aggregate_expression<unknown, decltype(count(col_txt))>::value, "");
  static_assert(sqlpp::is_aggregate_expression<unknown, decltype(max(v))>::value, "");
  static_assert(sqlpp::is_aggregate_expression<unknown, decltype(max(col_int))>::value, "");
  static_assert(sqlpp::is_aggregate_expression<unknown, decltype(max(col_txt))>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<unknown, decltype(dynamic(true, max(v)))>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<unknown, decltype(dynamic(true, max(col_int)))>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<unknown, decltype(dynamic(true, max(col_txt)))>::value, "");

  // Known aggregate expressions are detected as such.
  static_assert(sqlpp::is_aggregate_expression<known_aggregates, decltype(agg_int)>::value, "");
  static_assert(sqlpp::is_aggregate_expression<known_aggregates, decltype(agg_txt)>::value, "");

  static_assert(not sqlpp::is_aggregate_expression<known_aggregates, decltype(dynamic(true, agg_int))>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<known_aggregates, decltype(dynamic(true, agg_txt))>::value, "");

  // Known aggregate expressions are not detected as such in expressions.
  static_assert(not sqlpp::is_aggregate_expression<known_aggregates, decltype(agg_int + 17)>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<known_aggregates, decltype(17 + agg_int)>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<known_aggregates, decltype(agg_txt.like("%"))>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<known_aggregates, decltype((agg_int + v).between(1, 10))>::value, "");

  // Known aggregate expressions are not detected as such in aggregate functions.
  static_assert(sqlpp::is_aggregate_expression<known_aggregates, decltype(max(agg_int + 17))>::value, "");
  static_assert(sqlpp::is_aggregate_expression<known_aggregates, decltype(max(17 + agg_int))>::value, "");
  static_assert(sqlpp::is_aggregate_expression<known_aggregates, decltype(count(agg_txt.like("%")))>::value, "");
  static_assert(sqlpp::is_aggregate_expression<known_aggregates, decltype(count((agg_int + v).between(1, 10)))>::value, "");

  // Known aggregate expressions are not exposed as such by clauses.
  static_assert(not sqlpp::is_aggregate_expression<known_aggregates, decltype(select(agg_txt))>::value, "");
  static_assert(not sqlpp::is_aggregate_expression<known_aggregates, decltype(select(agg_int))>::value, "");
}

int main()
{
  void test_is_aggregate_function();
  void test_contains_aggregate_function();
  void test_is_aggregate_expression();
}


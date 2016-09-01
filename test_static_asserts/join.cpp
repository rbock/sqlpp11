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

#include "MockDb.h"
#include "Sample.h"
#include <iostream>
#include <sqlpp11/sqlpp11.h>

namespace
{
  constexpr auto t = test::TabBar{};
  constexpr auto f = test::TabFoo{};

  template <typename T>
  void print_type_on_error(std::true_type)
  {
  }

  template <typename T>
  void print_type_on_error(std::false_type)
  {
    T::_print_me_;
  }

  template <typename Assert, typename Lhs, typename Rhs>
  void join_static_check(const Lhs& lhs, const Rhs& rhs)
  {
    using CheckResult = sqlpp::check_pre_join_t<Lhs, Rhs>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using JoinType = decltype(sqlpp::join(lhs, rhs));
    using InnerJoinType = decltype(sqlpp::inner_join(lhs, rhs));
    using LeftOuterJoinType = decltype(sqlpp::left_outer_join(lhs, rhs));
    using RightOuterJoinType = decltype(sqlpp::right_outer_join(lhs, rhs));
    using OuterJoinType = decltype(sqlpp::outer_join(lhs, rhs));
    using CrossJoinType = decltype(sqlpp::cross_join(lhs, rhs));
    using ExpectedReturnType = sqlpp::logic::all_t<
        (Assert::value and sqlpp::is_pre_join_t<JoinType>::value and sqlpp::is_pre_join_t<InnerJoinType>::value and
         sqlpp::is_pre_join_t<LeftOuterJoinType>::value and sqlpp::is_pre_join_t<RightOuterJoinType>::value and
         sqlpp::is_pre_join_t<OuterJoinType>::value and sqlpp::is_join_t<CrossJoinType>::value) xor
        (std::is_same<JoinType, Assert>::value and std::is_same<InnerJoinType, Assert>::value and
         std::is_same<LeftOuterJoinType, Assert>::value and std::is_same<RightOuterJoinType, Assert>::value and
         std::is_same<OuterJoinType, Assert>::value and std::is_same<CrossJoinType, Assert>::value)>;
    print_type_on_error<JoinType>(ExpectedReturnType{});
    print_type_on_error<InnerJoinType>(ExpectedReturnType{});
    print_type_on_error<LeftOuterJoinType>(ExpectedReturnType{});
    print_type_on_error<RightOuterJoinType>(ExpectedReturnType{});
    print_type_on_error<OuterJoinType>(ExpectedReturnType{});
    print_type_on_error<CrossJoinType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename Lhs, typename Rhs>
  void on_static_check(const Lhs& lhs, const Rhs& rhs)
  {
    using CheckResult = sqlpp::check_join_on_t<Lhs, Rhs>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ResultType = decltype(lhs.on(rhs));
    using ExpectedReturnType = sqlpp::logic::all_t<(Assert::value and sqlpp::is_join_t<ResultType>::value) xor
                                                   std::is_same<ResultType, Assert>::value>;
    print_type_on_error<ResultType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  void static_join()
  {
    // Prepare a few table aliases for tests
    const auto ta = t.as(sqlpp::alias::a);
    const auto tb = t.as(sqlpp::alias::b);
    const auto fa = f.as(sqlpp::alias::a);
    const auto fb = f.as(sqlpp::alias::b);

    // OK: Join two different tables
    join_static_check<sqlpp::consistent_t>(t, f);
    join_static_check<sqlpp::consistent_t>(t, fa);
    join_static_check<sqlpp::consistent_t>(ta, fb);

    // OK: Self join
    join_static_check<sqlpp::consistent_t>(ta, tb);
    join_static_check<sqlpp::consistent_t>(t, tb);
    join_static_check<sqlpp::consistent_t>(ta, t);

    // Prepare a join for tests:
    const auto j = cross_join(ta, tb);

    // OK: Add a third table
    join_static_check<sqlpp::consistent_t>(j, f);
    join_static_check<sqlpp::consistent_t>(j, t.as(sqlpp::alias::c));
    join_static_check<sqlpp::consistent_t>(j, t);

    // Try a bunch of non-tables
    join_static_check<sqlpp::assert_pre_join_rhs_table_t>(t, 7);
    join_static_check<sqlpp::assert_pre_join_rhs_table_t>(t, t.alpha);
    join_static_check<sqlpp::assert_pre_join_rhs_table_t>(t, t.beta);
    join_static_check<sqlpp::assert_pre_join_rhs_table_t>(t, t.gamma);
    join_static_check<sqlpp::assert_pre_join_rhs_table_t>(t, t.delta);

    join_static_check<sqlpp::assert_pre_join_lhs_table_t>(7, t);
    join_static_check<sqlpp::assert_pre_join_lhs_table_t>(t.alpha, t);
    join_static_check<sqlpp::assert_pre_join_lhs_table_t>(t.beta, t);
    join_static_check<sqlpp::assert_pre_join_lhs_table_t>(t.gamma, t);
    join_static_check<sqlpp::assert_pre_join_lhs_table_t>(t.delta, t);

    // Try to join with join (rhs)
    join_static_check<sqlpp::assert_pre_join_rhs_no_join_t>(t, j);
    join_static_check<sqlpp::assert_pre_join_rhs_no_join_t>(f, j);
    join_static_check<sqlpp::assert_pre_join_rhs_no_join_t>(t.as(sqlpp::alias::left), j);

    // Try to join identical table names
    join_static_check<sqlpp::assert_pre_join_unique_names_t>(t, t);
    join_static_check<sqlpp::assert_pre_join_unique_names_t>(f, f);
    join_static_check<sqlpp::assert_pre_join_unique_names_t>(t.as(f), f);
    join_static_check<sqlpp::assert_pre_join_unique_names_t>(t, f.as(t));
    join_static_check<sqlpp::assert_pre_join_unique_names_t>(ta, fa);
    join_static_check<sqlpp::assert_pre_join_unique_names_t>(j, fa);
    join_static_check<sqlpp::assert_pre_join_unique_names_t>(j, fb);
    join_static_check<sqlpp::assert_pre_join_unique_names_t>(j, ta);
    join_static_check<sqlpp::assert_pre_join_unique_names_t>(j, tb);

    // Prepare a pre_joins for tests:
    const auto t_f = join(t, f);
    const auto f_t = join(f, t);
    const auto t_t = join(ta, tb);
    const auto f_f = join(fa, fb);

    // OK join.on()
    on_static_check<sqlpp::consistent_t>(t_f, t.alpha > f.omega);
    on_static_check<sqlpp::consistent_t>(f_t, t.alpha < f.omega);
    on_static_check<sqlpp::consistent_t>(f_f, fa.omega == fb.omega);
    on_static_check<sqlpp::consistent_t>(t_t, ta.alpha == tb.alpha);
    on_static_check<sqlpp::consistent_t>(t_f, t.gamma);

    // Try join.on(non-expression)
    on_static_check<sqlpp::assert_on_is_expression_t>(t_f, true);
    on_static_check<sqlpp::assert_on_is_expression_t>(t_f, 7);
    on_static_check<sqlpp::assert_on_is_expression_t>(t_f, t);

    // Try join.on(non-boolean)
    on_static_check<sqlpp::assert_on_is_boolean_expression_t>(t_f, t.alpha);
    on_static_check<sqlpp::assert_on_is_boolean_expression_t>(t_f, t.beta);
    on_static_check<sqlpp::assert_on_is_boolean_expression_t>(t_f, f.omega);

    // Try join.on(foreign-table)
    on_static_check<sqlpp::assert_join_on_no_foreign_table_dependencies_t>(t_f, ta.alpha != 0);
    on_static_check<sqlpp::assert_join_on_no_foreign_table_dependencies_t>(t_t, t.gamma);
    on_static_check<sqlpp::assert_join_on_no_foreign_table_dependencies_t>(f_f, f.omega > fa.omega);
  }

  template <typename Assert, typename Table>
  void join_dynamic_check(const Table& table)
  {
    using CheckResult = sqlpp::check_dynamic_pre_join_t<Table>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using JoinType = decltype(sqlpp::dynamic_join(table));
    using InnerJoinType = decltype(sqlpp::dynamic_inner_join(table));
    using LeftOuterJoinType = decltype(sqlpp::dynamic_left_outer_join(table));
    using RightOuterJoinType = decltype(sqlpp::dynamic_right_outer_join(table));
    using OuterJoinType = decltype(sqlpp::dynamic_outer_join(table));
    using CrossJoinType = decltype(sqlpp::dynamic_cross_join(table));
    using ExpectedReturnType = sqlpp::logic::all_t<
        (Assert::value and sqlpp::is_dynamic_pre_join_t<JoinType>::value and
         sqlpp::is_dynamic_pre_join_t<InnerJoinType>::value and
         sqlpp::is_dynamic_pre_join_t<LeftOuterJoinType>::value and
         sqlpp::is_dynamic_pre_join_t<RightOuterJoinType>::value and
         sqlpp::is_dynamic_pre_join_t<OuterJoinType>::value and sqlpp::is_dynamic_join_t<CrossJoinType>::value) xor
        (std::is_same<JoinType, Assert>::value and std::is_same<InnerJoinType, Assert>::value and
         std::is_same<LeftOuterJoinType, Assert>::value and std::is_same<RightOuterJoinType, Assert>::value and
         std::is_same<OuterJoinType, Assert>::value and std::is_same<CrossJoinType, Assert>::value)>;
    print_type_on_error<JoinType>(ExpectedReturnType{});
    print_type_on_error<InnerJoinType>(ExpectedReturnType{});
    print_type_on_error<LeftOuterJoinType>(ExpectedReturnType{});
    print_type_on_error<RightOuterJoinType>(ExpectedReturnType{});
    print_type_on_error<OuterJoinType>(ExpectedReturnType{});
    print_type_on_error<CrossJoinType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename Lhs, typename Rhs>
  void on_dynamic_check(const Lhs& lhs, const Rhs& rhs)
  {
    using CheckResult = sqlpp::check_dynamic_join_on_t<Lhs, Rhs>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ResultType = decltype(lhs.on(rhs));
    using ExpectedReturnType = sqlpp::logic::all_t<(Assert::value and sqlpp::is_dynamic_join_t<ResultType>::value) xor
                                                   std::is_same<ResultType, Assert>::value>;
    print_type_on_error<ResultType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  void dynamic_join()
  {
    // Prepare a few table aliases for tests
    const auto ta = t.as(sqlpp::alias::a);
    const auto fa = f.as(sqlpp::alias::a);

    // OK
    join_dynamic_check<sqlpp::consistent_t>(t);
    join_dynamic_check<sqlpp::consistent_t>(f);
    join_dynamic_check<sqlpp::consistent_t>(ta);
    join_dynamic_check<sqlpp::consistent_t>(fa);
    join_dynamic_check<sqlpp::consistent_t>(sqlpp::verbatim_table("tab_sample"));
    join_dynamic_check<sqlpp::consistent_t>(sqlpp::verbatim_table("tab_sample").as(sqlpp::alias::a));

    // Try a bunch of non-tables
    join_dynamic_check<sqlpp::assert_dynamic_pre_join_table_t>(7);
    join_dynamic_check<sqlpp::assert_dynamic_pre_join_table_t>(t.alpha);
    join_dynamic_check<sqlpp::assert_dynamic_pre_join_table_t>(t.beta);
    join_dynamic_check<sqlpp::assert_dynamic_pre_join_table_t>(t.gamma);
    join_dynamic_check<sqlpp::assert_dynamic_pre_join_table_t>(t.delta);

    // Try (pre) joins
    join_dynamic_check<sqlpp::assert_dynamic_pre_join_table_t>(t.join(f));
    join_dynamic_check<sqlpp::assert_dynamic_pre_join_no_join_t>(t.cross_join(f));

    // Prepare a dynamic_pre_joins for tests:
    const auto tj = dynamic_join(t);
    const auto fj = dynamic_join(f);
    const auto vj = dynamic_join(sqlpp::verbatim_table("tab_sample"));

    // OK dynamic_join.on()
    on_dynamic_check<sqlpp::consistent_t>(tj, t.alpha > f.omega);
    on_dynamic_check<sqlpp::consistent_t>(fj, t.alpha < f.omega);

    // Try dynamic_join.on(non-expression)
    on_dynamic_check<sqlpp::assert_on_is_expression_t>(tj, true);
    on_dynamic_check<sqlpp::assert_on_is_expression_t>(tj, 7);
    on_dynamic_check<sqlpp::assert_on_is_expression_t>(tj, t);

    // Try dynamic_join.on(non-boolean)
    on_dynamic_check<sqlpp::assert_on_is_boolean_expression_t>(tj, t.alpha);
    on_dynamic_check<sqlpp::assert_on_is_boolean_expression_t>(tj, t.beta);
    on_dynamic_check<sqlpp::assert_on_is_boolean_expression_t>(tj, f.omega);

    // OK dynamic_join.on(foreign-table)
    on_dynamic_check<sqlpp::consistent_t>(tj, ta.alpha != 0);
    on_dynamic_check<sqlpp::consistent_t>(tj, t.gamma);
    on_dynamic_check<sqlpp::consistent_t>(tj, f.omega > fa.omega);
    on_dynamic_check<sqlpp::consistent_t>(vj, t.alpha < f.omega);
  }
}

int main(int, char* [])
{
  static_join();
  dynamic_join();
}

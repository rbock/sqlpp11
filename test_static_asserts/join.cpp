/*
 * Copyright (c) 2015-2016, Roland Bock
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

#include <iostream>
#include "MockDb.h"
#include "Sample.h"
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
    using CheckResult = sqlpp::check_cross_join_t<Lhs, Rhs>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using JoinType = decltype(join(lhs, rhs));
    using InnerJoinType = decltype(inner_join(lhs, rhs));
    using LeftOuterJoinType = decltype(left_outer_join(lhs, rhs));
    using RightOuterJoinType = decltype(right_outer_join(lhs, rhs));
    using OuterJoinType = decltype(outer_join(lhs, rhs));
    using ExpectedReturnType =
        sqlpp::logic::all_t<Assert::value xor (std::is_same<JoinType, sqlpp::bad_statement>::value and
                                               std::is_same<InnerJoinType, sqlpp::bad_statement>::value and
                                               std::is_same<LeftOuterJoinType, sqlpp::bad_statement>::value and
                                               std::is_same<RightOuterJoinType, sqlpp::bad_statement>::value and
                                               std::is_same<OuterJoinType, sqlpp::bad_statement>::value)>;
    print_type_on_error<JoinType>(ExpectedReturnType{});
    print_type_on_error<InnerJoinType>(ExpectedReturnType{});
    print_type_on_error<LeftOuterJoinType>(ExpectedReturnType{});
    print_type_on_error<RightOuterJoinType>(ExpectedReturnType{});
    print_type_on_error<OuterJoinType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  /*
  template <typename Assert, typename Expression>
  void join_dynamic_check(const Expression& expression)
  {
    static auto db = MockDb{};
    using CheckResult = sqlpp::check_join_dynamic_t<decltype(db), Expression>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(dynamic_select(db, t.alpha).dynamic_join(expression));
    using ExpectedReturnType =
        sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, sqlpp::bad_statement>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }
  */

  void static_join()
  {
    // OK: Join two different tables
    join_static_check<sqlpp::consistent_t>(t, f);
    join_static_check<sqlpp::consistent_t>(t, f.as(sqlpp::alias::a));
    join_static_check<sqlpp::consistent_t>(t.as(sqlpp::alias::a), f.as(sqlpp::alias::b));

    // OK: Self join
    join_static_check<sqlpp::consistent_t>(t.as(sqlpp::alias::a), t.as(sqlpp::alias::b));
    join_static_check<sqlpp::consistent_t>(t, t.as(sqlpp::alias::b));
    join_static_check<sqlpp::consistent_t>(t.as(sqlpp::alias::a), t);

    // Prepare a join for tests:
    const auto j = join(t.as(sqlpp::alias::a), t.as(sqlpp::alias::b)).unconditionally();

    // OK: Add a third table
    join_static_check<sqlpp::consistent_t>(j, f);
    join_static_check<sqlpp::consistent_t>(j, t.as(sqlpp::alias::c));
    join_static_check<sqlpp::consistent_t>(j, t);

    // Try a bunch of non-tables
    join_static_check<sqlpp::assert_cross_join_rhs_table_t>(t, 7);
    join_static_check<sqlpp::assert_cross_join_rhs_table_t>(t, t.alpha);
    join_static_check<sqlpp::assert_cross_join_rhs_table_t>(t, t.beta);
    join_static_check<sqlpp::assert_cross_join_rhs_table_t>(t, t.gamma);
    join_static_check<sqlpp::assert_cross_join_rhs_table_t>(t, t.delta);

    join_static_check<sqlpp::assert_cross_join_lhs_table_t>(7, t);
    join_static_check<sqlpp::assert_cross_join_lhs_table_t>(t.alpha, t);
    join_static_check<sqlpp::assert_cross_join_lhs_table_t>(t.beta, t);
    join_static_check<sqlpp::assert_cross_join_lhs_table_t>(t.gamma, t);
    join_static_check<sqlpp::assert_cross_join_lhs_table_t>(t.delta, t);

    // Try to join with join (rhs)
    join_static_check<sqlpp::assert_cross_join_rhs_no_join_t>(t, j);
    join_static_check<sqlpp::assert_cross_join_rhs_no_join_t>(f, j);
    join_static_check<sqlpp::assert_cross_join_rhs_no_join_t>(t.as(sqlpp::alias::left), j);

    // Try to join identical table names
    join_static_check<sqlpp::assert_cross_join_unique_names_t>(t, t);
    join_static_check<sqlpp::assert_cross_join_unique_names_t>(f, f);
    join_static_check<sqlpp::assert_cross_join_unique_names_t>(t.as(f), f);
    join_static_check<sqlpp::assert_cross_join_unique_names_t>(t, f.as(t));
    join_static_check<sqlpp::assert_cross_join_unique_names_t>(t.as(sqlpp::alias::a), f.as(sqlpp::alias::a));
    join_static_check<sqlpp::assert_cross_join_unique_names_t>(j, f.as(sqlpp::alias::a));
    join_static_check<sqlpp::assert_cross_join_unique_names_t>(j, f.as(sqlpp::alias::b));
    join_static_check<sqlpp::assert_cross_join_unique_names_t>(j, t.as(sqlpp::alias::a));
    join_static_check<sqlpp::assert_cross_join_unique_names_t>(j, t.as(sqlpp::alias::b));
  }

  void dynamic_join()
  {
    /*
    // OK
    join_dynamic_check<sqlpp::consistent_t>(t);
    join_dynamic_check<sqlpp::consistent_t>(t.join(f).unconditionally());
    join_dynamic_check<sqlpp::consistent_t>(t.join(f).on(t.alpha > f.omega));

    // Try a bunch of non-tables
    join_dynamic_check<sqlpp::assert_join_table_t>(7);
    join_dynamic_check<sqlpp::assert_join_table_t>(t.alpha);
    join_dynamic_check<sqlpp::assert_join_table_t>(t.beta);
    join_dynamic_check<sqlpp::assert_join_table_t>(t.gamma);
    join_dynamic_check<sqlpp::assert_join_table_t>(t.delta);

    // Try cross joins (missing condition)
    join_dynamic_check<sqlpp::assert_join_not_cross_join_t>(t.join(f));
    */
  }
}

int main(int, char* [])
{
  static_join();
  dynamic_join();
}

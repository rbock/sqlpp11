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
  auto print_type_on_error(std::true_type) -> void
  {
  }

  template <typename T>
  auto print_type_on_error(std::false_type) -> void
  {
    T::_print_me_;
  }

  template <typename Assert, typename Expression>
  auto having_static_check(const Expression& expression) -> void
  {
    using CheckResult = sqlpp::check_having_static_t<Expression>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(select(all_of(t)).from(t).unconditionally().group_by(t.alpha).having(expression));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename Expression>
  auto having_dynamic_check(const Expression& expression) -> void
  {
    static auto db = MockDb{};
    using CheckResult = sqlpp::check_having_dynamic_t<decltype(db), Expression>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType =
        decltype(dynamic_select(db, all_of(t)).from(t).unconditionally().group_by(t.alpha).dynamic_having(expression));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  auto static_having() -> void
  {
    // OK
    having_static_check<sqlpp::consistent_t>(t.gamma);
    having_static_check<sqlpp::consistent_t>(t.gamma == true);

    // OK using aggregate functions in having
    having_static_check<sqlpp::consistent_t>(count(t.alpha) > 0);
    having_static_check<sqlpp::consistent_t>(t.gamma and count(t.alpha) > 0);
    having_static_check<sqlpp::consistent_t>(case_when(count(t.alpha) > 0).then(t.gamma).else_(not t.gamma));

    // Try assignment as condition
    having_static_check<sqlpp::assert_having_boolean_expression_t>(t.gamma = true);

    // Try non-boolean expression
    having_static_check<sqlpp::assert_having_boolean_expression_t>(t.alpha);

    // Try builtin bool
    having_static_check<sqlpp::assert_having_not_cpp_bool_t>(true);
    having_static_check<sqlpp::assert_having_not_cpp_bool_t>(17 > 3);

    // Try some other types as expressions
    having_static_check<sqlpp::assert_having_boolean_expression_t>("true");
    having_static_check<sqlpp::assert_having_boolean_expression_t>(17);
    having_static_check<sqlpp::assert_having_boolean_expression_t>('c');
    having_static_check<sqlpp::assert_having_boolean_expression_t>(nullptr);
    having_static_check<sqlpp::assert_having_boolean_expression_t>(t.alpha.as(t.beta));
  }

  auto dynamic_having() -> void
  {
    // OK
    having_dynamic_check<sqlpp::consistent_t>(t.gamma);
    having_dynamic_check<sqlpp::consistent_t>(t.gamma == true);

    // OK using aggregate functions in having
    having_dynamic_check<sqlpp::consistent_t>(count(t.alpha) > 0);
    having_dynamic_check<sqlpp::consistent_t>(t.gamma and count(t.alpha) > 0);
    having_dynamic_check<sqlpp::consistent_t>(case_when(count(t.alpha) > 0).then(t.gamma).else_(not t.gamma));

    // Try assignment as condition
    having_dynamic_check<sqlpp::assert_having_boolean_expression_t>(t.gamma = true);

    // Try non-boolean expression
    having_dynamic_check<sqlpp::assert_having_boolean_expression_t>(t.alpha);

    // Try builtin bool
    having_dynamic_check<sqlpp::assert_having_not_cpp_bool_t>(true);
    having_dynamic_check<sqlpp::assert_having_not_cpp_bool_t>(17 > 3);

    // Try some other types as expressions
    having_dynamic_check<sqlpp::assert_having_boolean_expression_t>("true");
    having_dynamic_check<sqlpp::assert_having_boolean_expression_t>(17);
    having_dynamic_check<sqlpp::assert_having_boolean_expression_t>('c');
    having_dynamic_check<sqlpp::assert_having_boolean_expression_t>(nullptr);
    having_dynamic_check<sqlpp::assert_having_boolean_expression_t>(t.alpha.as(t.beta));

    // Try dynamic_having on a non-dynamic select
    using CheckResult = sqlpp::check_having_dynamic_t<void, sqlpp::boolean_operand>;
    using ExpectedCheckResult = std::is_same<CheckResult, sqlpp::assert_having_dynamic_statement_dynamic_t>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(select(all_of(t)).from(t).dynamic_having());
    using ExpectedReturnType = std::is_same<ReturnType, sqlpp::assert_having_dynamic_statement_dynamic_t>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename Statement, typename HavingCondition>
  auto static_consistency_check(const Statement statement, const HavingCondition condtion) -> void
  {
    using CheckResult = sqlpp::consistency_check_t<decltype(statement.having(condtion))>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");
  }

  auto consistency_check() -> void
  {
    const auto select_without_group_by = select(all_of(t)).from(t).unconditionally();

    // OK
    static_consistency_check<sqlpp::consistent_t>(select_without_group_by, avg(t.alpha) > 17);

    // Try non aggregate
    static_consistency_check<sqlpp::assert_having_no_non_aggregates_t>(select_without_group_by, t.alpha > 17);
    static_consistency_check<sqlpp::assert_having_no_non_aggregates_t>(select_without_group_by,
                                                                       count(t.alpha) > 3 and t.alpha > 17);

    // Try foreign table
    static_consistency_check<sqlpp::assert_having_no_unknown_tables_t>(select_without_group_by, f.omega > 17);

    const auto select_with_group_by = select(t.alpha).from(t).unconditionally().group_by(t.alpha);

    // OK
    static_consistency_check<sqlpp::consistent_t>(select_with_group_by, avg(t.alpha) > 17);
    static_consistency_check<sqlpp::consistent_t>(select_with_group_by, t.alpha > 17);
    static_consistency_check<sqlpp::consistent_t>(select_with_group_by, count(t.alpha) > 3 and t.alpha > 17);

    // Try non aggregate
    static_consistency_check<sqlpp::assert_having_no_non_aggregates_t>(select_with_group_by, t.beta > "17");
    static_consistency_check<sqlpp::assert_having_no_non_aggregates_t>(select_with_group_by,
                                                                       count(t.beta) > 3 and t.beta > "17");

    // Try foreign table
    static_consistency_check<sqlpp::assert_having_no_unknown_tables_t>(select_with_group_by, f.omega > 17);
  }
}

int main(int, char* [])
{
  static_having();
  dynamic_having();
  consistency_check();
}

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

    using ReturnType = decltype(select(all_of(t)).from(t).unconditionally().group_by(t.id).having(expression));
    using ExpectedReturnType = sqlpp::logic::all<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  auto static_having() -> void
  {
    // OK
    having_static_check<sqlpp::consistent_t>(t.boolNn);
    having_static_check<sqlpp::consistent_t>(t.boolNn == true);

    // OK using aggregate functions in having
    having_static_check<sqlpp::consistent_t>(count(t.id) > 0);
    having_static_check<sqlpp::consistent_t>(t.boolNn and count(t.id) > 0);
    having_static_check<sqlpp::consistent_t>(case_when(count(t.id) > 0).then(t.boolNn).else_(not t.boolNn));

    // Try assignment as condition
    having_static_check<sqlpp::assert_having_boolean_expression_t>(t.boolNn = true);

    // Try non-boolean expression
    having_static_check<sqlpp::assert_having_boolean_expression_t>(t.id);

    // Try builtin bool
    having_static_check<sqlpp::assert_having_not_cpp_bool_t>(true);
    having_static_check<sqlpp::assert_having_not_cpp_bool_t>(17 > 3);

    // Try some other types as expressions
    having_static_check<sqlpp::assert_having_boolean_expression_t>("true");
    having_static_check<sqlpp::assert_having_boolean_expression_t>(17);
    having_static_check<sqlpp::assert_having_boolean_expression_t>('c');
    having_static_check<sqlpp::assert_having_boolean_expression_t>(nullptr);
    having_static_check<sqlpp::assert_having_boolean_expression_t>(t.id.as(t.textN));
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
    static_consistency_check<sqlpp::consistent_t>(select_without_group_by, avg(t.id) > 17);
    static_consistency_check<sqlpp::consistent_t>(select_without_group_by, avg(t.id) > parameter(t.id));

    // Try non aggregate
    static_consistency_check<sqlpp::assert_having_all_aggregates_t>(select_without_group_by, t.id > 17);
    static_consistency_check<sqlpp::assert_having_all_aggregates_t>(select_without_group_by,
                                                                    count(t.id) > 3 and t.id > 17);

    // Try foreign table
    static_consistency_check<sqlpp::assert_having_no_unknown_tables_t>(select_without_group_by, f.doubleN > 17);

    const auto select_with_group_by = select(t.id).from(t).unconditionally().group_by(t.id);

    // OK
    static_consistency_check<sqlpp::consistent_t>(select_with_group_by, avg(t.id) > 17);
    static_consistency_check<sqlpp::consistent_t>(select_with_group_by, t.id > 17);
    static_consistency_check<sqlpp::consistent_t>(select_with_group_by, count(t.id) > 3 and t.id > 17);

    // Try non aggregate
    static_consistency_check<sqlpp::assert_having_all_aggregates_t>(select_with_group_by, t.textN > "17");
    static_consistency_check<sqlpp::assert_having_all_aggregates_t>(select_with_group_by,
                                                                       count(t.textN) > 3 and t.textN > "17");

    // Try foreign table
    static_consistency_check<sqlpp::assert_having_no_unknown_tables_t>(select_with_group_by, f.doubleN > 17);
  }
}

int main(int, char* [])
{
  static_having();
#warning: Add tests with optional expressions?
  consistency_check();
}

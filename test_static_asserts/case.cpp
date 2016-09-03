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

#include "MockDb.h"
#include "Sample.h"
#include <iostream>
#include <sqlpp11/sqlpp11.h>

namespace
{
  constexpr auto t = test::TabBar{};

  template <typename T>
  void print_type_on_error(std::true_type)
  {
  }

  template <typename T>
  void print_type_on_error(std::false_type)
  {
    T::_print_me_;
  }

  template <typename Assert, typename When>
  void when_check(const When& when)
  {
    using CheckResult = sqlpp::check_case_when_t<When>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(sqlpp::case_when(when));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename When, typename Then>
  void then_check(const When& when, const Then& then)
  {
    using CheckResult = sqlpp::check_case_then_t<Then>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(sqlpp::case_when(when).then(then));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename When, typename Then, typename Else>
  void else_check(const When& when, const Then& then, const Else& else_)
  {
    using CheckResult = sqlpp::check_case_else_t<sqlpp::wrap_operand_t<Then>, Else>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(sqlpp::case_when(when).then(then).else_(else_));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  void when()
  {
    // OK
    when_check<sqlpp::consistent_t>(t.gamma);
    when_check<sqlpp::consistent_t>(t.gamma == true);
    when_check<sqlpp::consistent_t>(count(t.alpha) > 0);

    // Try assignment as "when"
    when_check<sqlpp::assert_case_when_boolean_expression_t>(t.gamma = true);

    // Try non-boolean expression as "when"
    when_check<sqlpp::assert_case_when_boolean_expression_t>(t.alpha);

    // Try some other types as "when"
    when_check<sqlpp::assert_case_when_boolean_expression_t>("true");
    when_check<sqlpp::assert_case_when_boolean_expression_t>(42);
    when_check<sqlpp::assert_case_when_boolean_expression_t>('c');
    when_check<sqlpp::assert_case_when_boolean_expression_t>(nullptr);

    // Try to use a table as "when"
    when_check<sqlpp::assert_case_when_boolean_expression_t>(t);

    // Try to use an alias as "when"
    when_check<sqlpp::assert_case_when_boolean_expression_t>(t.gamma.as(t.beta));
  }

  void then()
  {
    // OK
    then_check<sqlpp::consistent_t>(t.gamma, t.gamma);
    then_check<sqlpp::consistent_t>(t.gamma, t.gamma == true);
    then_check<sqlpp::consistent_t>(t.gamma, count(t.alpha) > 0);
    then_check<sqlpp::consistent_t>(t.gamma, t.alpha);
    then_check<sqlpp::consistent_t>(t.gamma, "true");
    then_check<sqlpp::consistent_t>(t.gamma, 42);
    then_check<sqlpp::consistent_t>(t.gamma, 'c');
    then_check<sqlpp::consistent_t>(t.gamma, nullptr);

    // Try to use an assignment as "then"
    then_check<sqlpp::assert_case_then_expression_t>(t.gamma, t.gamma = true);

    // Try to use a table as "then"
    then_check<sqlpp::assert_case_then_expression_t>(t.gamma, t);

    // Try to use an alias as "then"
    then_check<sqlpp::assert_case_then_expression_t>(t.gamma, t.alpha.as(t.beta));
  }

  void else_()
  {
    // OK
    else_check<sqlpp::consistent_t>(t.gamma, t.gamma, t.gamma);
    else_check<sqlpp::consistent_t>(t.gamma, t.alpha, 42);
    else_check<sqlpp::consistent_t>(t.gamma, t.beta, "twentyseven");

    // Try to use an assignment as "else"
    else_check<sqlpp::assert_case_else_expression_t>(t.gamma, t.alpha, t.alpha = 7);

    // Try to use a table as "else"
    else_check<sqlpp::assert_case_else_expression_t>(t.gamma, t.alpha, t);

    // Try to use an alias as "else"
    else_check<sqlpp::assert_case_else_expression_t>(t.gamma, t.alpha, t.alpha.as(t.beta));
  }
}

int main(int, char* [])
{
  when();
  then();
  else_();
}

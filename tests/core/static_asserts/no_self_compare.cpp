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
#include <MockDb.h>
#include "AssertTables.h"
#include <sqlpp11/sqlpp11.h>

namespace
{
  constexpr auto t = test::TabAllTypes{};

  template <typename T>
  void print_type_on_error(std::true_type)
  {
  }

  template <typename T>
  void print_type_on_error(std::false_type)
  {
    T::_print_me_;
  }

  template <typename Assert, typename Operand>
  void static_check_self_compare(const Operand& operand)
  {
    using CheckResult = sqlpp::check_comparison_t<Operand, Operand>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    static_assert(ExpectedCheckResult::value, "Unexpected check result");
    print_type_on_error<CheckResult>(ExpectedCheckResult{});

    using ReturnType = sqlpp::detail::make_type_set_t<decltype(operand < operand), decltype(operand <= operand),
                                                      decltype(operand == operand), decltype(operand != operand),
                                                      decltype(operand >= operand), decltype(operand > operand)>;
    using ExpectedReturnType = sqlpp::logic::all_t<
        Assert::value xor
        std::is_same<ReturnType, sqlpp::detail::type_set<sqlpp::bad_expression<sqlpp::boolean>>>::value>;
    static_assert(ExpectedReturnType::value, "Unexpected return type");
    print_type_on_error<ReturnType>(ExpectedReturnType{});
  }

  void disallowed_self_comparison()
  {
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.someString);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.someInt);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.someFloat);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.someBool);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.someDayPoint);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.someTimePoint);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.otherString);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.otherInt);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.otherFloat);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.otherBool);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.otherDayPoint);
    static_check_self_compare<sqlpp::assert_comparison_lhs_rhs_differ_t>(t.otherTimePoint);
  }
}

int main(int, char* [])
{
  // t.someString == t.someString;
  disallowed_self_comparison();
}

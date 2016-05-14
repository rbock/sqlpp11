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
  void static_check_comparison(const Operand& operand)
  {
    using CheckResult = sqlpp::check_comparison_t<decltype(t.someString), Operand>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    static_assert(ExpectedCheckResult::value, "Unexpected check result");
    print_type_on_error<CheckResult>(ExpectedCheckResult{});

    using ReturnType = sqlpp::detail::make_type_set_t<
        decltype(t.someString < operand), decltype(t.someString <= operand), decltype(t.someString == operand),
        decltype(t.someString != operand), decltype(t.someString >= operand), decltype(t.someString > operand),
        decltype(t.someString.in(operand)), decltype(t.someString.in(operand, operand)),
        decltype(t.someString.not_in(operand)), decltype(t.someString.not_in(operand, operand))>;
    using ExpectedReturnType = sqlpp::logic::all_t<
        Assert::value xor
        std::is_same<ReturnType, sqlpp::detail::type_set<sqlpp::bad_expression<sqlpp::boolean>>>::value>;
    static_assert(ExpectedReturnType::value, "Unexpected return type");
    print_type_on_error<ReturnType>(ExpectedReturnType{});
  }

  void allowed_comparands()
  {
    static_check_comparison<sqlpp::consistent_t>("");
    // static_check_comparison<sqlpp::consistent_t>('d'); // not today
    static_check_comparison<sqlpp::consistent_t>(std::string(""));
    static_check_comparison<sqlpp::consistent_t>(t.otherString);
  }

  void disallowed_comparands()
  {
    static_check_comparison<sqlpp::assert_comparison_rhs_is_valid_operand_t>(17);
    static_check_comparison<sqlpp::assert_comparison_rhs_is_valid_operand_t>(17.4);
    static_check_comparison<sqlpp::assert_comparison_rhs_is_expression_t>(t);
    static_check_comparison<sqlpp::assert_comparison_rhs_is_valid_operand_t>(t.someBool);
    static_check_comparison<sqlpp::assert_comparison_rhs_is_valid_operand_t>(t.someFloat);
    static_check_comparison<sqlpp::assert_comparison_rhs_is_valid_operand_t>(t.someInt);
    static_check_comparison<sqlpp::assert_comparison_rhs_is_valid_operand_t>(t.someDayPoint);
    static_check_comparison<sqlpp::assert_comparison_rhs_is_valid_operand_t>(t.someTimePoint);
  }

  template <typename Expected, typename Expression>
  void static_check_type()
  {
    static_assert(std::is_same<Expected, Expression>::value, "Unexpected check result");
  }

  auto check_expressions() -> void
  {
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(t.someString + 1)>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(t.someString - 1)>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(t.someString - "")>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(t.someString * 1)>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(t.someString * "")>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(t.someString / 1)>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(t.someString / "")>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(t.someString % 1)>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(t.someString % "")>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(-t.someString)>();
    static_check_type<sqlpp::bad_expression<sqlpp::text>, decltype(+t.someString)>();
  }
}

int main(int, char* [])
{
  allowed_comparands();
  disallowed_comparands();
  check_expressions();
}

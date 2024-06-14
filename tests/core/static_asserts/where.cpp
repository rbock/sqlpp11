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

  template <typename Assert, typename Expression>
  void where_static_check(const Expression& expression)
  {
    using CheckResult = sqlpp::check_where_static_t<Expression>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(remove_from(t).where(expression));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

 void static_where()
  {
    // OK
    where_static_check<sqlpp::consistent_t>(t.boolNn);
    where_static_check<sqlpp::consistent_t>(t.boolNn == true);

    // Try assignment as condition
    where_static_check<sqlpp::assert_where_arg_is_boolean_expression_t>(t.boolNn = true);

    // Try non-boolean expression
    where_static_check<sqlpp::assert_where_arg_is_boolean_expression_t>(t.id);

    // Try builtin bool
    where_static_check<sqlpp::assert_where_arg_is_not_cpp_bool_t>(true);
    where_static_check<sqlpp::assert_where_arg_is_not_cpp_bool_t>(17 > 3);

    // Try some other types as expressions
    where_static_check<sqlpp::assert_where_arg_is_boolean_expression_t>("true");
    where_static_check<sqlpp::assert_where_arg_is_boolean_expression_t>(17);
    where_static_check<sqlpp::assert_where_arg_is_boolean_expression_t>('c');
    where_static_check<sqlpp::assert_where_arg_is_boolean_expression_t>(nullptr);
    where_static_check<sqlpp::assert_where_arg_is_boolean_expression_t>(t.id.as(t.textN));

    // Try using aggregate functions in where
    where_static_check<sqlpp::assert_where_arg_contains_no_aggregate_functions_t>(count(t.id) > 0);
    where_static_check<sqlpp::assert_where_arg_contains_no_aggregate_functions_t>(t.boolNn and count(t.id) > 0);
    where_static_check<sqlpp::assert_where_arg_contains_no_aggregate_functions_t>(
        case_when(count(t.id) > 0).then(t.boolNn).else_(not t.boolNn));
  }

}

int main(int, char* [])
{
  static_where();
#warning: Add test with optional expressions?
}

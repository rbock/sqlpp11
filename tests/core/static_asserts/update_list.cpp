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

  template <typename Assert, typename... Expressions>
  void update_set_static_check(const Expressions&... expressions)
  {
    using CheckResult = sqlpp::check_update_static_set_t<Expressions...>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(update(t).set(expressions...));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  void static_update_set()
  {
    // OK
    update_set_static_check<sqlpp::consistent_t>(t.boolNn = true);
    update_set_static_check<sqlpp::consistent_t>(t.boolNn = true, t.textN = "");

    // Try to update nothing
    update_set_static_check<sqlpp::assert_update_set_count_args_t>();

    // Try condition as assignment
    update_set_static_check<sqlpp::assert_update_set_assignments_t>(t.boolNn == true);

    // Try duplicate columns
    update_set_static_check<sqlpp::assert_update_set_no_duplicates_t>(t.boolNn = true, t.boolNn = false);
    update_set_static_check<sqlpp::assert_update_set_no_duplicates_t>(t.boolNn = true, t.textN = "", t.boolNn = false);

    // Try to update prohibited columns
    update_set_static_check<sqlpp::assert_update_set_allowed_t>(t.id = 42);

    // Try to update multiple tables at once
    update_set_static_check<sqlpp::assert_update_set_single_table_t>(t.boolNn = true, f.doubleN = 7);
  }

}

int main(int, char* [])
{
  static_update_set();
#warning: add tests with optional 
}

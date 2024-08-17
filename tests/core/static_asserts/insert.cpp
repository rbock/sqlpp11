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

  template <typename Assert, typename... Assignments>
  void set_static_check(const Assignments&... assignments)
  {
    using CheckResult = sqlpp::check_insert_static_set_t<Assignments...>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(insert_into(t).set(assignments...));
    using ExpectedReturnType = sqlpp::logic::all<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  // column id is not allowed, column boolNn is required
  void static_set()
  {
    // OK
    set_static_check<sqlpp::consistent_t>(t.boolNn = true);
    set_static_check<sqlpp::consistent_t>(t.textN = "fortytwo", t.boolNn = true);
    set_static_check<sqlpp::consistent_t>(t.textN = "fortytwo", t.boolNn = true, t.intN = 42);
    set_static_check<sqlpp::consistent_t>(t.intN = 42, t.textN = "fortytwo", t.boolNn = true);
    set_static_check<sqlpp::consistent_t>(t.intN = 42, t.boolNn = true, t.textN = "fortytwo");
    set_static_check<sqlpp::consistent_t>(t.boolNn = true, t.intN = 42, t.textN = "fortytwo");

    // Try setting id
    set_static_check<sqlpp::assert_insert_set_allowed_t>(t.id = 17, t.textN = "whatever");
    set_static_check<sqlpp::assert_insert_set_allowed_t>(t.textN = "whatever", t.id = 17);

    // Try omitting boolNn
    set_static_check<sqlpp::assert_insert_static_set_all_required_t>(t.intN = 42);
    set_static_check<sqlpp::assert_insert_static_set_all_required_t>(t.textN = "whatever");

    // Try no arguments
    set_static_check<sqlpp::assert_insert_static_set_count_args_t>();

    // Try none-assignment arguments
    set_static_check<sqlpp::assert_insert_set_assignments_t>(t.intN == 42, t.intN = 42, t.textN = "fortytwo",
                                                             t.boolNn = true);
    set_static_check<sqlpp::assert_insert_set_assignments_t>(17, t.intN = 42, t.textN = "fortytwo", t.boolNn = true);
    set_static_check<sqlpp::assert_insert_set_assignments_t>(t.intN = 42, t.textN = "fortytwo", t.boolNn = true, "EEEK");

    // Try duplicates
    set_static_check<sqlpp::assert_insert_set_no_duplicates_t>(t.intN = 41, t.intN = 42, t.textN = "fortytwo",
                                                               t.boolNn = true);
    set_static_check<sqlpp::assert_insert_set_no_duplicates_t>(t.textN = "fortyone", t.intN = 41, t.textN = "fortytwo",
                                                               t.boolNn = true);
    set_static_check<sqlpp::assert_insert_set_no_duplicates_t>(t.boolNn = false, t.intN = 41, t.textN = "fortytwo",
                                                               t.boolNn = true);

    // Try multiple tables
    set_static_check<sqlpp::assert_insert_set_single_table_t>(f.doubleN = 41, t.boolNn = true);
  }

}

int main(int, char* [])
{
  static_set();
#warning add tests with optional expressions
}

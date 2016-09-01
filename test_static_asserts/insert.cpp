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
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename... Assignments>
  void set_dynamic_check(const Assignments&... assignments)
  {
    static auto db = MockDb{};
    using CheckResult = sqlpp::check_insert_dynamic_set_t<decltype(db), Assignments...>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    static_assert(ExpectedCheckResult::value, "Unexpected check result");
    print_type_on_error<CheckResult>(ExpectedCheckResult{});

    using ReturnType = decltype(dynamic_insert_into(db, t).dynamic_set(assignments...));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    static_assert(ExpectedReturnType::value, "Unexpected return type");
    print_type_on_error<ReturnType>(ExpectedReturnType{});
  }

  // column alpha is not allowed, column gamma is required
  void static_set()
  {
    // OK
    set_static_check<sqlpp::consistent_t>(t.gamma = true);
    set_static_check<sqlpp::consistent_t>(t.beta = "fortytwo", t.gamma = true);
    set_static_check<sqlpp::consistent_t>(t.beta = "fortytwo", t.gamma = true, t.delta = 42);
    set_static_check<sqlpp::consistent_t>(t.delta = 42, t.beta = "fortytwo", t.gamma = true);
    set_static_check<sqlpp::consistent_t>(t.delta = 42, t.gamma = true, t.beta = "fortytwo");
    set_static_check<sqlpp::consistent_t>(t.gamma = true, t.delta = 42, t.beta = "fortytwo");

    // Try setting alpha
    set_static_check<sqlpp::assert_insert_set_allowed_t>(t.alpha = 17, t.beta = "whatever");
    set_static_check<sqlpp::assert_insert_set_allowed_t>(t.beta = "whatever", t.alpha = 17);

    // Try omitting gamma
    set_static_check<sqlpp::assert_insert_static_set_all_required_t>(t.delta = 42);
    set_static_check<sqlpp::assert_insert_static_set_all_required_t>(t.beta = "whatever");

    // Try no arguments
    set_static_check<sqlpp::assert_insert_static_set_count_args_t>();

    // Try none-assignment arguments
    set_static_check<sqlpp::assert_insert_set_assignments_t>(t.delta == 42, t.delta = 42, t.beta = "fortytwo",
                                                             t.gamma = true);
    set_static_check<sqlpp::assert_insert_set_assignments_t>(17, t.delta = 42, t.beta = "fortytwo", t.gamma = true);
    set_static_check<sqlpp::assert_insert_set_assignments_t>(t.delta = 42, t.beta = "fortytwo", t.gamma = true, "EEEK");

    // Try duplicates
    set_static_check<sqlpp::assert_insert_set_no_duplicates_t>(t.delta = 41, t.delta = 42, t.beta = "fortytwo",
                                                               t.gamma = true);
    set_static_check<sqlpp::assert_insert_set_no_duplicates_t>(t.beta = "fortyone", t.delta = 41, t.beta = "fortytwo",
                                                               t.gamma = true);
    set_static_check<sqlpp::assert_insert_set_no_duplicates_t>(t.gamma = false, t.delta = 41, t.beta = "fortytwo",
                                                               t.gamma = true);

    // Try multiple tables
    set_static_check<sqlpp::assert_insert_set_single_table_t>(f.omega = 41, t.gamma = true);
  }

  // column alpha is not allowed, column gamma is required
  void dynamic_set()
  {
    // OK
    set_dynamic_check<sqlpp::consistent_t>(t.gamma = true);
    set_dynamic_check<sqlpp::consistent_t>(t.beta = "fortytwo", t.gamma = true);
    set_dynamic_check<sqlpp::consistent_t>(t.beta = "fortytwo", t.gamma = true, t.delta = 42);
    set_dynamic_check<sqlpp::consistent_t>(t.delta = 42, t.beta = "fortytwo", t.gamma = true);
    set_dynamic_check<sqlpp::consistent_t>(t.delta = 42, t.gamma = true, t.beta = "fortytwo");
    set_dynamic_check<sqlpp::consistent_t>(t.gamma = true, t.delta = 42, t.beta = "fortytwo");

    // Try setting alpha
    set_dynamic_check<sqlpp::assert_insert_set_allowed_t>(t.alpha = 17, t.beta = "whatever");
    set_dynamic_check<sqlpp::assert_insert_set_allowed_t>(t.beta = "whatever", t.alpha = 17);

    // Omitting gamma is OK in the dynamic case, since we have to assume that it gets added later
    set_dynamic_check<sqlpp::consistent_t>(t.delta = 42);
    set_dynamic_check<sqlpp::consistent_t>(t.beta = "whatever");

    // Same with no arguments
    set_dynamic_check<sqlpp::consistent_t>();

    // Try none-assignment arguments
    set_dynamic_check<sqlpp::assert_insert_set_assignments_t>(t.delta == 42, t.delta = 42, t.beta = "fortytwo",
                                                              t.gamma = true);
    set_dynamic_check<sqlpp::assert_insert_set_assignments_t>(17, t.delta = 42, t.beta = "fortytwo", t.gamma = true);
    set_dynamic_check<sqlpp::assert_insert_set_assignments_t>(t.delta = 42, t.beta = "fortytwo", t.gamma = true,
                                                              "EEEK");

    // Try duplicates
    set_dynamic_check<sqlpp::assert_insert_set_no_duplicates_t>(t.delta = 41, t.delta = 42, t.beta = "fortytwo",
                                                                t.gamma = true);
    set_dynamic_check<sqlpp::assert_insert_set_no_duplicates_t>(t.beta = "fortyone", t.delta = 41, t.beta = "fortytwo",
                                                                t.gamma = true);
    set_dynamic_check<sqlpp::assert_insert_set_no_duplicates_t>(t.gamma = false, t.delta = 41, t.beta = "fortytwo",
                                                                t.gamma = true);

    // Try multiple tables
    set_dynamic_check<sqlpp::assert_insert_set_single_table_t>(f.omega = 41, t.gamma = true);

    // Try dynamic_set on a non-dynamic insert
    using CheckResult = sqlpp::check_insert_dynamic_set_t<void>;
    using ExpectedCheckResult = std::is_same<CheckResult, sqlpp::assert_insert_dynamic_set_statement_dynamic_t>;
    static_assert(ExpectedCheckResult::value, "Unexpected check result");
    print_type_on_error<CheckResult>(ExpectedCheckResult{});

    using ReturnType = decltype(insert_into(t).dynamic_set());
    using ExpectedReturnType = std::is_same<ReturnType, sqlpp::assert_insert_dynamic_set_statement_dynamic_t>;
    static_assert(ExpectedReturnType::value, "Unexpected return type");
    print_type_on_error<ReturnType>(ExpectedReturnType{});
  }
}

int main(int, char* [])
{
  static_set();
  dynamic_set();
}

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

  template <typename Assert, typename Expression>
  void from_static_check(const Expression& expression)
  {
    using CheckResult = sqlpp::check_from_static_t<Expression>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(select(t.alpha).from(expression));
    using ExpectedReturnType =
        sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, sqlpp::bad_statement>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename Expression>
  void from_dynamic_check(const Expression& expression)
  {
    static auto db = MockDb{};
    using CheckResult = sqlpp::check_from_dynamic_t<decltype(db), Expression>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(dynamic_select(db, t.alpha).dynamic_from(expression));
    using ExpectedReturnType =
        sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, sqlpp::bad_statement>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  void static_from()
  {
    // OK
    from_static_check<sqlpp::consistent_t>(t);
    from_static_check<sqlpp::consistent_t>(t.cross_join(f));
    from_static_check<sqlpp::consistent_t>(t.join(f).on(t.alpha > f.omega));

    // Try a bunch of non-tables
    from_static_check<sqlpp::assert_from_table_t>(7);
    from_static_check<sqlpp::assert_from_table_t>(t.alpha);
    from_static_check<sqlpp::assert_from_table_t>(t.beta);
    from_static_check<sqlpp::assert_from_table_t>(t.gamma);
    from_static_check<sqlpp::assert_from_table_t>(t.delta);

    // Try cross joins (missing condition)
    from_static_check<sqlpp::assert_from_not_pre_join_t>(t.join(f));
  }

  void dynamic_from()
  {
    // OK
    from_dynamic_check<sqlpp::consistent_t>(t);
    from_dynamic_check<sqlpp::consistent_t>(t.cross_join(f));
    from_dynamic_check<sqlpp::consistent_t>(t.join(f).on(t.alpha > f.omega));

    // Try a bunch of non-tables
    from_dynamic_check<sqlpp::assert_from_table_t>(7);
    from_dynamic_check<sqlpp::assert_from_table_t>(t.alpha);
    from_dynamic_check<sqlpp::assert_from_table_t>(t.beta);
    from_dynamic_check<sqlpp::assert_from_table_t>(t.gamma);
    from_dynamic_check<sqlpp::assert_from_table_t>(t.delta);

    // Try cross joins (missing condition)
    from_dynamic_check<sqlpp::assert_from_not_pre_join_t>(t.join(f));
  }

  void dynamic_from()
  {
#warning : need to add tests for dynamic_from(xxx).add(yyy)
  }
}

int main(int, char* [])
{
  static_from();
  dynamic_from();
}

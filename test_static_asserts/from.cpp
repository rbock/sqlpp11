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

  template <typename Assert, typename Expression>
  void from_static_check(const Expression& expression)
  {
    using CheckResult = sqlpp::check_from_static_t<Expression>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(select(t.alpha).from(expression));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
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
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
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

  template <typename Assert, typename FromImpl, typename Expression>
  void dynamic_from_add_check(FromImpl from, const Expression& expression)
  {
    using CheckResult = sqlpp::check_from_add_t<FromImpl, Expression>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");

    using ReturnType = decltype(from.add(expression));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  void dynamic_from_add()
  {
    static auto db = MockDb{};
    auto fromT = dynamic_select(db, t.alpha).dynamic_from(t).from;
    auto staticFrom = dynamic_select(db, t.alpha).from(t).from;
    const auto fa = f.as(sqlpp::alias::a);

    // OK
    dynamic_from_add_check<sqlpp::consistent_t>(fromT, dynamic_join(f).on(t.alpha > f.omega));
    dynamic_from_add_check<sqlpp::consistent_t>(fromT, dynamic_inner_join(f).on(t.alpha > f.omega));
    dynamic_from_add_check<sqlpp::consistent_t>(fromT, dynamic_left_outer_join(f).on(t.alpha > f.omega));
    dynamic_from_add_check<sqlpp::consistent_t>(fromT, dynamic_right_outer_join(f).on(t.alpha > f.omega));
    dynamic_from_add_check<sqlpp::consistent_t>(fromT, dynamic_outer_join(f).on(t.alpha > f.omega));
    dynamic_from_add_check<sqlpp::consistent_t>(fromT, dynamic_cross_join(f));

    // Try a bunch of non dynamic joins
    dynamic_from_add_check<sqlpp::assert_from_add_dynamic>(staticFrom, 7);

    // Try a bunch of non dynamic joins
    dynamic_from_add_check<sqlpp::assert_from_add_dynamic_join>(fromT, 7);
    dynamic_from_add_check<sqlpp::assert_from_add_dynamic_join>(fromT, t.gamma);
    dynamic_from_add_check<sqlpp::assert_from_add_dynamic_join>(fromT, join(f, f.as(sqlpp::alias::a)));

    // Try incomplete dynamic join
    dynamic_from_add_check<sqlpp::assert_from_add_not_dynamic_pre_join>(fromT, dynamic_join(f));

    // Try joining the same table name
    dynamic_from_add_check<sqlpp::assert_from_add_unique_names>(fromT, dynamic_cross_join(t));
    dynamic_from_add_check<sqlpp::assert_from_add_unique_names>(fromT, dynamic_cross_join(f.as(t)));

    // Try joining with a condition that requires other tables
    dynamic_from_add_check<sqlpp::assert_from_add_no_required_tables>(fromT, dynamic_join(f).on(t.alpha > fa.omega));

    // If you really think you know what you are doing, use without_table_check
    dynamic_from_add_check<sqlpp::consistent_t>(fromT, dynamic_join(f).on(t.alpha > without_table_check(fa.omega)));
  }
}

int main(int, char* [])
{
  static_from();
  dynamic_from();
  dynamic_from_add();
}

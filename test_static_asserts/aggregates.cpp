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
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

namespace
{
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
  void static_run_check(const Expression&)
  {
    using Context = MockDb::_serializer_context_t;
    using CheckResult = sqlpp::run_check_t<Context, Expression>;
    using ExpectedCheckResult = std::is_same<CheckResult, Assert>;
    print_type_on_error<CheckResult>(ExpectedCheckResult{});
    static_assert(ExpectedCheckResult::value, "Unexpected check result");
  }

  SQLPP_ALIAS_PROVIDER(whatever)
  static constexpr auto t = test::TabBar{};

  // If there is no group_by, we can select whatever we want
  void no_group_by()
  {
    static_run_check<sqlpp::consistent_t>(select(all_of(t)).from(t).unconditionally());
    static_run_check<sqlpp::consistent_t>(select(t.alpha).from(t).unconditionally());
    static_run_check<sqlpp::consistent_t>(select(count(t.alpha)).from(t).unconditionally());
  }

  // If there is a dynamic group_by, we can still select whatever we want
  // because there is no way of knowing which expressions might have been added dynamically
  void dynamic_group_by()
  {
    static_run_check<sqlpp::consistent_t>(select(all_of(t)).from(t).unconditionally());
    static_run_check<sqlpp::consistent_t>(select(t.alpha).from(t).unconditionally());
    static_run_check<sqlpp::consistent_t>(select(count(t.alpha)).from(t).unconditionally());
  }

  // If there is a static group_by, selected columns must be made of group_by expressions, or aggregate expression (e.g.
  // count(t.id)) or values to be valid
  void static_group_by_ok()
  {
    static_run_check<sqlpp::consistent_t>(select(t.alpha).from(t).unconditionally().group_by(t.alpha));
    static_run_check<sqlpp::consistent_t>(
        select((t.alpha + 42).as(whatever)).from(t).unconditionally().group_by(t.alpha));
    static_run_check<sqlpp::consistent_t>(
        select((t.alpha + 42).as(whatever)).from(t).unconditionally().group_by(t.alpha, t.alpha + t.delta * 17));
    static_run_check<sqlpp::consistent_t>(select((t.alpha + t.delta * 17).as(whatever))
                                              .from(t)
                                              .unconditionally()
                                              .group_by(t.alpha, t.alpha + t.delta * 17));
    static_run_check<sqlpp::consistent_t>(
        select((t.beta + "fortytwo").as(whatever)).from(t).unconditionally().group_by(t.beta));

    static_run_check<sqlpp::consistent_t>(select(avg(t.alpha)).from(t).unconditionally().group_by(t.beta));
    static_run_check<sqlpp::consistent_t>(select(count(t.alpha)).from(t).unconditionally().group_by(t.beta));
    static_run_check<sqlpp::consistent_t>(select(max(t.alpha)).from(t).unconditionally().group_by(t.beta));
    static_run_check<sqlpp::consistent_t>(select(min(t.alpha)).from(t).unconditionally().group_by(t.beta));
    static_run_check<sqlpp::consistent_t>(select(sum(t.alpha)).from(t).unconditionally().group_by(t.beta));

    static_run_check<sqlpp::consistent_t>(
        select((t.alpha + count(t.delta)).as(whatever)).from(t).unconditionally().group_by(t.alpha));

    static_run_check<sqlpp::consistent_t>(
        select(sqlpp::value(1).as(whatever)).from(t).unconditionally().group_by(t.alpha));
    static_run_check<sqlpp::consistent_t>(
        select(sqlpp::value("whatever").as(whatever)).from(t).unconditionally().group_by(t.alpha));
  }

  // Failures with static group_by and selected non-aggregates or incorrect aggregates
  void static_group_by_nok()
  {
    static_run_check<sqlpp::assert_no_unknown_aggregates_t>(select(t.beta).from(t).unconditionally().group_by(t.alpha));
    static_run_check<sqlpp::assert_no_unknown_aggregates_t>(
        select((t.alpha + t.delta).as(whatever)).from(t).unconditionally().group_by(t.alpha));
    static_run_check<sqlpp::assert_no_unknown_aggregates_t>(
        select((t.alpha + t.delta).as(whatever)).from(t).unconditionally().group_by(t.alpha, t.alpha + t.delta * 17));
  }
}

int main(int, char* [])
{
  no_group_by();
  dynamic_group_by();
  static_group_by_ok();
  static_group_by_nok();
}

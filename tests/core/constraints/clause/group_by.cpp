/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/tests/core/constraints_helpers.h>
#include <sqlpp11/tests/core/tables.h>

namespace
{
  SQLPP_CREATE_NAME_TAG(something);

  template<typename... Expressions>
    concept can_call_group_by_with = requires(Expressions... expressions) {
#warning: Do this for the other can_call_with tests, too
      sqlpp::group_by(expressions...);
      sqlpp::statement_t<sqlpp::no_group_by_t>{}.group_by(expressions...);
    };
}  // namespace

int main()
{
  const auto maybe = true;
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // group_by(<non arguments>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(), "at least one column required in group_by()");

  // group_by(<arguments with no value>) cannot be called.
  static_assert(can_call_group_by_with<decltype(bar.boolNn)>, "OK, argument a column");
  static_assert(can_call_group_by_with<decltype(dynamic(maybe, bar.boolNn))>, "OK, argument a column");
  static_assert(can_call_group_by_with<decltype(sqlpp::declare_group_by_column(bar.id + 7))>, "OK, declared group by column");
  static_assert(can_call_group_by_with<decltype(7), decltype(bar.boolNn)>, "OK, but will fail later: 7 has a value, but is not a column");
  static_assert(not can_call_group_by_with<decltype(bar.id = 7), decltype(bar.boolNn)>, "not value: assignment");
  static_assert(not can_call_group_by_with<decltype(all_of(bar)), decltype(bar.boolNn)>, "not value: tuple");


  // group_by(<at least one non-group-by column>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(sqlpp::value(7)), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(bar.id, max(foo.id)), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");

  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(dynamic(maybe, sqlpp::value(7))), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(bar.id, dynamic(maybe, max(foo.id))), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(dynamic(maybe, bar.id), max(foo.id)), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");

  // group_by(<containing aggregate functions>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(declare_group_by_column(max(foo.id))), "arguments for group_by() must not contain aggregate functions");

  // group_by is not required
  {
    auto s = sqlpp::statement_t<sqlpp::no_group_by_t>{};
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::consistent_t>::value, "");
  }

  // group_by must not require unknown tables for prepare/run
  {
    auto s = select(foo.id).from(foo).where(true).group_by(foo.id);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::consistent_t>::value, "");
  }

  {
    auto s = select(foo.id).from(foo).where(true).group_by(foo.id, bar.id);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_no_unknown_tables_in_group_by_t>::value, "");
  }

  // `group_by` using unknown table
  {
    auto s = select(max(foo.id).as(something)).from(foo).where(true).group_by(bar.id);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_no_unknown_tables_in_group_by_t>::value, "");
  }

  // `group_by` statically using dynamic table
  {
    auto s = select(max(foo.id).as(something)).from(foo.cross_join(dynamic(maybe, bar))).where(true).group_by(bar.id);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_no_unknown_static_tables_in_group_by_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_no_unknown_static_tables_in_group_by_t>::value, "");
  }

}


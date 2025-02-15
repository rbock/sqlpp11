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
  template <typename... Expressions>
  concept can_call_order_by_with_standalone = requires(Expressions... expressions) {
    sqlpp::order_by(expressions...);
  };
  template <typename... Expressions>
  concept can_call_order_by_with_in_statement = requires(Expressions... expressions) {
    sqlpp::statement_t<sqlpp::no_order_by_t>{}.order_by(expressions...);
  };

  template <typename... Expressions>
  concept can_call_order_by_with =
      can_call_order_by_with_standalone<Expressions...> and can_call_order_by_with_in_statement<Expressions...>;

  template <typename... Expressions>
  concept cannot_call_order_by_with =
      not(can_call_order_by_with_standalone<Expressions...> or can_call_order_by_with_in_statement<Expressions...>);
}  // namespace

int main()
{
  const auto maybe = true;
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // order_by(<non arguments>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::order_by(), "at least one sort-order expression (e.g. column.asc()) required in order_by()");

  // order_by(<non-sort-order arguments>) cannot be called.
  static_assert(can_call_order_by_with<decltype(bar.boolNn.asc())>, "OK, argument a column ascending");
  static_assert(can_call_order_by_with<decltype(dynamic(maybe, bar.boolNn.desc()))>, "OK, argument a dynamic column");
  static_assert(can_call_order_by_with<decltype((bar.id + 7).asc())>, "OK, declared order by column");
  static_assert(cannot_call_order_by_with<decltype(bar.id)>, "not sort order: column");
  static_assert(cannot_call_order_by_with<decltype(7)>, "not sort order: integer");
  static_assert(cannot_call_order_by_with<decltype(bar.id = 7), decltype(bar.boolNn)>, "not sort order: assignment");
  static_assert(cannot_call_order_by_with<decltype(all_of(bar)), decltype(bar.boolNn)>, "not sort order: tuple");

  // order_by(<duplicate sort order expressions>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::order_by(bar.id.asc(), bar.id.asc()), "at least one duplicate argument detected in order_by()");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::order_by(dynamic(maybe, bar.id.asc()), bar.id.asc()), "at least one duplicate argument detected in order_by()");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::order_by(bar.id.asc(), dynamic(maybe, bar.id.asc())), "at least one duplicate argument detected in order_by()");

  // order_by is not required
  {
    auto s = sqlpp::statement_t<sqlpp::no_order_by_t>{};
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::consistent_t>::value, "");
  }

  // order_by must not require unknown tables for prepare/run
  {
    auto s = select(foo.id).from(foo).where(true).order_by(foo.id.asc());
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::consistent_t>::value, "");
  }

  {
    auto s = select(foo.id).from(foo).where(true).order_by(foo.id.asc(), bar.id.asc());
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_no_unknown_tables_in_order_by_t>::value, "");
  }

  // order_by must not require unknown tables for prepare/run
  {
    // OK, foo.id and max(...) are both aggregates
    auto s = select(foo.id).from(foo).where(true).group_by(foo.id).order_by(foo.id.asc(), max(foo.intN).desc());
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::consistent_t>::value, "");
  }

  {
    // Fail: foo.id is a non-aggregate, but max(...) is an aggregate
    auto s = select(foo.id).from(foo).where(true).order_by(foo.id.asc(), max(foo.intN).desc());
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_correct_order_by_aggregates_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_correct_order_by_aggregates_t>::value, "");
  }

  {
    // Fail: foo.id is an aggregate, but foo.intN is not.
    auto s = select(foo.id).from(foo).where(true).group_by(foo.id).order_by(foo.id.asc(), foo.intN.desc());
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_correct_order_by_aggregates_with_group_by_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_correct_order_by_aggregates_with_group_by_t>::value, "");
  }

  {
    // Fail: foo.intN is a dynamic aggregate, but foo.intN is statically used in order_by.
    auto s = select(foo.id).from(foo).where(true).group_by(foo.id, dynamic(maybe, foo.intN)).order_by(foo.id.asc(), foo.intN.desc());
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_correct_static_order_by_aggregates_with_group_by_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_correct_static_order_by_aggregates_with_group_by_t>::value, "");
  }

  // `order_by` using unknown table
  {
    auto s = select(foo.id).from(foo).where(true).order_by(bar.id.desc());
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_no_unknown_tables_in_order_by_t>::value, "");
  }

  // `order_by` statically using dynamic table
  {
    auto s = select(foo.id).from(foo.cross_join(dynamic(maybe, bar))).where(true).order_by(bar.id.desc());
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_no_unknown_static_tables_in_order_by_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_no_unknown_static_tables_in_order_by_t>::value, "");
  }

}


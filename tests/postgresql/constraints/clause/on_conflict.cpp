/*
 * Copyright (c) 2025, Roland Bock
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
#include <sqlpp11/tests/postgresql/tables.h>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/postgresql.h>

namespace
{
  SQLPP_CREATE_NAME_TAG(something);

  // Test on_conflict
  template <typename... Expressions>
  concept can_call_on_conflict_with_standalone = requires(Expressions... expressions) {
    sqlpp::postgresql::on_conflict(expressions...);
  };
  template <typename... Expressions>
  concept can_call_on_conflict_with_in_statement = requires(Expressions... expressions) {
    sqlpp::statement_t<sqlpp::postgresql::no_on_conflict_t>{}.on_conflict(expressions...);
  };

  template <typename... Expressions>
  concept can_call_on_conflict_with =
      can_call_on_conflict_with_standalone<Expressions...> and can_call_on_conflict_with_in_statement<Expressions...>;

  template <typename... Expressions>
  concept cannot_call_on_conflict_with =
      not(can_call_on_conflict_with_standalone<Expressions...> or can_call_on_conflict_with_in_statement<Expressions...>);

  // Test do_update
  template <typename Lhs, typename... Expressions>
  concept can_call_do_update_with = requires(Lhs lhs, Expressions... expressions) {
    lhs.do_update(expressions...);
  };

  template <typename Lhs, typename... Expressions>
  concept cannot_call_do_update_with =
      not(can_call_do_update_with<Lhs, Expressions...>);

  // Test where
  template <typename Lhs, typename... Expressions>
  concept can_call_where_with = requires(Lhs lhs, Expressions... expressions) {
    lhs.where(expressions...);
  };

  template <typename Lhs, typename... Expressions>
  concept cannot_call_where_with =
      not(can_call_where_with<Lhs, Expressions...>);

}  // namespace

int main()
{
  const auto maybe = true;
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  using sqlpp::postgresql::on_conflict;

  // OK
  on_conflict();
  on_conflict(foo.id);
  on_conflict(foo.id, foo.textNnD);
  on_conflict(foo.id, bar.id);
    static_assert(can_call_on_conflict_with<>, "");
    static_assert(can_call_on_conflict_with<decltype(foo.id)>, "");

  // -------------------------
  // on_conflict(<non-column>) cannot be constructed.
  // -------------------------
  {
    static_assert(cannot_call_on_conflict_with<decltype(all_of(foo))>, "");
    static_assert(cannot_call_on_conflict_with<decltype(bar.id.as(something))>, "");
  }

  // do_update requires assignments as arguments
  {
    auto insert = sqlpp::postgresql::insert_into(foo).default_values().on_conflict(foo.id);
    static_assert(can_call_do_update_with<decltype(insert), decltype(foo.id = 7)>, "");
    static_assert(can_call_do_update_with<decltype(insert), decltype(dynamic(maybe, foo.id = 7))>, "");
    static_assert(cannot_call_do_update_with<decltype(insert), decltype(foo.id == 7)>, "");

    using I = decltype(insert);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>, sqlpp::postgresql::assert_on_conflict_action_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<I>, sqlpp::postgresql::assert_on_conflict_action_t>::value, "");
  }

  // do_update in-function checks
  {
    const auto insert = sqlpp::postgresql::insert_into(foo).default_values();
    SQLPP_CHECK_STATIC_ASSERT(insert.on_conflict().do_update(foo.intN = 5),
                              "conflict_target specification is required with do_update()");
    SQLPP_CHECK_STATIC_ASSERT(insert.on_conflict(foo.id).do_update(),
                              "at least one assignment expression required in do_update()");
    SQLPP_CHECK_STATIC_ASSERT(insert.on_conflict(foo.id).do_update(foo.intN = 5, foo.intN = 19),
                              "at least one duplicate column detected in do_update()");
    SQLPP_CHECK_STATIC_ASSERT(insert.on_conflict(foo.id).do_update(foo.id = 7, bar.id = 5),
                              "do_update() contains assignments for columns from more than one table");
  }

  // do_update requires where to be called with a single boolean expression
  {
    auto insert = sqlpp::postgresql::insert_into(foo).default_values().on_conflict(foo.id).do_update(foo.id = 7);

    static_assert(can_call_where_with<decltype(insert), decltype(foo.id == 7)>, "");
    static_assert(can_call_where_with<decltype(insert), decltype(dynamic(maybe, foo.id == 7))>, "");
    static_assert(cannot_call_where_with<decltype(insert), decltype(foo.id = 7)>, "");
    static_assert(cannot_call_where_with<decltype(insert), decltype(foo.id = 7), decltype(true)>, "");

    using I = decltype(insert);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>, sqlpp::postgresql::assert_on_conflict_update_where_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<I>, sqlpp::postgresql::assert_on_conflict_update_where_t>::value, "");
  }

  // do_update.where in-function checks
  {
    auto insert = sqlpp::postgresql::insert_into(foo).default_values().on_conflict(foo.id).do_update(foo.id = 7);
    SQLPP_CHECK_STATIC_ASSERT(insert.where(max(foo.id) > 3), "where() must not contain aggregate functions");
  }

  // -----------------------------------------
  // bad table checks
  // -----------------------------------------
  {
    auto insert = sqlpp::postgresql::insert_into(foo).default_values().on_conflict(bar.id).do_update(foo.id = 7).where(true);
    using I = decltype(insert);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<I>, sqlpp::postgresql::assert_no_unknown_tables_in_on_conflict_do_update_t>::value, "");
  }

  {
    auto insert = sqlpp::postgresql::insert_into(foo).default_values().on_conflict(foo.id).do_update(bar.id = 7).where(true);
    using I = decltype(insert);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<I>, sqlpp::postgresql::assert_no_unknown_tables_in_on_conflict_do_update_t>::value, "");
  }

  {
    auto insert = sqlpp::postgresql::insert_into(foo).default_values().on_conflict(foo.id).do_update(foo.id = 7).where(bar.id > 8);
    using I = decltype(insert);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<I>, sqlpp::postgresql::assert_no_unknown_tables_in_on_conflict_do_update_t>::value, "");
  }

  // Dynamically provided tables are not a thing in `insert_into`. Constructing a nonsense custom query.
  //
  // Don't do this home!
  {
    auto nonsense = from(dynamic(maybe, foo)) << on_conflict(foo.id).do_update(foo.id = 7).where(foo.id > 8);
    using I = decltype(nonsense);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<I>, sqlpp::postgresql::assert_no_unknown_static_tables_in_on_conflict_do_update_t>::value, "");
  }
}


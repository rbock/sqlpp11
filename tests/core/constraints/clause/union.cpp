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
#include <sqlpp11/tests/core/tables.h>

namespace
{
  SQLPP_CREATE_NAME_TAG(something);

  template <typename Lhs, typename Rhs>
  concept can_call_union_all_with_standalone = requires(Lhs lhs, Rhs rhs) {
    sqlpp::union_all(lhs, rhs);
  };
  template <typename Lhs, typename Rhs>
  concept can_call_union_all_with_in_statement = requires(Lhs lhs, Rhs rhs) {
    lhs.union_all(rhs);
  };

  template <typename Lhs, typename Rhs>
  concept can_call_union_all_with =
      can_call_union_all_with_standalone<Lhs, Rhs> and can_call_union_all_with_in_statement<Lhs, Rhs>;

  template <typename Lhs, typename Rhs>
  concept cannot_call_union_all_with =
      not(can_call_union_all_with_standalone<Lhs, Rhs> or can_call_union_all_with_in_statement<Lhs, Rhs>);

  template <typename Lhs, typename Rhs>
  concept can_call_union_distinct_with_standalone = requires(Lhs lhs, Rhs rhs) {
    sqlpp::union_distinct(lhs, rhs);
  };
  template <typename Lhs, typename Rhs>
  concept can_call_union_distinct_with_in_statement = requires(Lhs lhs, Rhs rhs) {
    lhs.union_distinct(rhs);
  };

  template <typename Lhs, typename Rhs>
  concept can_call_union_distinct_with =
      can_call_union_distinct_with_standalone<Lhs, Rhs> and can_call_union_distinct_with_in_statement<Lhs, Rhs>;

  template <typename Lhs, typename Rhs>
  concept cannot_call_union_distinct_with =
      not(can_call_union_distinct_with_standalone<Lhs, Rhs> or can_call_union_distinct_with_in_statement<Lhs, Rhs>);

#define CAN_CALL_ALL_UNIONS_WITH(LHS, RHS) \
  static_assert(can_call_union_all_with<decltype(LHS), decltype(RHS)>, "");\
  static_assert(can_call_union_distinct_with<decltype(LHS), decltype(RHS)>, "");

#define CANNOT_CALL_ANY_UNION_WITH(LHS, RHS) \
  static_assert(cannot_call_union_all_with<decltype(LHS), decltype(RHS)>, "");\
  static_assert(cannot_call_union_distinct_with<decltype(LHS), decltype(RHS)>, "");

#define CHECK_UNION_STATIC_ASSERTS(LHS, RHS, MESSAGE)     \
SQLPP_CHECK_STATIC_ASSERT(union_all(LHS, RHS), MESSAGE); \
SQLPP_CHECK_STATIC_ASSERT(union_distinct(LHS, RHS), MESSAGE);

}  // namespace

int main()
{
  const auto maybe = true;
  const auto bar = test::TabBar{};
  const auto foo = test::TabFoo{};

  const auto incomplete_lhs = select(all_of(bar)).from(bar);
  const auto lhs = incomplete_lhs.where(true);
  const auto incomplete_rhs = select(all_of(bar.as(something))).from(bar.as(something));
  const auto rhs = incomplete_rhs.where(true);

  union_distinct(lhs, rhs);
  static_assert(can_call_union_all_with_in_statement<decltype(lhs), decltype(rhs)>, "");\
  // OK
  CAN_CALL_ALL_UNIONS_WITH(lhs, rhs);
  CAN_CALL_ALL_UNIONS_WITH(lhs, dynamic(maybe, rhs));

  // Cannot union with non-statement
  CANNOT_CALL_ANY_UNION_WITH(lhs, bar);
  CANNOT_CALL_ANY_UNION_WITH(lhs, bar.id);
  CANNOT_CALL_ANY_UNION_WITH(lhs, all_of(bar));
  CANNOT_CALL_ANY_UNION_WITH(bar, rhs);
  CANNOT_CALL_ANY_UNION_WITH(bar.id, rhs);
  CANNOT_CALL_ANY_UNION_WITH(all_of(bar), rhs);

  // UNION requires statements with result row
  {
    const auto bad_custom_lhs = sqlpp::statement_t<sqlpp::no_union_t>{};
    const auto bad_custom_rhs = sqlpp::statement_t<>{};
    CHECK_UNION_STATIC_ASSERTS(bad_custom_lhs, rhs, "left hand side argument of a union has to be a select statement or union");
    CHECK_UNION_STATIC_ASSERTS(lhs, bad_custom_rhs, "right hand side argument of a union has to be a select statement or union");
  }

  // UNION requires preparable statements
  CHECK_UNION_STATIC_ASSERTS(incomplete_lhs, incomplete_rhs, "calling where() required");
  CHECK_UNION_STATIC_ASSERTS(lhs, incomplete_rhs, "calling where() required");
  CHECK_UNION_STATIC_ASSERTS(incomplete_lhs, rhs, "calling where() required");

  // UNION requires statements with same result row
  {
    auto s_foo_int  = select(foo.textNnD, foo.id).from(foo).where(true);
    auto s_foo_int_n  = select(foo.textNnD, foo.intN).from(foo).where(true);
    auto s_value_id  = select(foo.textNnD, sqlpp::value(7).as(foo.id)).from(foo).where(true);
    auto s_value_oid  = select(foo.textNnD, sqlpp::value(7).as(something)).from(foo).where(true);
    // Different value type
    static_assert(not std::is_same<sqlpp::value_type_of_t<decltype(foo.id)>,
                                   sqlpp::value_type_of_t<decltype(foo.intN)>>::value,
                  "");
    CHECK_UNION_STATIC_ASSERTS(s_foo_int, s_foo_int_n, "both arguments in a union have to have the same result columns (type and name)");
    // Different name
    CHECK_UNION_STATIC_ASSERTS(s_value_id, s_value_oid, "both arguments in a union have to have the same result columns (type and name)");
    CHECK_UNION_STATIC_ASSERTS(s_foo_int, dynamic(maybe, s_foo_int_n), "both arguments in a union have to have the same result columns (type and name)");
    // Different name
    CHECK_UNION_STATIC_ASSERTS(s_value_id, dynamic(maybe, s_value_oid), "both arguments in a union have to have the same result columns (type and name)");
  }
}


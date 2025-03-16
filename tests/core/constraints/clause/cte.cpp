/*
 * Copyright (c) 2025, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp23/tests/core/constraints_helpers.h>

#include <sqlpp23/tests/core/tables.h>

namespace {
SQLPP_CREATE_NAME_TAG(something);

template <typename Lhs, typename Rhs>
concept can_call_cte_as_with = requires(Lhs lhs, Rhs rhs) { lhs.as(rhs); };

// Returns true if `declval<Lhs>().UNION(declval<Rhs>)` is a valid function
// call.
#define MAKE_CAN_CALL_CTE_UNION_WITH(UNION) \
  template <typename Lhs, typename Rhs>     \
  concept can_call_cte_##UNION##_with =     \
      requires(Lhs lhs, Rhs rhs) { lhs.UNION(rhs); };

MAKE_CAN_CALL_CTE_UNION_WITH(union_all);
MAKE_CAN_CALL_CTE_UNION_WITH(union_distinct);

#define CAN_CALL_ALL_CTE_UNIONS_WITH(LHS, RHS)                             \
  static_assert(can_call_cte_union_all_with<decltype(LHS), decltype(RHS)>, \
                "");                                                       \
  static_assert(                                                           \
      can_call_cte_union_distinct_with<decltype(LHS), decltype(RHS)>, "");

#define CANNOT_CALL_ANY_UNION_WITH(LHS, RHS)                                   \
  static_assert(not can_call_cte_union_all_with<decltype(LHS), decltype(RHS)>, \
                "");                                                           \
  static_assert(                                                               \
      not can_call_cte_union_distinct_with<decltype(LHS), decltype(RHS)>, "");

#define CHECK_CTE_UNION_STATIC_ASSERTS(LHS, RHS, MESSAGE) \
  SQLPP_CHECK_STATIC_ASSERT(LHS.union_all(RHS), MESSAGE); \
  SQLPP_CHECK_STATIC_ASSERT(LHS.union_distinct(RHS), MESSAGE);

}  // namespace

int main() {
  const auto maybe = true;
  const auto bar = test::TabBar{};
  const auto foo = test::TabFoo{};

  const auto ref = sqlpp::cte(something);
  const auto incomplete_s1 = select(all_of(bar)).from(bar);
  const auto s1 = incomplete_s1.where(true);
  const auto incomplete_s2 =
      select(all_of(bar.as(something))).from(bar.as(something));
  const auto s2 = incomplete_s2.where(true);

  const auto cte = sqlpp::cte(something).as(s1);

  // OK
  static_assert(can_call_cte_as_with<decltype(ref), decltype(s1)>, "");
  static_assert(can_call_cte_as_with<decltype(ref), decltype(s2)>, "");

  // No statement
  static_assert(not can_call_cte_as_with<decltype(ref), decltype(foo)>, "");
  static_assert(not can_call_cte_as_with<decltype(ref), decltype(foo.id)>, "");
  static_assert(not can_call_cte_as_with<decltype(ref), decltype(all_of(foo))>,
                "");

  // No statement
  static_assert(
      not can_call_cte_as_with<decltype(ref), decltype(insert_into(foo))>, "");
  static_assert(
      not can_call_cte_as_with<decltype(ref), decltype(sqlpp::statement_t<>{})>,
      "");

  // Missing where condition
  SQLPP_CHECK_STATIC_ASSERT(ref.as(incomplete_s1), "calling where() required");
  SQLPP_CHECK_STATIC_ASSERT(ref.as(incomplete_s2), "calling where() required");

  // Missing tables
  SQLPP_CHECK_STATIC_ASSERT(
      ref.as(select(foo.id).from(bar).where(true)),
      "common table expression must not use unknown tables");

  // Bad self-reference
  SQLPP_CHECK_STATIC_ASSERT(
      ref.as(select(cte.id).from(cte).where(true)),
      "common table expression must not self-reference in the first part, use "
      "union_all/union_distinct for recursion");

  // OK
  CAN_CALL_ALL_CTE_UNIONS_WITH(cte, s1);
  CAN_CALL_ALL_CTE_UNIONS_WITH(cte, s2);

  // Cannot union with non-statement
  CANNOT_CALL_ANY_UNION_WITH(cte, bar);
  CANNOT_CALL_ANY_UNION_WITH(cte, bar.id);
  CANNOT_CALL_ANY_UNION_WITH(cte, all_of(bar));
  CANNOT_CALL_ANY_UNION_WITH(cte, cte);

  // CTE UNION requires statements with result row
  {
    const auto bad_rhs = sqlpp::statement_t<>{};
    CHECK_CTE_UNION_STATIC_ASSERTS(
        cte, bad_rhs,
        "argument of a union has to be a select statement or a union");
  }

  // CTE UNION requires consistent statements
  {
    auto bad_rhs = select(all_of(foo)).from(foo);
    CHECK_CTE_UNION_STATIC_ASSERTS(cte, bad_rhs, "calling where() required");
  }

  // CTE UNION requires no missing tables
  {
    auto bad_rhs = select(all_of(foo));
    CHECK_CTE_UNION_STATIC_ASSERTS(
        cte, bad_rhs, "right hand side of cte union is is missing tables");
  }

  // CTE UNION requires statements with same result row
  {
    auto c_foo_int = sqlpp::cte(something).as(
        select(foo.textNnD, foo.id).from(foo).where(true));
    auto s_foo_int_n = select(foo.textNnD, foo.intN).from(foo).where(true);
    auto c_value_id = sqlpp::cte(something).as(
        select(foo.textNnD, sqlpp::value(7).as(foo.id)).from(foo).where(true));
    auto s_value_oid = select(foo.textNnD, sqlpp::value(7).as(something))
                           .from(foo)
                           .where(true);
    // Different value type
    static_assert(
        not std::is_same<sqlpp::value_type_of_t<decltype(foo.id)>,
                         sqlpp::value_type_of_t<decltype(foo.intN)>>::value,
        "");
    CHECK_CTE_UNION_STATIC_ASSERTS(
        c_foo_int, s_foo_int_n,
        "both select statements in a union have to have the same result "
        "columns (type and name)");
    // Different name
    CHECK_CTE_UNION_STATIC_ASSERTS(
        c_value_id, s_value_oid,
        "both select statements in a union have to have the same result "
        "columns (type and name)");
    CHECK_CTE_UNION_STATIC_ASSERTS(
        c_foo_int, dynamic(maybe, s_foo_int_n),
        "both select statements in a union have to have the same result "
        "columns (type and name)");
    // Different name
    CHECK_CTE_UNION_STATIC_ASSERTS(
        c_value_id, dynamic(maybe, s_value_oid),
        "both select statements in a union have to have the same result "
        "columns (type and name)");
  }
}

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

  // Returns true if `JOIN(declval<Lhs>(), JOIN(declval<Rhs>)` is a valid function call.
#define MAKE_CAN_CALL_JOIN_WITH(JOIN) \
  template <typename Lhs, typename Rhs, typename = void>\
  struct can_call_##JOIN##_with : public std::false_type\
  {\
  };\
\
  template <typename Lhs, typename Rhs>\
  struct can_call_##JOIN##_with<Lhs, Rhs, std::void_t<decltype(sqlpp::JOIN(std::declval<Lhs>(), std::declval<Rhs>()))>>\
      : public std::true_type\
  {\
  };

MAKE_CAN_CALL_JOIN_WITH(join);
MAKE_CAN_CALL_JOIN_WITH(inner_join);
MAKE_CAN_CALL_JOIN_WITH(left_outer_join);
MAKE_CAN_CALL_JOIN_WITH(right_outer_join);
MAKE_CAN_CALL_JOIN_WITH(full_outer_join);
MAKE_CAN_CALL_JOIN_WITH(cross_join);

#define CAN_CALL_ALL_JOINS_WITH(LHS, RHS) \
  static_assert(can_call_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(can_call_inner_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(can_call_left_outer_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(can_call_right_outer_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(can_call_full_outer_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(can_call_cross_join_with<decltype(LHS), decltype(RHS)>::value, "");

#define CANNOT_CALL_ANY_JOIN_WITH(LHS, RHS) \
  static_assert(not can_call_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(not can_call_inner_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(not can_call_left_outer_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(not can_call_right_outer_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(not can_call_full_outer_join_with<decltype(LHS), decltype(RHS)>::value, "");\
  static_assert(not can_call_cross_join_with<decltype(LHS), decltype(RHS)>::value, "");

#define CHECK_JOIN_STATIC_ASSERTS(LHS, RHS, MESSAGE)            \
SQLPP_CHECK_STATIC_ASSERT(join(LHS, RHS), MESSAGE);             \
SQLPP_CHECK_STATIC_ASSERT(inner_join(LHS, RHS), MESSAGE);       \
SQLPP_CHECK_STATIC_ASSERT(left_outer_join(LHS, RHS), MESSAGE);  \
SQLPP_CHECK_STATIC_ASSERT(right_outer_join(LHS, RHS), MESSAGE); \
SQLPP_CHECK_STATIC_ASSERT(full_outer_join(LHS, RHS), MESSAGE);  \
SQLPP_CHECK_STATIC_ASSERT(cross_join(LHS, RHS), MESSAGE);

struct weird_table : public sqlpp::enable_join<weird_table>
{
};

inline auto make_table_ref(weird_table) -> weird_table { return {}; };
}  // namespace

namespace sqlpp {
  template<>
    struct is_table<weird_table>: public std::true_type {};
  template<>
    struct required_tables_of<weird_table>
    {
      using type = detail::type_vector<test::TabBar>;
    };
}

int main()
{
  const auto maybe = true;
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};
  const auto aFoo = foo.as(sqlpp::alias::a);
  const auto bFoo = foo.as(sqlpp::alias::b);

  // OK
  CAN_CALL_ALL_JOINS_WITH(bar, foo);
  CAN_CALL_ALL_JOINS_WITH(bar, foo.as(something));

  // Cannot join with a non-table
  CANNOT_CALL_ANY_JOIN_WITH(bar, foo.id);


  // JOIN can be called with two identical tables, but will fail in static assert.
  CHECK_JOIN_STATIC_ASSERTS(foo, foo, "duplicate table names detected in join");

  // JOIN must not be called with tables that depend on other tables.
  // Not sure this can happen in the wild, which is why we are using the `weird_table` to simulate the situation.
  CHECK_JOIN_STATIC_ASSERTS(weird_table{}, foo, "table dependencies detected in left side of join");
  CHECK_JOIN_STATIC_ASSERTS(foo, weird_table{}, "table dependencies detected in right side of join");

  // JOIN ... ON can be called with any boolean expression, but will fail with static assert if it uses the wrong
  // tables. Here, bFoo is not provided by the join.
  SQLPP_CHECK_STATIC_ASSERT(foo.join(bar).on(bFoo.id == bar.id),
                            "on() condition of a join must only use tables provided in that join");
  SQLPP_CHECK_STATIC_ASSERT(foo.join(bar).on(foo.id == bar.id).join(aFoo).on(bFoo.id == aFoo.id),
                            "on() condition of a join must only use tables provided in that join");

  // Here, bar is not provided /dynamically/ by the first join, but required /statically/ by the second join.
  SQLPP_CHECK_STATIC_ASSERT(
      foo.join(dynamic(maybe, bar)).on(foo.id == bar.id).join(aFoo).on(bar.id == aFoo.id),
      "on() condition of a static join must not use tables provided only dynamically in that join");
}


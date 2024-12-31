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
#include <sqlpp11/core/compat/type_traits.h>

namespace
{
  SQLPP_CREATE_NAME_TAG(something);

  // Returns true if `where(declval<Expression>())` is a valid function call.
  template <typename Expression, typename = void>
  struct can_call_where_with : public std::false_type
  {
  };

  template <typename Expression>
  struct can_call_where_with<Expression, sqlpp::void_t<decltype(sqlpp::where(std::declval<Expression>()))>>
      : public std::true_type
  {
  };
}  // namespace

int main()
{
  const auto maybe = true;
  const auto bar = test::TabBar{};

  // OK
  where(bar.boolNn);
  where(bar.boolNn == true);
  static_assert(can_call_where_with<decltype(bar.boolNn)>::value, "");

  // Try assignment as condition
  static_assert(not can_call_where_with<decltype(bar.boolNn = true)>::value, "");

  // Try non-boolean expression
  static_assert(not can_call_where_with<decltype(bar.id)>::value, "");

  // Try some other types as expressions
  static_assert(not can_call_where_with<decltype("true")>::value, "");
  static_assert(not can_call_where_with<decltype(17)>::value, "");
  static_assert(not can_call_where_with<decltype('c')>::value, "");
  static_assert(not can_call_where_with<decltype(nullptr)>::value, "");

  // Try alias bool column (can only be used as select column, but not as a value in `where`).
  static_assert(not can_call_where_with<decltype(bar.boolNn.as(something))>::value, "");

  // Try using aggregate functions in where
  SQLPP_CHECK_STATIC_ASSERT(where(count(bar.id) > 0), "where() must not contain aggregate functions");
  SQLPP_CHECK_STATIC_ASSERT(where(bar.boolNn and count(bar.id) > 0), "where() must not contain aggregate functions");
  SQLPP_CHECK_STATIC_ASSERT(where(bar.boolNn and dynamic(maybe, (count(bar.id) > 0))),
                            "where() must not contain aggregate functions");
  SQLPP_CHECK_STATIC_ASSERT(where(case_when(count(bar.id) > 0).then(bar.boolNn).else_(not bar.boolNn)),
                            "where() must not contain aggregate functions");

  // `where` isn't required if neither tables nor CTEs are required.
  {
    auto s = select(sqlpp::value(7).as(something));
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }

  // Try omitting required where/unconditionally
  {
    auto s = select(all_of(bar)).from(bar);
    using S = decltype(s);
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_where_or_unconditionally_called_t>::value,
        "");
  }
  {
    auto c = cte(something).as(select(sqlpp::value(7).as(something)));
    auto s = with(c)(select(all_of(c)).from(c));
    using S = decltype(s);
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_where_or_unconditionally_called_t>::value,
        "");
  }
  {
    auto s = delete_from(bar);
    using S = decltype(s);
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_where_or_unconditionally_called_t>::value,
        "");
  }
  {
    auto s = update(bar).set(bar.id = 7);
    using S = decltype(s);
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_where_or_unconditionally_called_t>::value,
        "");
  }
}


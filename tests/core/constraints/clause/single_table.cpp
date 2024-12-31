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

  // Returns true if `single_table(declval<Expression>())` is a valid function call.
  template <typename Expression, typename = void>
  struct can_call_single_table_with : public std::false_type
  {
  };

  template <typename Expression>
  struct can_call_single_table_with<Expression, sqlpp::void_t<decltype(sqlpp::single_table(std::declval<Expression>()))>>
      : public std::true_type
  {
  };
}  // namespace

int main()
{
  const auto maybe = true;
  const auto bar = test::TabBar{};
  const auto foo = test::TabFoo{};
  const auto c = cte(something).as(select(bar.id).from(bar).where(true));

  // OK
  static_assert(can_call_single_table_with<decltype(bar)>::value, "");

  // Try dyanamic table
  static_assert(not can_call_single_table_with<decltype(dynamic(maybe, bar))>::value, "");

  // Try assignment as table
  static_assert(not can_call_single_table_with<decltype(bar.boolNn = true)>::value, "");

  // Try a column
  static_assert(not can_call_single_table_with<decltype(bar.id)>::value, "");

  // Try some other types as tables
  static_assert(not can_call_single_table_with<decltype("true")>::value, "");
  static_assert(not can_call_single_table_with<decltype(17)>::value, "");
  static_assert(not can_call_single_table_with<decltype('c')>::value, "");
  static_assert(not can_call_single_table_with<decltype(nullptr)>::value, "");

  // Can call with cte or table alias (will fail in static assert, though).
  static_assert(can_call_single_table_with<decltype(bar.as(something))>::value, "");
  static_assert(can_call_single_table_with<decltype(c)>::value, "");

  // Try using aggregate functions in single_table
  SQLPP_CHECK_STATIC_ASSERT(single_table(bar.as(something)), "single_table() argument must be a raw table, i.e. no join or cte");
  SQLPP_CHECK_STATIC_ASSERT(single_table(c), "single_table() argument must be a raw table, i.e. no join or cte");
  SQLPP_CHECK_STATIC_ASSERT(single_table(foo.cross_join(bar)),
                            "single_table() argument must be a raw table, i.e. no join or cte");

  // `single_table` is required
  {
    auto s = sqlpp::statement_t<sqlpp::no_single_table_t>{};
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_single_table_provided_t>::value, "");
  }

}


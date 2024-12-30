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

  // Returns true if `group_by(declval<Expressions>()...)` is a valid function call.
  template <typename TypeVector, typename = void>
  struct can_call_group_by_with_impl : public std::false_type
  {
  };

  template <typename... Expressions>
  struct can_call_group_by_with_impl<sqlpp::detail::type_vector<Expressions...>,
                                  sqlpp::void_t<decltype(sqlpp::group_by(std::declval<Expressions>()...))>>
      : public std::true_type
  {
  };

  template <typename... Expressions>
  struct can_call_group_by_with : public can_call_group_by_with_impl<sqlpp::detail::type_vector<Expressions...>>
  {
  };

}  // namespace

namespace test {
  SQLPP_CREATE_NAME_TAG(max_id);
}

int main()
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // group_by(<non arguments>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(), "at least one column required in group_by()");

  // group_by(<arguments with no value>) cannot be called.
  static_assert(can_call_group_by_with<decltype(bar.boolNn)>::value, "OK, argument a column");
  static_assert(can_call_group_by_with<decltype(dynamic(true, bar.boolNn))>::value, "OK, argument a column");
  static_assert(can_call_group_by_with<decltype(sqlpp::declare_group_by_column(bar.id + 7))>::value, "OK, declared group by column");
  static_assert(can_call_group_by_with<decltype(7), decltype(bar.boolNn)>::value, "OK, but will fail later: 7 has a value, but is not a column");
  static_assert(not can_call_group_by_with<decltype(bar.id = 7), decltype(bar.boolNn)>::value, "not value: assignment");
  static_assert(not can_call_group_by_with<decltype(all_of(bar)), decltype(bar.boolNn)>::value, "not value: tuple");


  // group_by(<at least one non-group-by column>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(sqlpp::value(7)), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(bar.id, max(foo.id)), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");

  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(dynamic(true, sqlpp::value(7))), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(bar.id, dynamic(true, max(foo.id))), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::group_by(dynamic(true, bar.id), max(foo.id)), "all arguments for group_by() must be columns or expressions wrapped in declare_group_by_column()");

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

}


/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/tests/core/constraints_helpers.h>

#include <sqlpp11/tests/core/tables.h>

namespace {
SQLPP_CREATE_NAME_TAG(something);

template <typename... Expressions>
concept can_call_where_with_standalone =
    requires(Expressions... expressions) { sqlpp::where(expressions...); };
template <typename... Expressions>
concept can_call_where_with_in_statement =
    requires(Expressions... expressions) {
      sqlpp::statement_t<sqlpp::no_where_t>{}.where(expressions...);
    };

template <typename... Expressions>
concept can_call_where_with = can_call_where_with_standalone<Expressions...> and
                              can_call_where_with_in_statement<Expressions...>;

template <typename... Expressions>
concept cannot_call_where_with =
    not(can_call_where_with_standalone<Expressions...> or
        can_call_where_with_in_statement<Expressions...>);
} // namespace

int main() {
  const auto maybe = true;
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // OK
  where(bar.boolNn);
  where(bar.boolNn == true);
  static_assert(can_call_where_with<decltype(bar.boolNn)>, "");

  // Try assignment as condition
  static_assert(cannot_call_where_with<decltype(bar.boolNn = true)>, "");

  // Try non-boolean expression
  static_assert(cannot_call_where_with<decltype(bar.id)>, "");

  // Try some other types as expressions
  static_assert(cannot_call_where_with<decltype("true")>, "");
  static_assert(cannot_call_where_with<decltype(17)>, "");
  static_assert(cannot_call_where_with<decltype('c')>, "");
  static_assert(cannot_call_where_with<decltype(nullptr)>, "");

  // Try alias bool column (can only be used as select column, but not as a
  // value in `where`).
  static_assert(cannot_call_where_with<decltype(bar.boolNn.as(something))>, "");

  // Try using aggregate functions in where
  SQLPP_CHECK_STATIC_ASSERT(where(count(bar.id) > 0),
                            "where() must not contain aggregate functions");
  SQLPP_CHECK_STATIC_ASSERT(where(bar.boolNn and count(bar.id) > 0),
                            "where() must not contain aggregate functions");
  SQLPP_CHECK_STATIC_ASSERT(
      where(bar.boolNn and dynamic(maybe, (count(bar.id) > 0))),
      "where() must not contain aggregate functions");
  SQLPP_CHECK_STATIC_ASSERT(
      where(
          case_when(count(bar.id) > 0).then(bar.boolNn).else_(not bar.boolNn)),
      "where() must not contain aggregate functions");

  // `where` isn't required if neither tables nor CTEs are required.
  {
    auto s = select(sqlpp::value(7).as(something));
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::consistent_t>::value,
                  "");
  }

  // Try omitting required where/unconditionally
  {
    auto s = select(all_of(bar)).from(bar);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::assert_where_called_t>::value,
                  "");
  }
  {
    auto c = cte(something).as(select(sqlpp::value(7).as(something)));
    auto s = with(c)(select(all_of(c)).from(c));
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::assert_where_called_t>::value,
                  "");
  }
  {
    auto s = delete_from(bar);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::assert_where_called_t>::value,
                  "");
  }
  {
    auto s = update(bar).set(bar.id = 7);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::assert_where_called_t>::value,
                  "");
  }

  // `where` using unknown table
  {
    auto s = select(max(foo.id).as(something)).from(foo).where(bar.id > 7);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::consistent_t>::value,
                  "");
    static_assert(
        std::is_same<sqlpp::statement_prepare_check_t<S>,
                     sqlpp::assert_no_unknown_tables_in_where_t>::value,
        "");
  }

  // `where` statically using dynamic table
  {
    auto s = select(max(foo.id).as(something))
                 .from(foo.cross_join(dynamic(maybe, bar)))
                 .where(bar.id > 7);
    using S = decltype(s);
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<S>,
                     sqlpp::assert_no_unknown_static_tables_in_where_t>::value,
        "");
    static_assert(
        std::is_same<sqlpp::statement_prepare_check_t<S>,
                     sqlpp::assert_no_unknown_static_tables_in_where_t>::value,
        "");
  }
}

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

#include <sqlpp23/tests/core/constraints_helpers.h>

#include <sqlpp23/tests/core/tables.h>

namespace {
SQLPP_CREATE_NAME_TAG(something);

template <typename... Expressions>
concept can_call_single_table_with_standalone = requires(
    Expressions... expressions) { sqlpp::single_table(expressions...); };
template <typename... Expressions>
concept can_call_single_table_with_in_statement = requires(
    Expressions... expressions) {
  sqlpp::statement_t<sqlpp::no_single_table_t>{}.single_table(expressions...);
};

template <typename... Expressions>
concept can_call_single_table_with =
    can_call_single_table_with_standalone<Expressions...> and
    can_call_single_table_with_in_statement<Expressions...>;

template <typename... Expressions>
concept cannot_call_single_table_with =
    not(can_call_single_table_with_standalone<Expressions...> or
        can_call_single_table_with_in_statement<Expressions...>);
}  // namespace

int main() {
  const auto maybe = true;
  const auto bar = test::TabBar{};
  const auto foo = test::TabFoo{};
  const auto c = cte(something).as(select(bar.id).from(bar).where(true));

  // OK
  static_assert(can_call_single_table_with<decltype(bar)>);

  // Try dyanamic table
  static_assert(cannot_call_single_table_with<decltype(dynamic(maybe, bar))>);

  // Try assignment as table
  static_assert(cannot_call_single_table_with<decltype(bar.boolNn = true)>);

  // Try a column
  static_assert(cannot_call_single_table_with<decltype(bar.id)>);

  // Try some other types as tables
  static_assert(cannot_call_single_table_with<decltype("true")>);
  static_assert(cannot_call_single_table_with<decltype(17)>);
  static_assert(cannot_call_single_table_with<decltype('c')>);
  static_assert(cannot_call_single_table_with<decltype(nullptr)>);

  // Cannot call with cte or table alias.
  static_assert(cannot_call_single_table_with<decltype(bar.as(something))>);
  static_assert(cannot_call_single_table_with<decltype(c)>);
  static_assert(cannot_call_single_table_with<decltype(foo.cross_join(bar))>);

  // `single_table` is required
  {
    auto s = sqlpp::statement_t<sqlpp::no_single_table_t>{};
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::assert_single_table_provided_t>::value);
  }
}

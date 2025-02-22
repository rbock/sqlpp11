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
template <typename... Expressions>
concept can_call_offset_with_standalone =
    requires(Expressions... expressions) { sqlpp::offset(expressions...); };
template <typename... Expressions>
concept can_call_offset_with_in_statement =
    requires(Expressions... expressions) {
      sqlpp::statement_t<sqlpp::no_offset_t>{}.offset(expressions...);
    };

template <typename... Expressions>
concept can_call_offset_with =
    can_call_offset_with_standalone<Expressions...> and
    can_call_offset_with_in_statement<Expressions...>;

template <typename... Expressions>
concept cannot_call_offset_with =
    not(can_call_offset_with_standalone<Expressions...> or
        can_call_offset_with_in_statement<Expressions...>);
} // namespace

int main() {
  const auto maybe = true;
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // OK
  static_assert(can_call_offset_with<decltype(7u)>, "");
  static_assert(can_call_offset_with<decltype(7)>, "");
  static_assert(can_call_offset_with<decltype(bar.id)>, "");
  static_assert(can_call_offset_with<decltype(bar.intN)>, "nullable is OK");

  static_assert(can_call_offset_with<decltype(sqlpp::dynamic(maybe, 7u))>, "");
  static_assert(can_call_offset_with<decltype(sqlpp::dynamic(maybe, 7))>, "");
  static_assert(can_call_offset_with<decltype(dynamic(maybe, bar.id))>, "");

  // Try assignment or comparison
  static_assert(cannot_call_offset_with<decltype(bar.id = 7)>, "");
  static_assert(cannot_call_offset_with<decltype(bar.id == 7)>, "");

  // Try non-integral expression
  static_assert(cannot_call_offset_with<decltype(bar.textN)>, "");

  // Try some other types as expressions
  static_assert(cannot_call_offset_with<decltype("true")>, "");
  static_assert(cannot_call_offset_with<decltype('c')>, "");
  static_assert(cannot_call_offset_with<decltype(nullptr)>, "");

  // `offset` isn't required
  {
    auto s = sqlpp::statement_t<sqlpp::no_offset_t>{};
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::consistent_t>::value,
                  "");
  }

  // `offset` using unknown table
  {
    auto s = select(foo.id).from(foo).where(true).offset(bar.id);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::consistent_t>::value,
                  "");
    static_assert(
        std::is_same<sqlpp::statement_prepare_check_t<S>,
                     sqlpp::assert_no_unknown_tables_in_offset_t>::value,
        "");
  }

  // `offset` statically using dynamic table
  {
    auto s = select(foo.id)
                 .from(foo.cross_join(dynamic(maybe, bar)))
                 .where(true)
                 .offset(bar.id);
    using S = decltype(s);
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<S>,
                     sqlpp::assert_no_unknown_static_tables_in_offset_t>::value,
        "");
    static_assert(
        std::is_same<sqlpp::statement_prepare_check_t<S>,
                     sqlpp::assert_no_unknown_static_tables_in_offset_t>::value,
        "");
  }
}

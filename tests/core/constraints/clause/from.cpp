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
template <typename... Expressions>
concept can_call_from_with_standalone =
    requires(Expressions... expressions) { sqlpp::from(expressions...); };
template <typename... Expressions>
concept can_call_from_with_in_statement = requires(Expressions... expressions) {
  sqlpp::statement_t<sqlpp::no_from_t>{}.from(expressions...);
};

template <typename... Expressions>
concept can_call_from_with = can_call_from_with_standalone<Expressions...> and
                             can_call_from_with_in_statement<Expressions...>;

template <typename... Expressions>
concept cannot_call_from_with =
    not(can_call_from_with_standalone<Expressions...> or
        can_call_from_with_in_statement<Expressions...>);
} // namespace

int main() {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  from(foo);                 // this is OK since cross_joins are condition-free.
  from(foo.cross_join(bar)); // this is OK since cross_joins are condition-free.

  // from() arg must not be a non-table
  static_assert(can_call_from_with<decltype(foo)>, "");
  static_assert(can_call_from_with<decltype(foo.cross_join(bar))>, "");
  static_assert(cannot_call_from_with<decltype(foo.join(bar))>,
                "missing condition for join");
  static_assert(cannot_call_from_with<decltype(7)>, "not a table");
  static_assert(cannot_call_from_with<decltype(foo.id)>, "not a table");
}

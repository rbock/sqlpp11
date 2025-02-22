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
// Returns true if `concat(declval<Expressions>()...)` is a valid function call.
template <typename TypeVector, typename = void>
struct can_call_concat_with_impl : public std::false_type {};

template <typename... Expressions>
struct can_call_concat_with_impl<
    sqlpp::detail::type_vector<Expressions...>,
    std::void_t<decltype(sqlpp::concat(std::declval<Expressions>()...))>>
    : public std::true_type {};

template <typename... Expressions>
struct can_call_concat_with : public can_call_concat_with_impl<
                                  sqlpp::detail::type_vector<Expressions...>> {
};

} // namespace

int main() {
  const auto maybe = true;
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // concat(<non arguments>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::concat(),
                            "at least one argument required in concat()");

  // concat(<non-text-order arguments>) cannot be called.
  static_assert(not can_call_concat_with<decltype(bar.boolNn)>::value,
                "Not a text argument");
  static_assert(
      not can_call_concat_with<decltype(dynamic(true, bar.boolNn))>::value,
      "Not a text argument");
  static_assert(not can_call_concat_with<decltype((bar.id + 7).asc())>::value,
                "Not a text argument");
  static_assert(not can_call_concat_with<decltype(bar.id)>::value,
                "Not a text argument");
  static_assert(not can_call_concat_with<decltype(7)>::value,
                "Not a text argument");
  static_assert(not can_call_concat_with<decltype(bar.id = 7),
                                         decltype(bar.boolNn)>::value,
                "Not a text argument");
  static_assert(not can_call_concat_with<decltype(all_of(bar)),
                                         decltype(bar.boolNn)>::value,
                "Not a text argument");

  // concat(<one or more text arguments>) is OK
  static_assert(can_call_concat_with<decltype("a")>::value, "");
  static_assert(can_call_concat_with<decltype("a"), decltype("b")>::value, "");
  static_assert(can_call_concat_with<decltype(std::nullopt)>::value, "");
  static_assert(can_call_concat_with<decltype(foo.textNnD)>::value, "");
  static_assert(
      can_call_concat_with<decltype(dynamic(maybe, foo.textNnD))>::value, "");
}

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

  // Returns true if `declval<Expression>().as(declval<NameTagProvider>())` is a valid function call.
  template <typename Expression, typename NameTagProvider, typename = void>
  struct can_call_as_with : public std::false_type
  {
  };

  template <typename Expression, typename NameTagProvider>
  struct can_call_as_with<Expression, NameTagProvider, sqlpp::void_t<decltype(std::declval<Expression>().as(std::declval<NameTagProvider>()))>>
      : public std::true_type
  {
  };
}  // namespace

int main()
{
  const auto maybe = true;
  const auto bar = test::TabBar{};

  // OK
  static_assert(can_call_as_with<decltype(bar.id), decltype(something)>::value, "");
  static_assert(can_call_as_with<decltype(bar.id), decltype(bar)>::value, "");
  static_assert(can_call_as_with<decltype(bar), decltype(something)>::value, "");
  static_assert(can_call_as_with<decltype(select(bar.id).from(bar).where(true)), decltype(something)>::value, "");

  // OK, functions can be named
  static_assert(can_call_as_with<decltype(max(bar.boolNn)), decltype(something)>::value, "");

  // dynamic cannot be named can be named
  static_assert(not can_call_as_with<decltype(dynamic(maybe, bar.boolNn)), decltype(bar)>::value, "");

  // Renamed things cannot be renamed again.
  static_assert(not can_call_as_with<decltype(bar.id.as(something)), decltype(bar)>::value, "");

  // Things without a name cannot be used to rename something else.
  static_assert(not can_call_as_with<decltype(bar.id), decltype(maybe)>::value, "");
  static_assert(not can_call_as_with<decltype(bar.id), decltype(sqlpp::value(7))>::value, "");
}


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

  // Returns true if `from(declval<Expression>())` is a valid function call.
  template <typename Expression, typename = void>
  struct can_call_from_with : public std::false_type
  {
  };

  template <typename Expression>
  struct can_call_from_with<Expression, sqlpp::void_t<decltype(sqlpp::from(std::declval<Expression>()))>>
      : public std::true_type
  {
  };
}  // namespace

int main()
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  from(foo); // this is OK since cross_joins are condition-free.
  from(foo.cross_join(bar)); // this is OK since cross_joins are condition-free.

  // from() arg must not be a non-table
  static_assert(can_call_from_with<decltype(foo)>::value, "");
  static_assert(can_call_from_with<decltype(foo.cross_join(bar))>::value, "");
  static_assert(not can_call_from_with<decltype(foo.join(bar))>::value, "missing condition for join");
  static_assert(not can_call_from_with<decltype(7)>::value, "not a table");
  static_assert(not can_call_from_with<decltype(foo.id)>::value, "not a table");
}


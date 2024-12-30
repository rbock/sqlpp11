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

  // Returns true if `offset(declval<Expression>())` is a valid function call.
  template <typename Expression, typename = void>
  struct can_call_offset_with : public std::false_type
  {
  };

  template <typename Expression>
  struct can_call_offset_with<Expression, sqlpp::void_t<decltype(sqlpp::offset(std::declval<Expression>()))>>
      : public std::true_type
  {
  };
}  // namespace

int main()
{
  const auto maybe = true;
  const auto bar = test::TabBar{};

  // OK
  static_assert(can_call_offset_with<decltype(7u)>::value, "");
  static_assert(can_call_offset_with<decltype(7)>::value, "");
  static_assert(can_call_offset_with<decltype(bar.id)>::value, "");
  static_assert(can_call_offset_with<decltype(bar.intN)>::value, "nullable is OK");

  static_assert(can_call_offset_with<decltype(sqlpp::dynamic(maybe, 7u))>::value, "");
  static_assert(can_call_offset_with<decltype(sqlpp::dynamic(maybe, 7))>::value, "");
  static_assert(can_call_offset_with<decltype(dynamic(maybe, bar.id))>::value, "");

  // Try assignment or comparison
  static_assert(not can_call_offset_with<decltype(bar.id = 7)>::value, "");
  static_assert(not can_call_offset_with<decltype(bar.id == 7)>::value, "");

  // Try non-integral expression
  static_assert(not can_call_offset_with<decltype(bar.textN)>::value, "");

  // Try some other types as expressions
  static_assert(not can_call_offset_with<decltype("true")>::value, "");
  static_assert(not can_call_offset_with<decltype('c')>::value, "");
  static_assert(not can_call_offset_with<decltype(nullptr)>::value, "");

  // `offset` isn't required
  {
    auto s = sqlpp::statement_t<sqlpp::no_offset_t>{};
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }

}


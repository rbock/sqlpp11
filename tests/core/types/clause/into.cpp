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

#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/tests/core/types_helpers.h>

SQLPP_CREATE_NAME_TAG(cheese);

void test_into()
{
  auto v = sqlpp::value(17);
  auto foo = test::TabFoo{};
  auto bar = test::TabBar{};
  using Foo = decltype(foo);
  auto id = foo.id;

  // Valid into clause
  {
    using S = decltype(sqlpp::into(foo));
    using I = extract_clause_t<S>;
    static_assert(std::is_same<sqlpp::provided_tables_of_t<I>, sqlpp::detail::type_set<Foo>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<I>, sqlpp::provided_tables_of_t<I>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<I>, sqlpp::detail::type_set<>>::value, "");
  }

#warning: write constraint tests
  /*
  assert_invalid_argument(foo.join(bar));
  assert_invalid_argument(foo.as(cheese));
  assert_invalid_argument(id);
  */
}

int main()
{
  void test_into();
}


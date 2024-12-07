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

void test_get_last_if()
{
  auto v = sqlpp::value("text");
  auto col_int = test::TabFoo{}.id;

  // Ending on a matching type
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop>, sqlpp::noop>::value, "");
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop, int>, int>::value, "");
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop, int, float, long>, long>::value, "");
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop, int, float, long, short>, short>::value, "");
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop, int, float, long, short, size_t>, short>::value, "");
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_numeric, sqlpp::noop, int, float, long, short, size_t>, size_t>::value, "");

  // Ending on a non-matching type
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop, double>, sqlpp::noop>::value, "");
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop, int, double>, int>::value, "");
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop, int, float, double>, int>::value, "");
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop, int, float, long, double>, long>::value, "");
  static_assert(std::is_same<sqlpp::detail::get_last_if_t<sqlpp::is_integral, sqlpp::noop, int, float, long, short, double>, short>::value, "");
}

int main()
{
  void test_get_last_if();
}


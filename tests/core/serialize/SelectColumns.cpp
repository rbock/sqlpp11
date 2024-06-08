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

#include "compare.h"
#include "Sample.h"
#include <sqlpp11/sqlpp11.h>

int SelectColumns(int, char*[])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Single column
  compare(__LINE__, select(foo.omega), "SELECT tab_foo.omega");

  // Two columns
  compare(__LINE__, select(foo.omega, bar.alpha), "SELECT tab_foo.omega,tab_bar.alpha");

  // All columns of a table
  compare(__LINE__, select(all_of(foo)), "SELECT tab_foo.delta,tab_foo.epsilon,tab_foo.omega,tab_foo.psi,tab_foo.book");

  // All columns of a table plus one more
  compare(__LINE__, select(all_of(foo), bar.alpha), "SELECT tab_foo.delta,tab_foo.epsilon,tab_foo.omega,tab_foo.psi,tab_foo.book,tab_bar.alpha");

  // One more, plus all columns of a table
  compare(__LINE__, select(bar.alpha, all_of(foo)), "SELECT tab_bar.alpha,tab_foo.delta,tab_foo.epsilon,tab_foo.omega,tab_foo.psi,tab_foo.book");

  // Column and aggregate function
  compare(__LINE__, select(foo.omega, count(bar.alpha)), "SELECT tab_foo.omega,COUNT(tab_bar.alpha) AS count_");

  // Column aliases
  compare(__LINE__, select(foo.omega.as(sqlpp::alias::o), count(bar.alpha).as(sqlpp::alias::a)), "SELECT tab_foo.omega AS o,COUNT(tab_bar.alpha) AS a");

#warning: add optional column tests

  return 0;
}

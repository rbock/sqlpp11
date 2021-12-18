/*
 * Copyright (c) 2016-2016, Roland Bock
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

#include <iostream>

namespace
{
  auto getFalse() -> std::string
  {
    MockDb::_serializer_context_t printer = {};
    return serialize(sqlpp::value(false), printer).str();
  }
}

int In(int, char* [])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Individual values
  compare(__LINE__, foo.omega.in(17), "tab_foo.omega IN(17)");
  compare(__LINE__, foo.omega.in(17, bar.alpha), "tab_foo.omega IN(17,tab_bar.alpha)");
  compare(__LINE__, foo.omega.in(17, bar.alpha, sqlpp::value(19)), "tab_foo.omega IN(17,tab_bar.alpha,19)");

  // Lists
  compare(__LINE__, foo.omega.in(sqlpp::value_list(std::vector<float>{1.7f, 2.5f, 17.f, 0.f})),
          "tab_foo.omega IN(1.7,2.5,17,0)");

  // Sub select
  compare(__LINE__, foo.omega.in(select(bar.alpha).from(bar).unconditionally()),
          "tab_foo.omega IN(SELECT tab_bar.alpha FROM tab_bar)");

  // Empty lists (not normally covered by SQL)
  compare(__LINE__, foo.omega.in(), getFalse());
  compare(__LINE__, foo.omega.in(sqlpp::value_list(std::vector<int>{})), getFalse());

  return 0;
}

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
  auto getTrue() -> std::string
  {
    MockDb::_serializer_context_t printer = {};
    return serialize(sqlpp::value(true), printer).str();
  }

  auto getFalse() -> std::string
  {
    MockDb::_serializer_context_t printer = {};
    return serialize(sqlpp::value(false), printer).str();
  }
}

int Where(int, char* [])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Unconditionally
  compare(__LINE__, select(foo.omega).from(foo).unconditionally(), "SELECT tab_foo.omega FROM tab_foo");
  compare(__LINE__, remove_from(foo).unconditionally(), "DELETE FROM tab_foo");
  compare(__LINE__, update(foo).set(foo.omega = 42).unconditionally(), "UPDATE tab_foo SET omega=42");
  compare(__LINE__, where(sqlpp::value(true)), " WHERE " + getTrue());

  // Never
  compare(__LINE__, where(sqlpp::value(false)), " WHERE " + getFalse());

  // Sometimes
  compare(__LINE__, where(bar.gamma), " WHERE tab_bar.gamma");
  compare(__LINE__, where(bar.gamma == false), " WHERE (tab_bar.gamma=" + getFalse() + ")");
  compare(__LINE__, where(bar.beta == "SQL"), " WHERE (tab_bar.beta='SQL')");

  return 0;
}

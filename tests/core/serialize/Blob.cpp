/*
 * Copyright (c) 2017, Roland Bock
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
#include <array>

namespace
{
  /*
  auto getTrue() -> std::string
  {
    MockDb::_serializer_context_t printer = {};
    return serialize(sqlpp::value(true), printer).str();
  }
  */

  auto getFalse() -> std::string
  {
    MockDb::_serializer_context_t printer = {};
    return serialize(sqlpp::value(false), printer).str();
  }

  auto toByteVector(const std::string& s) -> std::vector<std::uint8_t>
  {
    return std::vector<std::uint8_t>(s.begin(), s.end());
  }
}  // namespace

int Blob(int, char*[])
{
  const auto foo = test::TabFoo{};
  // const auto bar = test::TabBar{};

  // Unconditionally
  compare(__LINE__, select(foo.book).from(foo).where(foo.book == toByteVector("john doe")),
          "SELECT tab_foo.book FROM tab_foo WHERE (tab_foo.book=x'6A6F686E20646F65')");

  std::array<uint8_t, 8> arr{{'j', 'o', 'h', 'n', ' ', 'd', 'o', 'e'}};
  compare(__LINE__, select(foo.book).from(foo).where(foo.book == arr),
          "SELECT tab_foo.book FROM tab_foo WHERE (tab_foo.book=x'6A6F686E20646F65')");

  // Never
  compare(__LINE__, where(sqlpp::value(false)), " WHERE " + getFalse());

  return 0;
}

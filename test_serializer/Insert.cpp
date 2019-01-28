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
}

int Insert(int, char* [])
{
  const auto bar = test::TabBar{};

  compare(__LINE__, insert_into(bar).default_values(), "INSERT INTO tab_bar DEFAULT VALUES");
  compare(__LINE__, insert_into(bar).set(bar.beta = "cheesecake", bar.gamma = true),
          "INSERT INTO tab_bar (beta,gamma) VALUES('cheesecake'," + getTrue() + ")");
#if __cplusplus >= 201703L
  // string_view argument
  std::string_view cheeseCake = "cheesecake";
  compare(__LINE__, insert_into(bar).set(bar.beta = cheeseCake, bar.gamma = true),
          "INSERT INTO tab_bar (beta,gamma) VALUES('cheesecake'," + getTrue() + ")");
#endif

  return 0;
}

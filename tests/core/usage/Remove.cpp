/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <iostream>
#include <sqlpp11/sqlpp11.h>
#include "Sample.h"
#include "MockDb.h"
#include "is_regular.h"

int Remove(int, char* [])
{
  MockDb db = {};
  MockDb::_serializer_context_t printer = {};

  const auto t = test::TabBar{};
  const auto f = test::TabFoo{};

  {
    using T = decltype(remove_from(t));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  {
    using T = decltype(remove_from(t).where(t.textN != "transparent"));
    auto x = remove_from(t).where(t.textN != "transparent");
    T y(x);
    T z(std::move(x));
    static_assert(sqlpp::is_regular<T>::value, "type requirement");
  }

  serialize(printer, remove_from(t)).str();
  serialize(printer, remove_from(t).where(t.textN != "transparent")).str();
  serialize(printer, remove_from(t).using_(t)).str();
  serialize(printer, remove_from(t).using_(f)).str();
#warning: add tests with optional using and optional where
  printer.reset();
  std::cerr << serialize(printer, remove_from(t).unconditionally()).str() << std::endl;

  remove_from(t).where(t.textN.in(select(f.textNnD).from(f).unconditionally()));

  return 0;
}

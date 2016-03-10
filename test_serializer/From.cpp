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

namespace
{
  MockDb db = {};
}

int From(int, char* [])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};
  const auto aFoo = foo.as(sqlpp::alias::a);
  const auto bFoo = foo.as(sqlpp::alias::b);
  const auto cFoo = foo.as(sqlpp::alias::c);

  // Single table
  compare(__LINE__, from(foo), " FROM tab_foo");
  compare(__LINE__, from(bar), " FROM tab_bar");

  // Static joins
  compare(__LINE__, from(foo.cross_join(bar)), " FROM tab_foo CROSS JOIN tab_bar");
  compare(__LINE__, from(foo.join(bar).on(foo.omega > bar.alpha)),
          " FROM tab_foo INNER JOIN tab_bar ON (tab_foo.omega>tab_bar.alpha)");
  compare(__LINE__, from(aFoo.join(bFoo).on(aFoo.omega > bFoo.omega)),
          " FROM tab_foo AS a INNER JOIN tab_foo AS b ON (a.omega>b.omega)");
  compare(
      __LINE__, from(aFoo.join(bFoo).on(aFoo.omega > bFoo.omega).join(cFoo).on(bFoo.omega > cFoo.omega)),
      " FROM tab_foo AS a INNER JOIN tab_foo AS b ON (a.omega>b.omega) INNER JOIN tab_foo AS c ON (b.omega>c.omega)");

  // Dynamic joins
  const auto df = dynamic_from(db, foo);
  compare(__LINE__, df, " FROM tab_foo");
  {
    auto dfa = df;
    dfa.from.add(dynamic_cross_join(bar));
    compare(__LINE__, dfa, " FROM tab_foo CROSS JOIN tab_bar");
  }
  {
    auto dfa = df;
    dfa.from.add(dynamic_inner_join(bar).on(bar.alpha > foo.omega));
    compare(__LINE__, dfa, " FROM tab_foo INNER JOIN tab_bar ON (tab_bar.alpha>tab_foo.omega)");
  }
  {
    auto dfa = df;
    dfa.from.add(dynamic_inner_join(bar).on(bar.alpha > foo.omega));
    dfa.from.add(dynamic_outer_join(aFoo).on(bar.alpha > aFoo.omega));
    compare(__LINE__, dfa, " FROM tab_foo INNER JOIN tab_bar ON (tab_bar.alpha>tab_foo.omega) OUTER JOIN tab_foo AS a "
                           "ON (tab_bar.alpha>a.omega)");
  }

  return 0;
}

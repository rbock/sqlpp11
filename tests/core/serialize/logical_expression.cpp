/*
 * Copyright (c) 2021-2021, Roland Bock
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

#include "Sample.h"
#include "compare.h"
#include <sqlpp11/sqlpp11.h>

#include <iostream>

int logical_expression(int, char* [])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Basic logical expression.
  compare(__LINE__, foo.boolN and bar.boolNn, "(tab_foo.bool_n AND tab_bar.bool_nn)");
  compare(__LINE__, foo.boolN or bar.boolNn, "(tab_foo.bool_n OR tab_bar.bool_nn)");
  compare(__LINE__, not foo.boolN, "(NOT tab_foo.bool_n)");

  // With dynamic part.
  compare(__LINE__, foo.boolN and dynamic(true, bar.boolNn), "(tab_foo.bool_n AND tab_bar.bool_nn)");
  compare(__LINE__, foo.boolN and dynamic(false, bar.boolNn), "tab_foo.bool_n");

  compare(__LINE__, foo.boolN or dynamic(true, bar.boolNn), "(tab_foo.bool_n OR tab_bar.bool_nn)");
  compare(__LINE__, foo.boolN or dynamic(false, bar.boolNn), "tab_foo.bool_n");

  // Advanced logical expression.
  compare(__LINE__, not foo.boolN and not bar.boolNn, "((NOT tab_foo.bool_n) AND (NOT tab_bar.bool_nn))");
  compare(__LINE__, not foo.boolN or not bar.boolNn, "((NOT tab_foo.bool_n) OR (NOT tab_bar.bool_nn))");
  compare(__LINE__, not (foo.boolN and bar.boolNn), "(NOT (tab_foo.bool_n AND tab_bar.bool_nn))");
  compare(__LINE__, not (foo.boolN or bar.boolNn), "(NOT (tab_foo.bool_n OR tab_bar.bool_nn))");

  // With dynamic part.
  compare(__LINE__, not foo.boolN and dynamic(true, not bar.boolNn), "((NOT tab_foo.bool_n) AND (NOT tab_bar.bool_nn))");
  compare(__LINE__, not foo.boolN and dynamic(false, not bar.boolNn), "(NOT tab_foo.bool_n)");

  compare(__LINE__, not foo.boolN or dynamic(true, not bar.boolNn), "((NOT tab_foo.bool_n) OR (NOT tab_bar.bool_nn))");
  compare(__LINE__, not foo.boolN or dynamic(false, not bar.boolNn), "(NOT tab_foo.bool_n)");

#warning: Consider reducing braces a bit as in sqlpp17

  // Chained expression.
  compare(__LINE__, foo.boolN and bar.boolNn and (bar.id > 17),
          "((tab_foo.bool_n AND tab_bar.bool_nn) AND (tab_bar.id > 17))");
  compare(__LINE__, foo.boolN or bar.boolNn or (bar.id > 17),
          "((tab_foo.bool_n OR tab_bar.bool_nn) OR (tab_bar.id > 17))");

  compare(__LINE__, foo.boolN and bar.boolNn and dynamic(true, bar.id > 17),
          "((tab_foo.bool_n AND tab_bar.bool_nn) AND (tab_bar.id > 17))");
  compare(__LINE__, foo.boolN and bar.boolNn and dynamic(false, bar.id > 17),
          "(tab_foo.bool_n AND tab_bar.bool_nn)");
  compare(__LINE__, foo.boolN or bar.boolNn or dynamic(true, bar.id > 17),
          "((tab_foo.bool_n OR tab_bar.bool_nn) OR (tab_bar.id > 17))");
  compare(__LINE__, foo.boolN or bar.boolNn or dynamic(false, bar.id > 17),
          "(tab_foo.bool_n OR tab_bar.bool_nn)");

  compare(__LINE__, foo.boolN and dynamic(true, bar.boolNn and (bar.id > 17)),
          "(tab_foo.bool_n AND (tab_bar.bool_nn AND (tab_bar.id > 17)))");
  compare(__LINE__, foo.boolN and dynamic(false, bar.boolNn and (bar.id > 17)),
          "tab_foo.bool_n");
  compare(__LINE__, foo.boolN or dynamic(true, bar.boolNn or (bar.id > 17)),
          "(tab_foo.bool_n OR (tab_bar.bool_nn OR (tab_bar.id > 17)))");
  compare(__LINE__, foo.boolN or dynamic(false, bar.boolNn or (bar.id > 17)),
          "tab_foo.bool_n");

  return 0;
}

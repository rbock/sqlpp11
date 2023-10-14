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

int Operator(int, char* [])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Plus
  compare(__LINE__, bar.alpha + 3u, "(tab_bar.alpha+3)");
  compare(__LINE__, sqlpp::value(3) + foo.psi, "(3+tab_foo.psi)");

  // Shift left
  compare(__LINE__, sqlpp::value(3) << foo.psi, "(3<<tab_foo.psi)");
  compare(__LINE__, bar.alpha << 3u, "(tab_bar.alpha<<3)");

  // Shift right
  compare(__LINE__, sqlpp::value(3) >> foo.psi, "(3>>tab_foo.psi)");
  compare(__LINE__, bar.alpha >> 3u, "(tab_bar.alpha>>3)");

  // Comparison
  compare(__LINE__, bar.alpha < 3u, "(tab_bar.alpha<3)");
  compare(__LINE__, bar.alpha <= 3u, "(tab_bar.alpha<=3)");
  compare(__LINE__, bar.alpha == 3u, "(tab_bar.alpha=3)");
  compare(__LINE__, bar.alpha != 3u, "(tab_bar.alpha<>3)");
  compare(__LINE__, bar.alpha >= 3u, "(tab_bar.alpha>=3)");
  compare(__LINE__, bar.alpha > 3u, "(tab_bar.alpha>3)");

  return 0;
}

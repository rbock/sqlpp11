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

SQLPP_ALIAS_PROVIDER(cheese);
SQLPP_ALIAS_PROVIDER(maxId);

int As(int, char*[])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  compare(__LINE__, foo, "tab_foo");
  compare(__LINE__, foo.doubleN.as(cheese), "tab_foo.double_n AS cheese");
  compare(__LINE__, (foo.doubleN + 17).as(cheese), "(tab_foo.double_n + 17) AS cheese");
  compare(__LINE__, (foo.doubleN - 17).as(cheese), "(tab_foo.double_n - 17) AS cheese");
  compare(__LINE__, (foo.doubleN - uint32_t(17)).as(cheese), "(tab_foo.double_n - 17) AS cheese");
  compare(__LINE__, (foo.doubleN - bar.id).as(cheese), "(tab_foo.double_n - tab_bar.id) AS cheese");
  compare(__LINE__, (count(foo.doubleN) - bar.id).as(cheese), "(COUNT(tab_foo.double_n) - tab_bar.id) AS cheese");
  compare(__LINE__, (count(foo.doubleN) - uint32_t(17)).as(cheese), "(COUNT(tab_foo.double_n) - 17) AS cheese");

  compare(__LINE__, select(max(bar.id).as(maxId)), "SELECT MAX(tab_bar.id) AS maxId");
  compare(__LINE__, select(max(bar.id).as(cheese)), "SELECT MAX(tab_bar.id) AS cheese");
  compare(__LINE__, select(max(bar.id).as(maxId)).from(bar).unconditionally().as(cheese),
          "(SELECT MAX(tab_bar.id) AS maxId FROM tab_bar) AS cheese");
  compare(__LINE__, select(max(bar.id).as(maxId)).from(bar).unconditionally().as(cheese).maxId, "cheese.maxId");

  return 0;
}

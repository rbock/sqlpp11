/*
 * Copyright (c) 2016, Roland Bock
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

SQLPP_ALIAS_PROVIDER(cheese);

int Over(int, char* []) {
  auto const foo = test::TabFoo{};

  SQLPP_COMPARE(select(avg(foo.doubleN).over().as(cheese)), "SELECT AVG(tab_foo.double_n) OVER() AS cheese");
  SQLPP_COMPARE(select(count(foo.doubleN).over().as(cheese)), "SELECT COUNT(tab_foo.double_n) OVER() AS cheese");
  SQLPP_COMPARE(select(max(foo.doubleN).over().as(cheese)), "SELECT MAX(tab_foo.double_n) OVER() AS cheese");
  SQLPP_COMPARE(select(min(foo.doubleN).over().as(cheese)), "SELECT MIN(tab_foo.double_n) OVER() AS cheese");
  SQLPP_COMPARE(select(sum(foo.doubleN).over().as(cheese)), "SELECT SUM(tab_foo.double_n) OVER() AS cheese");

  return 0;
}

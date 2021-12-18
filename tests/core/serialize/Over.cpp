/*
 * Copyright (c) 2016-2020, Roland Bock, MacDue
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

SQLPP_ALIAS_PROVIDER(dueutil)

int Over(int, char* []) {
  auto const foo = test::TabFoo{};

  // no/auto alias (wrapped in select so alias is applied)
  compare(__LINE__, select(avg(foo.omega).over()), "SELECT AVG(tab_foo.omega) OVER() AS avg_");
  compare(__LINE__, select(count(foo.omega).over()), "SELECT COUNT(tab_foo.omega) OVER() AS count_");
  compare(__LINE__, select(max(foo.omega).over()), "SELECT MAX(tab_foo.omega) OVER() AS max_");
  compare(__LINE__, select(min(foo.omega).over()), "SELECT MIN(tab_foo.omega) OVER() AS min_");
  compare(__LINE__, select(sum(foo.omega).over()), "SELECT SUM(tab_foo.omega) OVER() AS sum_");

  // alias
  compare(__LINE__, avg(foo.omega).over().as(dueutil), "AVG(tab_foo.omega) OVER() AS dueutil");
  compare(__LINE__, count(foo.omega).over().as(dueutil), "COUNT(tab_foo.omega) OVER() AS dueutil");
  compare(__LINE__, max(foo.omega).over().as(dueutil), "MAX(tab_foo.omega) OVER() AS dueutil");
  compare(__LINE__, min(foo.omega).over().as(dueutil), "MIN(tab_foo.omega) OVER() AS dueutil");
  compare(__LINE__, sum(foo.omega).over().as(dueutil), "SUM(tab_foo.omega) OVER() AS dueutil");

  return 0;
}

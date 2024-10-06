/*
 * Copyright (c) 2024, Roland Bock
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
#include "../compare.h"
#include <sqlpp11/sqlpp11.h>

int main(int, char* [])
{
  const auto val = sqlpp::value(17);
  const auto expr = sqlpp::value(17) + 4;

  const auto foo = test::TabFoo{};

  // Without condition.
  SQLPP_COMPARE(sqlpp::having(true), " HAVING 1");

  // Whith static condition.
  SQLPP_COMPARE(sqlpp::having(foo.boolN), " HAVING tab_foo.bool_n");
  SQLPP_COMPARE(sqlpp::having(foo.boolN.is_not_distinct_from(true)), " HAVING tab_foo.bool_n IS NOT DISTINCT FROM 1");
  SQLPP_COMPARE(sqlpp::having(foo.id > 17), " HAVING tab_foo.id > 17");
  SQLPP_COMPARE(sqlpp::having(max(foo.id) > 17), " HAVING MAX(tab_foo.id) > 17");

  // With dynamic condition.
  SQLPP_COMPARE(sqlpp::having(dynamic(true, max(foo.id) > 17)), " HAVING MAX(tab_foo.id) > 17");
  SQLPP_COMPARE(sqlpp::having(dynamic(false, max(foo.id) > 17)), "");

  return 0;
}
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

#include "../compare.h"
#include "Sample.h"
#include <sqlpp11/sqlpp11.h>

int main(int, char* [])
{
  constexpr auto t = test::TabFoo{};
  const auto val = sqlpp::value(17);

  // Operands in assignments are enclosed in parentheses as required.
  SQLPP_COMPARE(t.id = val, "id = 17");
  SQLPP_COMPARE(t.id = val + 4, "id = (17 + 4)");

  // Active dynamic assignments are just as above.
  SQLPP_COMPARE(dynamic(true, t.id = val), "id = 17");
  SQLPP_COMPARE(dynamic(true, t.id = val + 4), "id = (17 + 4)");

  // This should be skipped by insert and update and should therefore never be called.
  SQLPP_COMPARE(dynamic(false, t.id = val), "NULL");
  SQLPP_COMPARE(dynamic(false, t.id = val + 4), "NULL");

  return 0;
}

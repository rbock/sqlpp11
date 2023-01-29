/*
 * Copyright (c) 2023-2023, Roland Bock
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

int Count(int, char* [])
{
  const auto bar = test::TabBar{};

  // Single column.
  compare(__LINE__, count(bar.alpha), "COUNT(tab_bar.alpha)");
  compare(__LINE__, count(sqlpp::distinct, bar.alpha), "COUNT(DISTINCT tab_bar.alpha)");

  // Expression.
  // Note that the inner parens aren't necessary.
  compare(__LINE__, count(bar.alpha + 7), "COUNT((tab_bar.alpha+7))");
  compare(__LINE__, count(sqlpp::distinct, bar.alpha + 7), "COUNT(DISTINCT (tab_bar.alpha+7))");

  // With sub select.
  compare(__LINE__, count(select(sqlpp::value(7).as(sqlpp::alias::a))), "COUNT((SELECT 7 AS a))");
  compare(__LINE__, count(sqlpp::distinct, select(sqlpp::value(7).as(sqlpp::alias::a))), "COUNT(DISTINCT (SELECT 7 AS a))");

  return 0;
}

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

#include <sqlpp11/tests/core/constraints_helpers.h>
#include <sqlpp11/tests/core/tables.h>

namespace test
{
  SQLPP_CREATE_NAME_TAG(tab);
};

int main()
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // from() arg must not be a pre-join (i.e. a join with missing condition)
  from(foo.cross_join(bar)); // this is OK since cross_joins are condition-free.
  SQLPP_CHECK_STATIC_ASSERT(from(foo.join(bar)), "from() join argument is missing condition, please use an explicit on() condition");

  // from() arg must not be a non-table
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::from(7),
                           "from() argument has to be a table or join expression");
  SQLPP_CHECK_STATIC_ASSERT(from(foo.id),
                           "from() argument has to be a table or join expression");

  // from() select arg must be a self-contained select.
  from(select(all_of(foo)).from(foo).unconditionally().as(test::tab)); // OK, self-contained
  SQLPP_CHECK_STATIC_ASSERT(from(select(all_of(foo)).as(test::tab)),
                          "at least one selected column requires a table which is otherwise not known in the statement");
}


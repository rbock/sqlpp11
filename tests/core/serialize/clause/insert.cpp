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

#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/tests/core/serialize_helpers.h>
#include <sqlpp11/sqlpp11.h>

SQLPP_CREATE_NAME_TAG(v);

int main(int, char* [])
{
  const auto val = sqlpp::value(17);
  const auto expr = sqlpp::value(17) + 4;

  const auto foo = test::TabFoo{};

  // Without values.
  SQLPP_COMPARE(insert_into(foo), "INSERT INTO tab_foo");

  // Default values.
  SQLPP_COMPARE(insert_into(foo).default_values(), "INSERT INTO tab_foo DEFAULT VALUES");

  // Single row with "SET".
  SQLPP_COMPARE(insert_into(foo).set(foo.id = 17, foo.boolN = std::nullopt, foo.textNnD = "cake"), "INSERT INTO tab_foo (id, bool_n, text_nn_d) VALUES(17, NULL, 'cake')");

  // Multiple rows with columns and values.
  {
    auto i = insert_into(foo).columns(foo.id, foo.boolN, foo.textNnD);
    i.add_values(foo.id = sqlpp::default_value, foo.boolN = sqlpp::default_value, foo.textNnD = "cheese");
    i.add_values(foo.id = 17, foo.boolN = std::nullopt, foo.textNnD = "cake");
    SQLPP_COMPARE(i, "INSERT INTO tab_foo (id, bool_n, text_nn_d) VALUES (DEFAULT, DEFAULT, 'cheese'), (17, NULL, 'cake')");
  }

  return 0;
}

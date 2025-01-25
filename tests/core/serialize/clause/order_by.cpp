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

  // Plain columns.
  SQLPP_COMPARE(order_by(foo.id.asc()), " ORDER BY tab_foo.id ASC");
  SQLPP_COMPARE(order_by(foo.textNnD.asc()), " ORDER BY tab_foo.text_nn_d ASC");
  SQLPP_COMPARE(order_by(foo.boolN.asc()), " ORDER BY tab_foo.bool_n ASC");

  SQLPP_COMPARE(order_by(foo.id.desc()), " ORDER BY tab_foo.id DESC");
  SQLPP_COMPARE(order_by(foo.textNnD.desc()), " ORDER BY tab_foo.text_nn_d DESC");
  SQLPP_COMPARE(order_by(foo.boolN.desc()), " ORDER BY tab_foo.bool_n DESC");

  // Multiple plain columns.
  SQLPP_COMPARE(order_by(foo.id.asc(), foo.textNnD.desc(), foo.boolN.desc()), " ORDER BY tab_foo.id ASC, tab_foo.text_nn_d DESC, tab_foo.bool_n DESC");

  // Single dynamic column (this is odd)
  SQLPP_COMPARE(order_by(dynamic(true, foo.id.asc())), " ORDER BY tab_foo.id ASC");
  SQLPP_COMPARE(order_by(dynamic(false, foo.id.asc())), "");

  // Multiple dynamic columns (this is odd if all are dynamic)
  SQLPP_COMPARE(order_by(dynamic(true, foo.id.asc()), foo.textNnD.asc(), foo.boolN.asc()), " ORDER BY tab_foo.id ASC, tab_foo.text_nn_d ASC, tab_foo.bool_n ASC");
  SQLPP_COMPARE(order_by(foo.id.asc(), dynamic(true, foo.textNnD.asc()), foo.boolN.asc()), " ORDER BY tab_foo.id ASC, tab_foo.text_nn_d ASC, tab_foo.bool_n ASC");
  SQLPP_COMPARE(order_by(foo.id.asc(), foo.textNnD.asc(), dynamic(true, foo.boolN.asc())), " ORDER BY tab_foo.id ASC, tab_foo.text_nn_d ASC, tab_foo.bool_n ASC");

  SQLPP_COMPARE(order_by(dynamic(false, foo.id.asc()), foo.textNnD.asc(), foo.boolN.asc()), " ORDER BY tab_foo.text_nn_d ASC, tab_foo.bool_n ASC");
  SQLPP_COMPARE(order_by(foo.id.asc(), dynamic(false, foo.textNnD.asc()), foo.boolN.asc()), " ORDER BY tab_foo.id ASC, tab_foo.bool_n ASC");
  SQLPP_COMPARE(order_by(foo.id.asc(), foo.textNnD.asc(), dynamic(false, foo.boolN.asc())), " ORDER BY tab_foo.id ASC, tab_foo.text_nn_d ASC");

  SQLPP_COMPARE(order_by(foo.id.asc(), dynamic(false, foo.textNnD.asc()), dynamic(false, foo.boolN.asc())), " ORDER BY tab_foo.id ASC");
  SQLPP_COMPARE(order_by(dynamic(false, foo.id.asc()), foo.textNnD.asc(), dynamic(false, foo.boolN.asc())), " ORDER BY tab_foo.text_nn_d ASC");
  SQLPP_COMPARE(order_by(dynamic(false, foo.id.asc()), dynamic(false, foo.textNnD.asc()), foo.boolN.asc()), " ORDER BY tab_foo.bool_n ASC");

  SQLPP_COMPARE(order_by(dynamic(false, foo.id.asc()), dynamic(false, foo.textNnD.asc()), dynamic(false, foo.boolN.asc())), "");

  return 0;
}

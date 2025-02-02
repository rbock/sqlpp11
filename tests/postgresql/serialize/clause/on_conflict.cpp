/*
 * Copyright (c) 2025, Roland Bock
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

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/postgresql.h>

#include <sqlpp11/tests/postgresql/serialize_helpers.h>
#include <sqlpp11/tests/postgresql/tables.h>

int main()
{
  using sqlpp::postgresql::on_conflict;

  auto foo = test::TabFoo{};

  // Testing ON CONFLICT with zero or more conflict targets
  SQLPP_COMPARE(on_conflict(), " ON CONFLICT");
  SQLPP_COMPARE(on_conflict(foo.id), " ON CONFLICT (id)");
  SQLPP_COMPARE(on_conflict(dynamic(true, foo.id)), " ON CONFLICT (id)");
  SQLPP_COMPARE(on_conflict(dynamic(false, foo.id)), " ON CONFLICT");
  SQLPP_COMPARE(on_conflict(foo.id, foo.textNnD), " ON CONFLICT (id, text_nn_d)");
  SQLPP_COMPARE(on_conflict(dynamic(true, foo.id), foo.textNnD), " ON CONFLICT (id, text_nn_d)");
  SQLPP_COMPARE(on_conflict(dynamic(false, foo.id), foo.textNnD), " ON CONFLICT (text_nn_d)");

  // Testing NO NOTHING
  SQLPP_COMPARE(on_conflict().do_nothing(), " ON CONFLICT DO NOTHING");

  // Testing DO UPDATE
  SQLPP_COMPARE(on_conflict(foo.id).do_update(foo.id = 7), " ON CONFLICT (id) DO UPDATE SET id = 7");
  SQLPP_COMPARE(on_conflict(foo.id).do_update(foo.id = 7, foo.textNnD = "cake"), " ON CONFLICT (id) DO UPDATE SET id = 7, text_nn_d = 'cake'");
  SQLPP_COMPARE(on_conflict(foo.id).do_update(dynamic(true, foo.id = 7), foo.textNnD = "cake"), " ON CONFLICT (id) DO UPDATE SET id = 7, text_nn_d = 'cake'");
  SQLPP_COMPARE(on_conflict(foo.id).do_update(dynamic(false, foo.id = 7), foo.textNnD = "cake"), " ON CONFLICT (id) DO UPDATE SET text_nn_d = 'cake'");

  // Testing WHERE
  SQLPP_COMPARE(on_conflict(foo.id).do_update(foo.id = 7).where(true), " ON CONFLICT (id) DO UPDATE SET id = 7 WHERE 't'");
  SQLPP_COMPARE(on_conflict(foo.id).do_update(foo.id = 7).where(foo.id == 17), " ON CONFLICT (id) DO UPDATE SET id = 7 WHERE tab_foo.id = 17");
  SQLPP_COMPARE(on_conflict(foo.id).do_update(foo.id = 7).where(foo.id == 17 and foo.textNnD > "cheese"), " ON CONFLICT (id) DO UPDATE SET id = 7 WHERE (tab_foo.id = 17) AND (tab_foo.text_nn_d > 'cheese')");

  return 0;
}

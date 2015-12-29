/*
 * Copyright (c) 2014-2015 Roland Bock
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
#include "MockDb.h"
#include <sqlpp11/sqlpp11.h>

int insert(int, char* [])
{
  MockDb db{};

  const auto p = test::TabPerson{};
  const auto f = test::TabFeature{};

  db(insert_into(f).set(f.name = "loves c++", f.fatal = false));

  // db(insert_into(f).set(f.nahme = "loves c++", f.fatal = false));

  // db(insert_into(f).set(f.name == "loves c++", f.fatal = false));

  // db(insert_into(f).set(f.name = "loves c++", f.fatal = "false"));

  // db(insert_into(p).set(f.name = "loves c++", f.fatal = false));

  // db(insert_into(f).set(f.name = "loves c++", p.feature = 7));

  // db(insert_into(f).set(f.id = 42, f.name = "loves c++", f.fatal = false));

  // db(insert_into(f).set(f.name = "loves c++"));

  db(insert_into(f).default_values());

  auto i = insert_into(p).columns(p.name, p.feature);
  i.values.add(p.name = "Roland", p.feature = 1);
  i.values.add(p.name = "Zaphod", p.feature = sqlpp::default_value);
  db(i);

  auto pi = db.prepare(insert_into(p).set(p.name = parameter(f.name), p.feature = parameter(p.feature)));
  pi.params.name = "likes java";
  pi.params.feature = true;

  db(pi);
  return 0;
}

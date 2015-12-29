/*
 * Copyright (c) 2014-2015, niXman (i dot nixman dog gmail dot com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if 0  // syntax example
SQLPP_DECLARE_TABLE(
	(table, \
		 SQLPP_DROP_IF_EXISTS \
		,SQLPP_CREATE_IF_NOT_EXISTS \
		,SQLPP_ENGINE("InnoDB") \
		,SQLPP_CHARACTER_SET("utf-8") \
		,SQLPP_COMMENT("table comments") \
	)
	,
	(id, int, SQLPP_NOT_NULL, SQLPP_PRIMARY_KEY, SQLPP_AUTO_INCREMENT)
	(name, varchar(64), SQLPP_NOT_NULL, SQLPP_INDEX("name_index"), SQLPP_DEFAULT("any name"))
	(age, int, SQLPP_NOT_NULL, SQLPP_INDEX("age_index"), SQLPP_UNIQUE, SQLPP_COMMENT("some comments"))
)
#endif

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/ppgen.h>

#include "MockDb.h"

// clang-format off
SQLPP_DECLARE_TABLE(
	(tab_person)
	,
	(id     , int         , SQLPP_PRIMARY_KEY)
	(name   , varchar(255), SQLPP_NOT_NULL   )
	(feature, int         , SQLPP_NOT_NULL   )
)

SQLPP_DECLARE_TABLE(
	(tab_feature)
	,
	(id   , int         , SQLPP_PRIMARY_KEY)
	(name , varchar(255), SQLPP_NULL       )
	(fatal, bool        , SQLPP_NOT_NULL   )
)
// clang-format on

int ppgen(int, char* [])
{
  MockDb db{};
  const auto p = tab_person::tab_person{};
  const auto f = tab_feature::tab_feature{};

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

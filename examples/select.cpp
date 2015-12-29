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

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-variable"
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include "Sample.h"
#include "MockDb.h"
#include <sqlpp11/sqlpp11.h>

SQLPP_ALIAS_PROVIDER(cheesecake)

int select(int, char* [])
{
  static constexpr bool some_condition = true;
  static constexpr bool some_other_condition = false;

  MockDb db{};

  const auto p = test::TabPerson{};
#if 0
  const auto f = test::TabFeature{};
#endif

  for (const auto& row : db(select(all_of(p)).from(p).where(p.id > 7)))
  {
    int64_t id = row.id;
    std::string name = row.name;
    int64_t feature = row.feature;
  }

#if 0
	for (const auto& row : db(select(p.name).from(p).where(p.name.like("Herb%"))))
	{
		int64_t id = row.id;
		std::string name = row.name;
		int64_t feature = row.feature;
	}
#endif

#if 0
	for (const auto& row : db(select(p.name, f.name.as(cheesecake)).from(p,f).where(p.id > 7 and p.feature == 3)))
	{
		//int64_t id = row.id;
		//std::string a = row.a;
		std::string name = row.name;
		std::string feature = row.cheesecake;
	}
#endif

#if 0
	for (const auto& row : db(select(multi_column(all_of(p)).as(p), multi_column(f.name, f.id).as(f)).from(p,f).where(true)))
	{
		//int64_t id = row.id;
		//std::string a = row.a;
		std::string name = row.tabPerson.name;
		std::string name1 = row.tabFeature.name;
		//int64_t feature = row.feature;
	}
#endif

#if 0
	auto s = select(p.id, p.name, f.id.as(cheesecake))
				      .from(p, f)
						  .where(p.name == any(select(f.name)
										               .from(f)
																	 .where(true)))
							.group_by(f.name)
							.having(p.name.like("%Bee%"))
							.order_by(p.name.asc())
							.limit(3).offset(7);

	auto x = s.as(sqlpp::alias::x);
	for (const auto& row : db(select(p.id, p.name, all_of(x).as(x))
				.from(p.join(x).on(p.feature == x.cheesecake))
				.where(true)))
	{
		int64_t id = row.id;
		std::string name = row.name;
		std::string x_name = row.x.name;
		int cheesecake = row.x.cheesecake;
	}
#endif

#if !0
  auto dysel = dynamic_select(db).dynamic_columns(p.name).from(p).dynamic_where();

  if (some_condition)
    dysel.selected_columns.add(p.feature);

  if (some_other_condition)
    dysel.where.add(p.id > 17);

  for (const auto& row : db(dysel))
  {
    std::string name = row.name;
    std::string feature = row.at("feature");
  }
#endif

  return 0;
}

/*
 * Copyright (c) 2013, Roland Bock
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
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/functions.h>

#include <iostream>

DbMock db = {};
DbMock::_serializer_context_t printer(std::cerr);
SQLPP_ALIAS_PROVIDER(kaesekuchen);

int main()
{
	test::TabFoo f;
	test::TabBar t;

	serialize(insert_into(t).columns(t.beta, t.gamma), printer).flush();
	{
		auto i = insert_into(t).columns(t.gamma, t.beta);
		i.add_values(t.gamma = true, t.beta = "cheesecake");
		serialize(i, printer).flush();
		i.add_values(t.gamma = false, t.beta = sqlpp::tvin("coffee"));
		i.add_values(t.gamma = false, t.beta = sqlpp::tvin(std::string()));
		serialize(i, printer).flush();
		i.add_values(t.gamma = sqlpp::default_value, t.beta = sqlpp::null);
		serialize(i, printer).flush();
	}

	serialize(t.alpha = sqlpp::null, printer).flush();
	serialize(t.alpha = sqlpp::default_value, printer).flush();
	serialize(t.alpha, printer).flush();
	serialize(-t.alpha, printer).flush();
	serialize(+t.alpha, printer).flush();
	serialize(-(t.alpha + 7), printer).flush();
	serialize(t.alpha = 0, printer).flush();
	serialize(t.alpha = sqlpp::tvin(0), printer).flush();
	serialize(t.alpha == 0, printer).flush();
	serialize(t.alpha == sqlpp::tvin(0), printer).flush();
	serialize(t.alpha != 0, printer).flush();
	serialize(t.gamma != sqlpp::tvin(false), printer).flush();
	serialize(t.alpha == 7, printer).flush();
	serialize(t.beta + "kaesekuchen", printer).flush();

	serialize(sqlpp::select(), printer).flush();
	serialize(sqlpp::select().flags(sqlpp::distinct), printer).flush();
	serialize(select(t.alpha, t.beta).flags(sqlpp::distinct), printer).flush();
	serialize(select(t.alpha, t.beta), printer).flush();
	serialize(select(t.alpha, t.beta).from(t), printer).flush();
	serialize(select(t.alpha, t.beta).from(t).where(t.alpha == 3), printer).flush();
	serialize(select(t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma), printer).flush();
	serialize(select(t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma).having(t.beta.like("%kuchen")), printer).flush();
	serialize(select(t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma).having(t.beta.like("%kuchen")).order_by(t.beta.asc()), printer).flush();
	serialize(select(t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma).having(t.beta.like("%kuchen")).order_by(t.beta.asc()).limit(17).offset(3), printer).flush();

	serialize(parameter(sqlpp::bigint(), t.alpha), printer).flush();
	serialize(parameter(t.alpha), printer).flush();
	serialize(t.alpha == parameter(t.alpha), printer).flush();
	serialize(t.alpha == parameter(t.alpha) and (t.beta + "gimmick").like(parameter(t.beta)), printer).flush();

	serialize(insert_into(t), printer).flush();
	serialize(insert_into(f).default_values(), printer).flush();
	serialize(insert_into(t).set(t.gamma = true), printer).flush();
	//serialize(insert_into(t).set(t.gamma = sqlpp::tvin(false)), printer).flush(); cannot test this since gamma cannot be null and a static assert is thrown

	serialize(update(t), printer).flush();
	serialize(update(t).set(t.gamma = true), printer).flush();
	serialize(update(t).set(t.gamma = true).where(t.beta.in("kaesekuchen", "cheesecake")), printer).flush();

	serialize(remove_from(t), printer).flush();
	serialize(remove_from(t).using_(t), printer).flush();
	serialize(remove_from(t).where(t.alpha == sqlpp::tvin(0)), printer).flush();
	serialize(remove_from(t).using_(t).where(t.alpha == sqlpp::tvin(0)), printer).flush();

	// functions
	serialize(sqlpp::value(7), printer).flush();
	serialize(sqlpp::verbatim<sqlpp::detail::integral>("irgendwas integrales"), printer).flush();
	serialize(sqlpp::value_list(std::vector<int>({1,2,3,4,5,6,8})), printer).flush();
	serialize(exists(select(t.alpha).from(t)), printer).flush();
	serialize(any(select(t.alpha).from(t)), printer).flush();
	serialize(some(select(t.alpha).from(t)), printer).flush();
	serialize(count(t.alpha), printer).flush();
	serialize(min(t.alpha), printer).flush();
	serialize(max(t.alpha), printer).flush();
	serialize(avg(t.alpha), printer).flush();
	serialize(sum(t.alpha), printer).flush();
	serialize(sqlpp::verbatim_table("whatever"), printer).flush();

	// alias
	serialize(t.as(t.alpha), printer).flush();
	serialize(t.as(t.alpha).beta, printer).flush();

	// select alias
	serialize(select(t.alpha).from(t).where(t.beta > "kaesekuchen").as(t.gamma), printer).flush();

	serialize(t.alpha.is_null(), printer).flush();

	// join
	serialize(t.inner_join(t.as(t.alpha)).on(t.beta == t.as(t.alpha).beta), printer).flush();

	// multi_column
	serialize(multi_column(t.alpha, (t.beta + "cake").as(t.gamma)).as(t.alpha), printer).flush();
	serialize(multi_column(all_of(t)).as(t), printer).flush();
	serialize(all_of(t).as(t), printer).flush();

	// dynamic select
	{
		auto s = dynamic_select(db).dynamic_flags().dynamic_columns();
		s.add_column(t.beta);
		s.add_column(t.gamma);
		serialize(s, printer).flush();
	}
	{
		auto s = dynamic_select(db).dynamic_flags().dynamic_columns();
		s.add_flag(sqlpp::distinct);
		s.add_column(t.beta);
		s.add_column(t.gamma);
		serialize(s, printer).flush();
	}
	{
		auto s = dynamic_select(db).dynamic_flags(sqlpp::distinct).dynamic_columns(t.alpha);
		s.add_flag(sqlpp::all);
		s.add_column(t.beta);
		s.add_column(t.gamma);
		serialize(s, printer).flush();
	}

	// distinct aggregate
	serialize(count(sqlpp::distinct, t.alpha % 7), printer).flush();
	serialize(avg(sqlpp::distinct, t.alpha - 7), printer).flush();
	serialize(sum(sqlpp::distinct, t.alpha + 7), printer).flush();

	serialize(select(all_of(t)).from(t).where(true), printer).flush();
	serialize(select(all_of(t)).from(t).where(false), printer).flush();
	return 0;
}

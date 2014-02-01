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
DbMock::_context_t printer(std::cerr);
SQLPP_ALIAS_PROVIDER(kaesekuchen);

int main()
{
	test::TabFoo f;
	test::TabBar t;

	interpret(insert_into(t).columns(t.gamma, t.beta), printer).flush();
	interpret(insert_into(t).columns(t.gamma, t.beta).add_values(t.gamma = true, t.beta = "cheesecake"), printer).flush();
	interpret(insert_into(t).columns(t.gamma, t.beta)
			.add_values(t.gamma = true, t.beta = "cheesecake")
			.add_values(t.gamma = false, t.beta = sqlpp::tvin("coffee"))
			.add_values(t.gamma = false, t.beta = sqlpp::tvin(std::string()))
			, printer).flush();
	interpret(insert_into(t).columns(t.gamma, t.beta)
			.add_values(t.gamma = sqlpp::default_value, t.beta = sqlpp::null)
			, printer).flush();

	interpret(t.alpha = sqlpp::null, printer).flush();
	interpret(t.alpha = sqlpp::default_value, printer).flush();
	interpret(t.alpha, printer).flush();
	interpret(-t.alpha, printer).flush();
	interpret(+t.alpha, printer).flush();
	interpret(-(t.alpha + 7), printer).flush();
	interpret(t.alpha = 0, printer).flush();
	interpret(t.alpha = sqlpp::tvin(0), printer).flush();
	interpret(t.alpha == 0, printer).flush();
	interpret(t.alpha == sqlpp::tvin(0), printer).flush();
	interpret(t.alpha != 0, printer).flush();
	interpret(t.gamma != sqlpp::tvin(false), printer).flush();
	interpret(t.alpha == 7, printer).flush();
	interpret(t.beta + "kaesekuchen", printer).flush();

	interpret(select(sqlpp::distinct, t.alpha, t.beta), printer).flush();
	interpret(select(sqlpp::distinct, t.alpha, t.beta).from(t), printer).flush();
	interpret(select(sqlpp::distinct, t.alpha, t.beta).from(t).where(t.alpha == 3), printer).flush();
	interpret(select(sqlpp::distinct, t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma), printer).flush();
	interpret(select(sqlpp::distinct, t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma).having(t.beta.like("%kuchen")), printer).flush();
	interpret(select(sqlpp::distinct, t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma).having(t.beta.like("%kuchen")).order_by(t.beta.asc()), printer).flush();
	interpret(select(sqlpp::distinct, t.alpha, t.beta).from(t).where(t.alpha == 3).group_by(t.gamma).having(t.beta.like("%kuchen")).order_by(t.beta.asc()).limit(17).offset(3), printer).flush();

	interpret(parameter(sqlpp::bigint(), t.alpha), printer).flush();
	interpret(parameter(t.alpha), printer).flush();
	interpret(t.alpha == parameter(t.alpha), printer).flush();
	interpret(t.alpha == parameter(t.alpha) and (t.beta + "gimmick").like(parameter(t.beta)), printer).flush();

	interpret(insert_into(t), printer).flush();
	interpret(insert_into(f).default_values(), printer).flush();
	interpret(insert_into(t).set(t.gamma = true), printer).flush();
	//interpret(insert_into(t).set(t.gamma = sqlpp::tvin(false)), printer).flush(); cannot test this since gamma cannot be null and a static assert is thrown

	interpret(update(t), printer).flush();
	interpret(update(t).set(t.gamma = true), printer).flush();
	interpret(update(t).set(t.gamma = true).where(t.beta.in("kaesekuchen", "cheesecake")), printer).flush();

	interpret(remove_from(t), printer).flush();
	interpret(remove_from(t).where(t.alpha == sqlpp::tvin(0)), printer).flush();
	interpret(remove_from(t).using_(t).where(t.alpha == sqlpp::tvin(0)), printer).flush();

	// functions
	sqlpp::interpret(sqlpp::value(7), printer).flush(); // FIXME: Why is the namespace specifier required?
	interpret(sqlpp::verbatim<sqlpp::detail::integral>("irgendwas integrales"), printer).flush();
	interpret(sqlpp::value_list(std::vector<int>({1,2,3,4,5,6,8})), printer).flush();
	interpret(exists(select(t.alpha).from(t)), printer).flush();
	interpret(any(select(t.alpha).from(t)), printer).flush();
	interpret(some(select(t.alpha).from(t)), printer).flush();
	interpret(count(t.alpha), printer).flush();
	interpret(min(t.alpha), printer).flush();
	interpret(max(t.alpha), printer).flush();
	interpret(avg(t.alpha), printer).flush();
	interpret(sum(t.alpha), printer).flush();
	interpret(sqlpp::verbatim_table("whatever"), printer).flush();

	// alias
	interpret(t.as(t.alpha), printer).flush();
	interpret(t.as(t.alpha).beta, printer).flush();

	// select alias
	interpret(select(t.alpha).from(t).where(t.beta > "kaesekuchen").as(t.gamma), printer).flush();

	interpret(t.alpha.is_null(), printer).flush();

	// join
	interpret(t.inner_join(t.as(t.alpha)).on(t.beta == t.as(t.alpha).beta), printer).flush();

	// multi_column
	interpret(multi_column(t.alpha, t.alpha, (t.beta + "cake").as(t.gamma)), printer).flush();

	// dynamic select
	interpret(dynamic_select(db).dynamic_flags().dynamic_columns(t.alpha).add_column(t.beta).add_column(t.gamma), printer).flush();
	interpret(dynamic_select(db).dynamic_flags().add_flag(sqlpp::distinct).dynamic_columns().add_column(t.gamma).add_column(t.beta), printer).flush();
	interpret(dynamic_select(db).dynamic_flags(sqlpp::distinct).add_flag(sqlpp::all).dynamic_columns(t.alpha).add_column(t.beta), printer).flush();

	// distinct aggregate
	interpret(count(sqlpp::distinct, t.alpha % 7), printer).flush();
	interpret(avg(sqlpp::distinct, t.alpha - 7), printer).flush();
	interpret(sum(sqlpp::distinct, t.alpha + 7), printer).flush();

	interpret(select(all_of(t)).from(t).where(true), printer).flush();
	interpret(select(all_of(t)).from(t).where(false), printer).flush();

	return 0;
}

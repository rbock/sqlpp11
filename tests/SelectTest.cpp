/*
 * Copyright (c) 2013-2014, Roland Bock
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

#include <iostream>
#include "Sample.h"
#include "MockDb.h"
#include "is_regular.h"
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/select.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/connection.h>


MockDb db = {};
MockDb::_serializer_context_t printer;

namespace alias
{
	SQLPP_ALIAS_PROVIDER(a);
	SQLPP_ALIAS_PROVIDER(b);
	SQLPP_ALIAS_PROVIDER(left);
	SQLPP_ALIAS_PROVIDER(right);
}

int main()
{
	test::TabFoo f; 
	test::TabBar t;

	for (const auto& row : db(select(all_of(t)).from(t).where(true)))
	{
		int64_t a = row.alpha;
		const std::string b = row.beta;
	}

	for (const auto& row : db(select(all_of(t).as(t)).from(t).where(true)))
	{
		int64_t a = row.tabBar.alpha;
		const std::string b = row.tabBar.beta;
	}

	for (const auto& row : db(select(all_of(t).as(t), t.gamma).from(t).where(t.alpha > 7)))
	{
		int64_t a = row.tabBar.alpha;
		const std::string b = row.tabBar.beta;
		const bool g = row.gamma;
	}

	auto s = dynamic_select(db).dynamic_columns(all_of(t)).dynamic_flags().dynamic_from(t).extra_tables(f,t).dynamic_where().dynamic_group_by(t.alpha).dynamic_order_by().dynamic_having(t.gamma).dynamic_limit().dynamic_offset();
	s.add_flag(sqlpp::distinct);
	s.add_column(f.omega);
	s.add_from(f);
	s.add_where(t.alpha > 7);
	s.add_having(t.alpha > 7);
	s.set_limit(3);
	s.set_offset(3);
	s.add_group_by(t.beta);
	s.add_order_by(t.beta.asc());
	for (const auto& row : db(s))
	{
		int64_t a = row.alpha;
	}


	auto X = select(all_of(t)).from(t).as(t.alpha);

	return 0;
}

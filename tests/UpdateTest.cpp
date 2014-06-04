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
#include <sqlpp11/update.h>
#include "Sample.h"
#include "MockDb.h"
#include "is_regular.h"

MockDb db;
MockDb::_serializer_context_t printer;

int main()
{
	test::TabBar t;
	test::TabFoo f;

	auto x = t.alpha = 7;
	auto y = t.beta = "kaesekuchen";
	auto z = t.gamma = true;

	{
		using T = decltype(update(t));
		static_assert(sqlpp::is_regular<T>::value, "type requirement");
	}

	{
		using T = decltype(update(t).set(t.gamma = false).where(t.beta != "transparent"));
		static_assert(sqlpp::is_regular<T>::value, "type requirement");
	}

	{
		using T = decltype(dynamic_update(db, t).dynamic_set(t.gamma = false).dynamic_where());
		static_assert(sqlpp::is_regular<T>::value, "type requirement");
	}

	serialize(update(t), printer).str();
	serialize(update(t).set(t.gamma = false), printer).str();
	serialize(update(t).set(t.gamma = false).where(t.beta != "transparent"), printer).str();
	serialize(update(t).set(t.beta = "opaque").where(t.beta != t.beta), printer).str();
	auto u = dynamic_update(db, t).dynamic_set(t.gamma = false).dynamic_where();
	u.assignments.add(t.gamma = false);
	u.where.add(t.gamma != false);
	serialize(u, printer).str();

	db(u);

	return 0;
}

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
#include "is_regular.h"
#include <sqlpp11/insert.h>
#include <iostream>

DbMock db;
DbMock::_context_t printer(std::cerr);

int main()
{
	test::TabBar t;

	auto x = t.alpha = 7;
	auto y = t.beta = "kaesekuchen";
	auto z = t.gamma = true;
	auto a = t.alpha;
	a = t.alpha;

	{
		using T = decltype(insert_into(t));
		static_assert(sqlpp::is_regular<T>::value, "type requirement");
	}

	{
		using T = decltype(insert_into(t).set(t.beta = "kirschauflauf"));
		static_assert(sqlpp::is_regular<T>::value, "type requirement");
	}

	{
		using T = decltype(dynamic_insert_into(db, t).dynamic_set());
		static_assert(sqlpp::is_regular<T>::value, "type requirement");
	}

	interpret(insert_into(t), printer).flush();
	interpret(insert_into(t).set(t.beta = "kirschauflauf"), printer).flush();
	auto i = dynamic_insert_into(db, t).dynamic_set();
	i = i.add_set(t.beta = "kirschauflauf");
	interpret(i, printer).flush();

	return 0;
}

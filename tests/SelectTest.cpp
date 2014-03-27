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

#warning this should fail because f is not in from()
	for (const auto& row : db(select(f.omega, all_of(t).as(t), t.gamma).from(t).where(true)))
	{
		int64_t a = row.tabBar.alpha;
		const std::string b = row.tabBar.beta;
		const bool g = row.gamma;
		const float o = row.omega;
	}

	return 0;
}

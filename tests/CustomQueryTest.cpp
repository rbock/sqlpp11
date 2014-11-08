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
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/custom_query.h>

MockDb db = {};
MockDb::_serializer_context_t printer;

int main()
{
	test::TabFoo f; 
	test::TabBar t;

	auto c = custom_query(select(all_of(t)).from(t), sqlpp::verbatim("INTO"), f);
	std::cerr << serialize(c, printer).str() << std::endl;
	db(c);

	auto p = db.prepare(custom_query(select(all_of(t)).from(t), where(t.alpha > sqlpp::parameter(t.alpha))));
	p.params.alpha = 8;

	printer.reset();
	auto x = custom_query(sqlpp::verbatim("PRAGMA writeable_schema = "), true);
	std::cerr << serialize(x, printer).str() << std::endl;
	db(x);

	db.run_prepared_execute(db.prepare(x));

	return 0;
}

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

#include <iostream>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include "TabSample.h"

#include <iostream>
class DbMock
{
public:
	const std::string& escape(const std::string& text) { return text; }
};

DbMock db;

int main()
{
	TabSample t;

	auto x = t.alpha = 7;
	auto y = t.beta = "kaesekuchen";
	auto z = t.gamma = true;

	remove_from(t).serialize(std::cerr, db); std::cerr << "\n";
	remove_from(t).where(t.beta != "transparent").serialize(std::cerr, db); std::cerr << "\n";
	remove_from(t).using_(t).serialize(std::cerr, db); std::cerr << "\n";
	auto r = dynamic_remove_from(db, t).dynamic_using_().dynamic_where();
	r.add_using_(t);
	r.add_where(t.beta != "transparent");
	r.serialize(std::cerr, db); std::cerr << "\n";
	//insert_into(t).values(7, "wurstwaren", true).serialize(std::cerr, db); std::cerr << "\n";
	//insert_into(t).columns(t.alpha, t.beta).values(25, "drei").serialize(std::cerr, db); std::cerr << "\n";
	//insert_into(t).columns(t.alpha, t.beta).select(select(t.alpha, t.beta).from(t)).serialize(std::cerr, db);
	return 0;
}

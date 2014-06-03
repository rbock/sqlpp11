/*
 * Copyright (c) 2013-2014, Roland Bock
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

#ifndef SQLPP_REMOVE_H
#define SQLPP_REMOVE_H

#include <sqlpp11/statement.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_remove.h>
#include <sqlpp11/vendor/noop.h>
#warning: need to use another table provider, since delete can be used with several tables
#include <sqlpp11/vendor/single_table.h>
#include <sqlpp11/vendor/extra_tables.h>
#include <sqlpp11/vendor/using.h>
#include <sqlpp11/vendor/where.h>

namespace sqlpp
{
	template<typename Database>
		using blank_remove_t = statement_t<Database,
#warning Make this another policy that can take several table (cannot be select's from, since it has to be a result provider)
			vendor::no_single_table_t,
			vendor::no_using_t,
			vendor::no_extra_tables_t,
			vendor::no_where_t>;

	auto remove()
		-> blank_remove_t<void>
		{
			return { blank_remove_t<void>() };
		}

	template<typename Table>
		auto remove_from(Table table)
		-> decltype(blank_remove_t<void>().from(table))
		{
			return { blank_remove_t<void>().from(table) };
		}

	template<typename Database>
		auto  dynamic_remove(const Database&)
		-> decltype(blank_remove_t<Database>())
		{
			return { blank_remove_t<Database>() };
		}

	template<typename Database, typename Table>
		auto  dynamic_remove_from(const Database&, Table table)
		-> decltype(blank_remove_t<Database>().from(table))
		{
			return { blank_remove_t<Database>().from(table) };
		}
}

#endif

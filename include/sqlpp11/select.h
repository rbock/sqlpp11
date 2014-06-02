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

#ifndef SQLPP_SELECT_H
#define SQLPP_SELECT_H

#include <sqlpp11/statement.h>

#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/select_flag_list.h>
#include <sqlpp11/vendor/select_column_list.h>
#include <sqlpp11/vendor/from.h>
#include <sqlpp11/vendor/extra_tables.h>
#include <sqlpp11/vendor/where.h>
#include <sqlpp11/vendor/group_by.h>
#include <sqlpp11/vendor/having.h>
#include <sqlpp11/vendor/order_by.h>
#include <sqlpp11/vendor/limit.h>
#include <sqlpp11/vendor/offset.h>
#include <sqlpp11/vendor/expression.h>
#include <sqlpp11/vendor/wrong.h>


namespace sqlpp
{
	template<typename Database>
		using blank_select_t = statement_t<Database,
			vendor::no_select_flag_list_t, 
			vendor::no_select_column_list_t, 
			vendor::no_from_t,
			vendor::no_extra_tables_t,
			vendor::no_where_t, 
			vendor::no_group_by_t, 
			vendor::no_having_t,
			vendor::no_order_by_t, 
			vendor::no_limit_t, 
			vendor::no_offset_t>;


	blank_select_t<void> select() // FIXME: These should be constexpr
	{
		return { };
	}

	template<typename... Columns>
		auto select(Columns... columns)
		-> decltype(blank_select_t<void>().columns(columns...))
		{
			return blank_select_t<void>().columns(columns...);
		}

	template<typename Database>
		blank_select_t<Database> dynamic_select(const Database&)
		{
			return { };
		}

	template<typename Database, typename... Columns>
		auto dynamic_select(const Database&, Columns... columns)
		-> decltype(blank_select_t<Database>().columns(columns...))
		{
			return blank_select_t<Database>().columns(columns...);
		}

}
#endif

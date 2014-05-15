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

#ifndef SQLPP_VENDOR_SINGLE_TABLE_H
#define SQLPP_VENDOR_SINGLE_TABLE_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
	namespace vendor
	{
		// A SINGLE TABLE
		template<typename Database, typename Table>
			struct single_table_t
			{
				using _is_single_table = std::true_type;
				using _required_tables = typename Table::_required_tables;
				using _provided_tables = typename Table::_provided_tables;

				static_assert(is_table_t<Table>::value, "argument has to be a table");
				static_assert(_required_tables::size::value == 0, "table depends on another table");

				single_table_t(Table table):
					_table(table)
				{}

				single_table_t(const single_table_t&) = default;
				single_table_t(single_table_t&&) = default;
				single_table_t& operator=(const single_table_t&) = default;
				single_table_t& operator=(single_table_t&&) = default;
				~single_table_t() = default;

				Table _table;
			};

		struct no_single_table_t
		{
			using _provided_tables = detail::type_set<>;
			using _required_tables = ::sqlpp::detail::type_set<>;
		};

		// Interpreters
		template<typename Context, typename Database, typename Table>
			struct serializer_t<Context, single_table_t<Database, Table>>
			{
				using T = single_table_t<Database, Table>;

				static Context& _(const T& t, Context& context)
				{
					serialize(t._table, context);
					return context;
				}
			};

	}
}

#endif

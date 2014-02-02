/*
 * Copyright (c) 2013, Roland Bock
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

#ifndef SQLPP_TABLE_ALIAS_H
#define SQLPP_TABLE_ALIAS_H

#include <sqlpp11/column_fwd.h>
#include <sqlpp11/interpret.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/alias.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
	struct table_alias_base_t {};

	template<typename AliasProvider, typename Table, typename... ColumnSpec>
		struct table_alias_t: public table_alias_base_t, public ColumnSpec::_name_t::template _member_t<column_t<AliasProvider, ColumnSpec>>...
	{
		//FIXME: Need to add join functionality
		using _is_table = std::true_type;
		using _table_set = detail::type_set<table_alias_t>;

		struct _value_type: Table::_value_type
		{
			using _is_expression = std::false_type;
			using _is_named_expression = copy_type_trait<Table, is_value_t>;
			using _is_alias = std::true_type;
		};

		using _name_t = typename AliasProvider::_name_t;
		using _all_of_t = std::tuple<column_t<AliasProvider, ColumnSpec>...>;

		table_alias_t(Table table):
			_table(table)
		{}

		Table _table;
	};

	namespace vendor
	{
		template<typename Context, typename X>
			struct interpreter_t<Context, X, typename std::enable_if<std::is_base_of<table_alias_base_t, X>::value, void>::type>
			{
				using T = X;

				static Context& _(const T& t, Context& context)
				{
					context << "(";
					interpret(t._table, context);
					context << ") AS " << T::_name_t::_get_name();
					return context;
				}
			};

	}
}

#endif


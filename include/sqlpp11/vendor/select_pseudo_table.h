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

#ifndef SQLPP_SELECT_PSEUDO_TABLE_H
#define SQLPP_SELECT_PSEUDO_TABLE_H

#include <sqlpp11/no_value.h>

namespace sqlpp
{
	// provide type information for sub-selects that are used as named expressions or tables
	template<typename Expr>
		struct select_column_spec_t
		{
			using _name_t = typename Expr::_name_t;
			using _value_type = typename Expr::_value_type;
			struct _column_type {};
		};

	template<
						 typename Select,
						 typename... NamedExpr
							 >
							 struct select_pseudo_table_t: public sqlpp::table_t<select_pseudo_table_t<
																						 Select,
																						 NamedExpr...>, select_column_spec_t<NamedExpr>...>
	{
		using _value_type = no_value_t;
		using _is_pseudo_table = std::true_type;

		select_pseudo_table_t(Select select):
			_select(select)
		{
		}

		select_pseudo_table_t(const select_pseudo_table_t& rhs) = default;
		select_pseudo_table_t(select_pseudo_table_t&& rhs) = default;
		select_pseudo_table_t& operator=(const select_pseudo_table_t& rhs) = default;
		select_pseudo_table_t& operator=(select_pseudo_table_t&& rhs) = default;
		~select_pseudo_table_t() = default;

		Select _select;
	};

	namespace vendor
	{
		template<typename Context, typename Select, typename... NamedExpr>
			struct interpreter_t<Context, select_pseudo_table_t<Select, NamedExpr...>>
			{
				using T = select_pseudo_table_t<Select, NamedExpr...>;

				static Context& _(const T& t, Context& context)
				{
					interpret(t._select, context);
					return context;
				}
			};

	}
}

#endif

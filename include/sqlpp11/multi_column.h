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

#ifndef SQLPP_MULTI_COLUMN_H
#define SQLPP_MULTI_COLUMN_H

#include <sqlpp11/no_value.h>
#include <sqlpp11/detail/make_expression_tuple.h>
#include <sqlpp11/detail/logic.h>

namespace sqlpp
{
	template<typename AliasProvider, typename T>
		struct multi_column_t
		{
			static_assert(vendor::wrong_t<T>::value, "invalid argument for multicolumn_t");
		};

	template<typename AliasProvider, typename... NamedExpr>
		struct multi_column_t<AliasProvider, std::tuple<NamedExpr...>>
		{
			static_assert(detail::and_t<is_named_expression_t, NamedExpr...>::value, "multi_column parameters need to be named expressions");

			using _name_t = typename AliasProvider::_name_t;

			struct _value_type: public no_value_t
			{
				using _is_named_expression = std::true_type;
			};
			using _is_multi_column = std::true_type;

			std::tuple<NamedExpr...> _columns;
		};

	namespace vendor
	{
		template<typename Context, typename AliasProvider, typename... NamedExpr>
			struct interpreter_t<Context, multi_column_t<AliasProvider, NamedExpr...>>
			{
				using T = multi_column_t<AliasProvider, NamedExpr...>;

				static Context& _(const T& t, Context& context)
				{
					interpret_tuple(t._columns, ',', context);
					return context;
				}
			};
	}

	namespace detail
	{
		template<typename AliasProvider, typename... Expr>
			using make_multi_column_t = 
			multi_column_t<AliasProvider, decltype(make_expression_tuple(std::declval<Expr>()...))>;
	}

	template<typename AliasProvider, typename... NamedExpr>
		detail::make_multi_column_t<AliasProvider, NamedExpr...> multi_column(const AliasProvider& aliasProvider, NamedExpr... namedExpr) 
		{
			return { detail::make_expression_tuple(namedExpr...)};
		}

}

#endif

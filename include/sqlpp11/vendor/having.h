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

#ifndef SQLPP_HAVING_H
#define SQLPP_HAVING_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/expression.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/interpretable_list.h>
#include <sqlpp11/detail/logic.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename Database, typename... Expr>
			struct having_t
			{
				using _is_having = std::true_type;
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
				using _parameter_tuple_t = std::tuple<Expr...>;

				static_assert(_is_dynamic::value or sizeof...(Expr), "at least one expression argument required in having()");
				static_assert(::sqlpp::detail::and_t<is_expression_t, Expr...>::value, "at least one argument is not an expression in having()");

				using _parameter_list_t = typename make_parameter_list_t<_parameter_tuple_t>::type;

				template<typename E>
					void add(E expr)
					{
						static_assert(is_expression_t<E>::value, "invalid expression argument in add_having()");
						_dynamic_expressions.emplace_back(expr);
					}

				_parameter_tuple_t _expressions;
				vendor::interpretable_list_t<Database> _dynamic_expressions;
			};

		template<typename Context, typename Database, typename... Expr>
			struct interpreter_t<Context, having_t<Database, Expr...>>
			{
				using T = having_t<Database, Expr...>;

				static Context& _(const T& t, Context& context)
				{
					if (sizeof...(Expr) == 0 and t._dynamic_expressions.empty())
						return context;
					context << " HAVING ";
					interpret_tuple(t._expressions, " AND ", context);
					if (sizeof...(Expr) and not t._dynamic_expressions.empty())
						context << " AND ";
					interpret_list(t._dynamic_expressions, " AND ", context);
					return context;
				}
			};
	}
}

#endif

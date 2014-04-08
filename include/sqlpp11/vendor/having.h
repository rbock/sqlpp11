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

#ifndef SQLPP_HAVING_H
#define SQLPP_HAVING_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/expression.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/interpretable_list.h>
#include <sqlpp11/vendor/policy_update.h>
#include <sqlpp11/detail/logic.h>

namespace sqlpp
{
	namespace vendor
	{
		// HAVING
		template<typename Database, typename... Expressions>
			struct having_t
			{
				using _is_having = std::true_type;
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
				using _parameter_tuple_t = std::tuple<Expressions...>;

				static_assert(_is_dynamic::value or sizeof...(Expressions), "at least one expression argument required in having()");
				static_assert(::sqlpp::detail::all_t<is_expression_t, Expressions...>::value, "at least one argument is not an expression in having()");

				using _parameter_list_t = typename make_parameter_list_t<_parameter_tuple_t>::type;

				using _table_set = typename ::sqlpp::detail::make_joined_set<typename Expressions::_table_set...>::type;

				having_t(Expressions... expressions):
					_expressions(expressions...)
				{}

				having_t(const having_t&) = default;
				having_t(having_t&&) = default;
				having_t& operator=(const having_t&) = default;
				having_t& operator=(having_t&&) = default;
				~having_t() = default;

				template<typename Policies>
					struct _methods_t
					{
						template<typename Expression>
							void add_having(Expression expression)
							{
								static_assert(_is_dynamic::value, "add_having must not be called for static having");
								static_assert(is_expression_t<Expression>::value, "invalid expression argument in add_having()");
#warning: Need to dispatch to actual add method to prevent error messages from being generated
								return static_cast<typename Policies::_select_t*>(this)->_having._dynamic_expressions.emplace_back(expression);
							}
					};

				_parameter_tuple_t _expressions;
				vendor::interpretable_list_t<Database> _dynamic_expressions;
			};

		struct no_having_t
		{
			using _is_noop = std::true_type;
			using _table_set = ::sqlpp::detail::type_set<>;

			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
					using _new_select_t = typename Policies::template _policies_update_t<no_having_t, T>;

					template<typename... Args>
						auto having(Args... args)
						-> _new_select_t<having_t<void, Args...>>
						{
							return { *static_cast<typename Policies::_select_t*>(this), having_t<void, Args...>{args...} };
						}

					template<typename... Args>
						auto dynamic_having(Args... args)
						-> _new_select_t<having_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_having must not be called in a static statement");
							return { *static_cast<typename Policies::_select_t*>(this), vendor::having_t<_database_t, Args...>{args...} };
						}
				};
		};

		// Interpreters
		template<typename Context, typename Database, typename... Expressions>
			struct serializer_t<Context, having_t<Database, Expressions...>>
			{
				using T = having_t<Database, Expressions...>;

				static Context& _(const T& t, Context& context)
				{
					if (sizeof...(Expressions) == 0 and t._dynamic_expressions.empty())
						return context;
					context << " HAVING ";
					interpret_tuple(t._expressions, " AND ", context);
					if (sizeof...(Expressions) and not t._dynamic_expressions.empty())
						context << " AND ";
					interpret_list(t._dynamic_expressions, " AND ", context);
					return context;
				}
			};

		template<typename Context>
			struct serializer_t<Context, no_having_t>
			{
				using T = no_having_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

	}
}

#endif

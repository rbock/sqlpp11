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

#ifndef SQLPP_WHERE_H
#define SQLPP_WHERE_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/vendor/expression.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/interpretable_list.h>
#include <sqlpp11/detail/logic.h>

namespace sqlpp
{
	namespace vendor
	{
		// WHERE
		template<typename Database, typename... Expressions>
			struct where_t
			{
				using _is_where = std::true_type;
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
				using _parameter_tuple_t = std::tuple<Expressions...>;

				static_assert(_is_dynamic::value or sizeof...(Expressions), "at least one expression argument required in where()");
				static_assert(not sqlpp::detail::any_t<is_assignment_t, Expressions...>::value, "at least one argument is an assignment in where()");
				static_assert(sqlpp::detail::all_t<is_expression_t, Expressions...>::value, "at least one argument is not valid expression in where()");

				using _parameter_list_t = typename make_parameter_list_t<_parameter_tuple_t>::type;

				using _table_set = typename ::sqlpp::detail::make_joined_set<typename Expressions::_table_set...>::type;

				where_t(Expressions... expressions):
					_expressions(expressions...)
				{}

				where_t(const where_t&) = default;
				where_t(where_t&&) = default;
				where_t& operator=(const where_t&) = default;
				where_t& operator=(where_t&&) = default;
				~where_t() = default;

				template<typename Policies>
					struct _methods_t
					{
						template<typename Expression>
							void add_where(Expression expression)
							{
								static_assert(_is_dynamic::value, "add_where can only be called for dynamic_where");
								static_assert(is_expression_t<Expression>::value, "invalid expression argument in add_where()");
#warning: Need to dispatch to actual add method to prevent error messages from being generated
								return static_cast<typename Policies::_statement_t*>(this)->_where._dynamic_expressions.emplace_back(expression);
							}
					};

				_parameter_tuple_t _expressions;
				vendor::interpretable_list_t<Database> _dynamic_expressions;
			};

		template<>
			struct where_t<void, bool>
			{
				using _is_where = std::true_type;
				using _is_dynamic = std::false_type;
				using _table_set = ::sqlpp::detail::type_set<>;

				where_t(bool condition):
					_condition(condition)
				{}

				where_t(const where_t&) = default;
				where_t(where_t&&) = default;
				where_t& operator=(const where_t&) = default;
				where_t& operator=(where_t&&) = default;
				~where_t() = default;

				template<typename Policies>
					struct _methods_t
					{
					};

				bool _condition;
			};

		struct no_where_t
		{
			using _is_noop = std::true_type;
			using _table_set = ::sqlpp::detail::type_set<>;

			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
					using _new_select_t = typename Policies::template _policies_update_t<no_where_t, T>;

					template<typename... Args>
						auto where(Args... args)
						-> _new_select_t<where_t<void, Args...>>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), where_t<void, Args...>{args...} };
						}

					template<typename... Args>
						auto dynamic_where(Args... args)
						-> _new_select_t<where_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_where must not be called in a static statement");
							return { *static_cast<typename Policies::_statement_t*>(this), vendor::where_t<_database_t, Args...>{args...} };
						}
				};
		};

		// Interpreters
		template<typename Context, typename Database, typename... Expressions>
			struct serializer_t<Context, where_t<Database, Expressions...>>
			{
				using T = where_t<Database, Expressions...>;

				static Context& _(const T& t, Context& context)
				{
					if (sizeof...(Expressions) == 0 and t._dynamic_expressions.empty())
						return context;
					context << " WHERE ";
					interpret_tuple(t._expressions, " AND ", context);
					if (sizeof...(Expressions) and not t._dynamic_expressions.empty())
						context << " AND ";
					interpret_list(t._dynamic_expressions, " AND ", context);
					return context;
				}
			};

		template<typename Context>
			struct serializer_t<Context, where_t<void, bool>>
			{
				using T = where_t<void, bool>;

				static Context& _(const T& t, Context& context)
				{
					if (not t._condition)
						context << " WHERE NULL";
					return context;
				}
			};

		template<typename Context>
			struct serializer_t<Context, no_where_t>
			{
				using T = no_where_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

	}
}

#endif

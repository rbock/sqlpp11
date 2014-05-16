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

#ifndef SQLPP_ORDER_BY_H
#define SQLPP_ORDER_BY_H

#include <tuple>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/interpretable.h>
#include <sqlpp11/vendor/policy_update.h>
#include <sqlpp11/detail/logic.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename Database,typename... Expressions>
			struct order_by_t
			{
				using _traits = make_traits<no_value_t, ::sqlpp::tag::group_by>;
				using _recursive_traits = make_recursive_traits<Expressions...>;

				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

				static_assert(_is_dynamic::value or sizeof...(Expressions), "at least one sort-order expression required in order_by()");

				static_assert(not ::sqlpp::detail::has_duplicates<Expressions...>::value, "at least one duplicate argument detected in order_by()");

				static_assert(::sqlpp::detail::all_t<is_sort_order_t<Expressions>::value...>::value, "at least one argument is not a sort order expression in order_by()");

				order_by_t(Expressions... expressions):
					_expressions(expressions...)
				{}

				order_by_t(const order_by_t&) = default;
				order_by_t(order_by_t&&) = default;
				order_by_t& operator=(const order_by_t&) = default;
				order_by_t& operator=(order_by_t&&) = default;
				~order_by_t() = default;

				template<typename Policies>
					struct _methods_t
					{
						template<typename Expression>
							void add_order_by_ntc(Expression expression)
							{
								add_order_by<Expression, std::false_type>(expression);
							}

						template<typename Expression, typename TableCheckRequired = std::true_type>
							void add_order_by(Expression expression)
							{
								static_assert(_is_dynamic::value, "add_order_by must not be called for static order_by");
								static_assert(is_sort_order_t<Expression>::value, "invalid expression argument in add_order_by()");
								static_assert(TableCheckRequired::value or Policies::template _no_unknown_tables<Expression>::value, "expression uses tables unknown to this statement in add_order_by()");

								using ok = ::sqlpp::detail::all_t<_is_dynamic::value, is_sort_order_t<Expression>::value>;

								_add_order_by_impl(expression, ok()); // dispatch to prevent compile messages after the static_assert
							}

					private:
						template<typename Expression>
							void _add_order_by_impl(Expression expression, const std::true_type&)
							{
								return static_cast<typename Policies::_statement_t*>(this)->_order_by._dynamic_expressions.emplace_back(expression);
							}

						template<typename Expression>
							void _add_order_by_impl(Expression expression, const std::false_type&);
					};

				std::tuple<Expressions...> _expressions;
				vendor::interpretable_list_t<Database> _dynamic_expressions;
			};

		struct no_order_by_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::noop>;
			using _recursive_traits = make_recursive_traits<>;

			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
					using _new_statement_t = typename Policies::template _new_statement_t<no_order_by_t, T>;

					template<typename... Args>
						auto order_by(Args... args)
						-> _new_statement_t<order_by_t<void, Args...>>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), order_by_t<void, Args...>{args...} };
						}

					template<typename... Args>
						auto dynamic_order_by(Args... args)
						-> _new_statement_t<order_by_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_order_by must not be called in a static statement");
							return { *static_cast<typename Policies::_statement_t*>(this), vendor::order_by_t<_database_t, Args...>{args...} };
						}
				};
		};

		// Interpreters
		template<typename Context, typename Database, typename... Expressions>
			struct serializer_t<Context, order_by_t<Database, Expressions...>>
			{
				using T = order_by_t<Database, Expressions...>;

				static Context& _(const T& t, Context& context)
				{
					if (sizeof...(Expressions) == 0 and t._dynamic_expressions.empty())
						return context;
					context << " ORDER BY ";
					interpret_tuple(t._expressions, ',', context);
					if (sizeof...(Expressions) and not t._dynamic_expressions.empty())
						context << ',';
					interpret_list(t._dynamic_expressions, ',', context);
					return context;
				}
			};

		template<typename Context>
			struct serializer_t<Context, no_order_by_t>
			{
				using T = no_order_by_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

	}
}

#endif

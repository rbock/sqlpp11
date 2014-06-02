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

#ifndef SQLPP_GROUP_BY_H
#define SQLPP_GROUP_BY_H

#include <tuple>
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
		// GROUP BY DATA
		template<typename Database, typename... Expressions>
			struct group_by_data_t
			{
				group_by_data_t(Expressions... expressions):
					_expressions(expressions...)
				{}

				group_by_data_t(const group_by_data_t&) = default;
				group_by_data_t(group_by_data_t&&) = default;
				group_by_data_t& operator=(const group_by_data_t&) = default;
				group_by_data_t& operator=(group_by_data_t&&) = default;
				~group_by_data_t() = default;

				std::tuple<Expressions...> _expressions;
				vendor::interpretable_list_t<Database> _dynamic_expressions;
			};

		// GROUP BY
		template<typename Database, typename... Expressions>
			struct group_by_t
			{
				using _traits = make_traits<no_value_t, ::sqlpp::tag::group_by>;
				using _recursive_traits = make_recursive_traits<Expressions...>;

				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

				static_assert(_is_dynamic::value or sizeof...(Expressions), "at least one expression (e.g. a column) required in group_by()");

				static_assert(not ::sqlpp::detail::has_duplicates<Expressions...>::value, "at least one duplicate argument detected in group_by()");

				static_assert(::sqlpp::detail::all_t<is_expression_t<Expressions>::value...>::value, "at least one argument is not an expression in group_by()");

				// Data
				using _data_t = group_by_data_t<Database, Expressions...>;

				// Member implementation with data and methods
				template<typename Policies>
					struct _impl_t
					{
						template<typename Expression>
							void add_ntc(Expression expression)
							{
								add<Expression, std::false_type>(expression);
							}

						template<typename Expression, typename TableCheckRequired = std::true_type>
							void add(Expression expression)
							{
								static_assert(_is_dynamic::value, "add() must not be called for static group_by");
								static_assert(is_expression_t<Expression>::value, "invalid expression argument in group_by::add()");
								static_assert(TableCheckRequired::value or Policies::template _no_unknown_tables<Expression>::value, "expression uses tables unknown to this statement in group_by::add()");

								using ok = ::sqlpp::detail::all_t<_is_dynamic::value, is_expression_t<Expression>::value>;

								_add_impl(expression, ok()); // dispatch to prevent compile messages after the static_assert
							}

					private:
						template<typename Expression>
							void _add_impl(Expression expression, const std::true_type&)
							{
								return _data._dynamic_expressions.emplace_back(expression);
							}

						template<typename Expression>
							void _add_impl(Expression expression, const std::false_type&);
					public:
						_data_t _data;
					};

				// Member template for adding the named member to a statement
				template<typename Policies>
					struct _member_t
					{
						using _data_t = group_by_data_t<Database, Expressions...>;

						_impl_t<Policies> group_by;
						_impl_t<Policies>& operator()() { return group_by; }
						const _impl_t<Policies>& operator()() const { return group_by; }

						template<typename T>
							static auto _get_member(T t) -> decltype(t.group_by)
							{
								return t.group_by;
							}
					};

				template<typename Policies>
					struct _methods_t
					{
					};
			};

		// NO GROUP BY YET
		struct no_group_by_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::noop>;
			using _recursive_traits = make_recursive_traits<>;

			// Data
			using _data_t = no_data_t;

			// Member implementation with data and methods
			template<typename Policies>
				struct _impl_t
				{
					_data_t _data;
				};

			// Member template for adding the named member to a statement
			template<typename Policies>
				struct _member_t
				{
					using _data_t = no_data_t;

					_impl_t<Policies> no_group_by;
					_impl_t<Policies>& operator()() { return no_group_by; }
					const _impl_t<Policies>& operator()() const { return no_group_by; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.no_group_by)
						{
							return t.no_group_by;
						}
				};

			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
					using _new_statement_t = typename Policies::template _new_statement_t<no_group_by_t, T>;

					template<typename... Args>
						auto group_by(Args... args)
						-> _new_statement_t<group_by_t<void, Args...>>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), group_by_data_t<void, Args...>{args...} };
						}

					template<typename... Args>
						auto dynamic_group_by(Args... args)
						-> _new_statement_t<group_by_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_group_by must not be called in a static statement");
							return { *static_cast<typename Policies::_statement_t*>(this), group_by_data_t<_database_t, Args...>{args...} };
						}
				};
		};

		// Interpreters
		template<typename Context, typename Database, typename... Expressions>
			struct serializer_t<Context, group_by_data_t<Database, Expressions...>>
			{
				using T = group_by_data_t<Database, Expressions...>;

				static Context& _(const T& t, Context& context)
				{
					if (sizeof...(Expressions) == 0 and t._dynamic_expressions.empty())
						return context;
					context << " GROUP BY ";
					interpret_tuple(t._expressions, ',', context);
					if (sizeof...(Expressions) and not t._dynamic_expressions.empty())
						context << ',';
					interpret_list(t._dynamic_expressions, ',', context);
					return context;
				}
			};
	}
}

#endif

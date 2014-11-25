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

#include <sqlpp11/statement_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/expression.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/detail/logic.h>

namespace sqlpp
{
	// WHERE DATA
	template<typename Database, typename... Expressions>
		struct where_data_t
		{
			where_data_t(Expressions... expressions):
				_expressions(expressions...)
			{}

			where_data_t(const where_data_t&) = default;
			where_data_t(where_data_t&&) = default;
			where_data_t& operator=(const where_data_t&) = default;
			where_data_t& operator=(where_data_t&&) = default;
			~where_data_t() = default;

			std::tuple<Expressions...> _expressions;
			interpretable_list_t<Database> _dynamic_expressions;
		};

	// WHERE(EXPR)
	template<typename Database, typename... Expressions>
		struct where_t
		{
			using _traits = make_traits<no_value_t, tag::is_where>;
			using _recursive_traits = make_recursive_traits<Expressions...>;

			using _is_dynamic = is_database<Database>;

			static_assert(_is_dynamic::value or sizeof...(Expressions), "at least one expression argument required in where()");
			static_assert(detail::none_t<is_assignment_t<Expressions>::value...>::value, "at least one argument is an assignment in where()");
			static_assert(detail::all_t<is_expression_t<Expressions>::value...>::value, "at least one argument is not valid expression in where()");

			// Data
			using _data_t = where_data_t<Database, Expressions...>;

			// Member implementation with data and methods
			template <typename Policies>
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
							static_assert(_is_dynamic::value, "where::add() can only be called for dynamic_where");
							static_assert(is_expression_t<Expression>::value, "invalid expression argument in where::add()");
							static_assert(not TableCheckRequired::value or Policies::template _no_unknown_tables<Expression>::value, "expression uses tables unknown to this statement in where::add()");

							using ok = detail::all_t<_is_dynamic::value, is_expression_t<Expression>::value>;

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
					using _data_t = where_data_t<Database, Expressions...>;

					_impl_t<Policies> where;
					_impl_t<Policies>& operator()() { return where; }
					const _impl_t<Policies>& operator()() const { return where; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.where)
						{
							return t.where;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
#warning: here and elsewhere: add check for missing tables (see select columns, for instance)
					using _consistency_check = consistent_t;
				};
		};

	template<>
		struct where_data_t<void, bool>
		{
			bool _condition;
		};

	// WHERE(BOOL)
	template<>
		struct where_t<void, bool>
		{
			using _traits = make_traits<no_value_t, tag::is_where>;
			using _recursive_traits = make_recursive_traits<>;

			// Data
			using _data_t = where_data_t<void, bool>;

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
					using _data_t = where_data_t<void, bool>;

					_impl_t<Policies> where;
					_impl_t<Policies>& operator()() { return where; }
					const _impl_t<Policies>& operator()() const { return where; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.where)
						{
							return t.where;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
					using _consistency_check = consistent_t;
				};

		};

	struct assert_where_t
	{
		using type = std::false_type;

		template<typename T = void>
			static void _()
			{
				static_assert(wrong_t<T>::value, "where expression required, e.g. where(true)");
			};
	};

	// NO WHERE YET
	template<bool WhereRequired>
		struct no_where_t
		{
			using _traits = make_traits<no_value_t, tag::is_where>;
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

					_impl_t<Policies> no_where;
					_impl_t<Policies>& operator()() { return no_where; }
					const _impl_t<Policies>& operator()() const { return no_where; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.no_where)
						{
							return t.no_where;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
						using _new_statement_t = new_statement<Policies, no_where_t, T>;

					using _consistency_check = typename std::conditional<
						WhereRequired and (Policies::_all_provided_tables::size::value > 0),
						assert_where_t,
						consistent_t>::type;

					template<typename... Args>
						auto where(Args... args) const
						-> _new_statement_t<where_t<void, Args...>>
						{
							return { static_cast<const derived_statement_t<Policies>&>(*this), where_data_t<void, Args...>{args...} };
						}

					template<typename... Args>
						auto dynamic_where(Args... args) const
						-> _new_statement_t<where_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_where must not be called in a static statement");
							return { static_cast<const derived_statement_t<Policies>&>(*this), where_data_t<_database_t, Args...>{args...} };
						}
				};
		};

	// Interpreters
	template<typename Context, typename Database, typename... Expressions>
		struct serializer_t<Context, where_data_t<Database, Expressions...>>
		{
			using _serialize_check = serialize_check_of<Context, Expressions...>;
			using T = where_data_t<Database, Expressions...>;

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
		struct serializer_t<Context, where_data_t<void, bool>>
		{
			using _serialize_check = consistent_t;
			using T = where_data_t<void, bool>;

			static Context& _(const T& t, Context& context)
			{
				if (not t._condition)
					context << " WHERE NULL";
				return context;
			}
		};

	template<typename... T>
		auto where(T&&... t) -> decltype(statement_t<void, no_where_t<false>>().where(std::forward<T>(t)...))
		{
			return statement_t<void, no_where_t<false>>().where(std::forward<T>(t)...);
		}
}

#endif

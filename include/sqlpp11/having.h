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
#include <sqlpp11/expression.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/detail/logic.h>

namespace sqlpp
{
	// HAVING DATA
	template<typename Database, typename... Expressions>
		struct having_data_t
		{
			having_data_t(Expressions... expressions):
				_expressions(expressions...)
			{}

			having_data_t(const having_data_t&) = default;
			having_data_t(having_data_t&&) = default;
			having_data_t& operator=(const having_data_t&) = default;
			having_data_t& operator=(having_data_t&&) = default;
			~having_data_t() = default;

			std::tuple<Expressions...> _expressions;
			interpretable_list_t<Database> _dynamic_expressions;
		};

	struct assert_no_unknown_tables_in_having_t
	{
		using type = std::false_type;

		template<typename T = void>
		static void _()
		{
			static_assert(wrong_t<T>::value, "at least one having-expression requires a table which is otherwise not known in the statement");
		}
	};

	// HAVING
	template<typename Database, typename... Expressions>
		struct having_t
		{
			using _traits = make_traits<no_value_t, tag::is_having>;
			using _recursive_traits = make_recursive_traits<Expressions...>;

			using _is_dynamic = is_database<Database>;

			// Data
			using _data_t = having_data_t<Database, Expressions...>;

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
							static_assert(_is_dynamic::value, "having::add() can only be called for dynamic_having");
							static_assert(is_expression_t<Expression>::value, "invalid expression argument in having::add()");
							static_assert(not TableCheckRequired::value or Policies::template _no_unknown_tables<Expression>::value, "expression uses tables unknown to this statement in having::add()");
							using _serialize_check = sqlpp::serialize_check_t<typename Database::_serializer_context_t, Expression>;
							_serialize_check::_();

							using ok = detail::all_t<_is_dynamic::value, is_expression_t<Expression>::value, _serialize_check::type::value>;

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
					using _data_t = having_data_t<Database, Expressions...>;

					_impl_t<Policies> having;
					_impl_t<Policies>& operator()() { return having; }
					const _impl_t<Policies>& operator()() const { return having; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.having)
						{
							return t.having;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
					using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<having_t>::value,
								consistent_t,
								assert_no_unknown_tables_in_having_t>::type;
				};
		};

	// NO HAVING YET
	struct no_having_t
	{
		using _traits = make_traits<no_value_t, tag::is_noop>;
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

				_impl_t<Policies> no_having;
				_impl_t<Policies>& operator()() { return no_having; }
				const _impl_t<Policies>& operator()() const { return no_having; }

				template<typename T>
					static auto _get_member(T t) -> decltype(t.no_having)
					{
						return t.no_having;
					}
			};

		template<typename Policies>
			struct _methods_t
			{
				using _database_t = typename Policies::_database_t;

				template<typename... T>
					using _check = detail::all_t<is_expression_t<T>::value...>;

				template<typename Check, typename T>
					using _new_statement_t = new_statement_t<Check::value, Policies, no_having_t, T>;

				using _consistency_check = consistent_t;

				template<typename... Expressions>
					auto having(Expressions... expressions) const
					-> _new_statement_t<_check<Expressions...>, having_t<void, Expressions...>>
					{
						static_assert(_check<Expressions...>::value, "at least one argument is not an expression in having()");
						static_assert(sizeof...(Expressions), "at least one expression argument required in having()");

						return _having_impl<void>(_check<Expressions...>{}, expressions...);
					}

				template<typename... Expressions>
					auto dynamic_having(Expressions... expressions) const
					-> _new_statement_t<_check<Expressions...>, having_t<_database_t, Expressions...>>
					{
						static_assert(_check<Expressions...>::value, "at least one argument is not an expression in having()");
						static_assert(not std::is_same<_database_t, void>::value, "dynamic_having must not be called in a static statement");
						return _having_impl<_database_t>(_check<Expressions...>{}, expressions...);
					}

			private:
				template<typename Database, typename... Expressions>
					auto _having_impl(const std::false_type&, Expressions... expressions) const
					-> bad_statement;

				template<typename Database, typename... Expressions>
					auto _having_impl(const std::true_type&, Expressions... expressions) const
					-> _new_statement_t<std::true_type, having_t<Database, Expressions...>>
					{
						return { static_cast<const derived_statement_t<Policies>&>(*this), having_data_t<_database_t, Expressions...>{expressions...} };
					}

			};
	};

	// Interpreters
	template<typename Context, typename Database, typename... Expressions>
		struct serializer_t<Context, having_data_t<Database, Expressions...>>
		{
			using _serialize_check = serialize_check_of<Context, Expressions...>;
			using T = having_data_t<Database, Expressions...>;

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
}

#endif

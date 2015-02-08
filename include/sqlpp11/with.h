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

#ifndef SQLPP_WITH_H
#define SQLPP_WITH_H

#include <sqlpp11/statement_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/expression.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/logic.h>


#include <sqlpp11/cte.h>

namespace sqlpp
{
	template<typename Database, typename... Expressions>
		struct with_data_t
		{
			with_data_t(Expressions... expressions):
				_expressions(expressions...)
			{}

			with_data_t(const with_data_t&) = default;
			with_data_t(with_data_t&&) = default;
			with_data_t& operator=(const with_data_t&) = default;
			with_data_t& operator=(with_data_t&&) = default;
			~with_data_t() = default;

			std::tuple<Expressions...> _expressions;
		};

	template<typename Database, typename... Expressions>
		struct with_t
		{
			using _traits = make_traits<no_value_t, tag::is_with>;
			struct _recursive_traits
			{
				using _required_ctes = detail::type_set<>;
				using _provided_ctes = detail::make_joined_set_t<required_ctes_of<Expressions>...>; // with provides common table expressions
				using _required_tables = detail::type_set<>;
				using _provided_tables = detail::type_set<>;
				using _provided_outer_tables = detail::type_set<>;
				using _extra_tables = detail::type_set<>;
				using _parameters = detail::make_parameter_tuple_t<parameters_of<Expressions>...>;
				using _tags = detail::type_set<>;
			};


			using _is_dynamic = is_database<Database>;

			// Data
			using _data_t = with_data_t<Database, Expressions...>;

			// Member implementation with data and methods
			template <typename Policies>
				struct _impl_t
				{
				public:
					_data_t _data;
				};

			// Base template to be inherited by the statement
			template<typename Policies>
				struct _base_t
				{
					using _data_t = with_data_t<Database, Expressions...>;

					_impl_t<Policies> with;
					_impl_t<Policies>& operator()() { return with; }
					const _impl_t<Policies>& operator()() const { return with; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.with)
						{
							return t.with;
						}

					// FIXME: Need real checks here
					using _consistency_check = consistent_t;
				};
		};


	struct no_with_t
	{
		using _traits = make_traits<no_value_t, tag::is_with>;
		using _recursive_traits = make_recursive_traits<>;

		// Data
		using _data_t = no_data_t;

		// Member implementation with data and methods
		template<typename Policies>
			struct _impl_t
			{
				_data_t _data;
			};

		// Base template to be inherited by the statement
		template<typename Policies>
			struct _base_t
			{
				using _data_t = no_data_t;

				_impl_t<Policies> no_with;
				_impl_t<Policies>& operator()() { return no_with; }
				const _impl_t<Policies>& operator()() const { return no_with; }

				template<typename T>
					static auto _get_member(T t) -> decltype(t.no_with)
					{
						return t.no_with;
					}

				using _consistency_check = consistent_t;

			};
	};

	template<typename Database, typename... Expressions>
		struct blank_with_t
		{
			with_data_t<Database, Expressions...> _data;

			template<typename Statement>
				auto operator()(Statement statement)
				-> new_statement_t<true, typename Statement::_policies_t, no_with_t, with_t<Database, Expressions...>>
				{
					// FIXME need checks here
					//       check that no cte refers to any of the ctes to the right
					return { statement, _data };
				}
		};

	// Interpreters
	template<typename Context, typename Database, typename... Expressions>
		struct serializer_t<Context, with_data_t<Database, Expressions...>>
		{
			using _serialize_check = serialize_check_of<Context, Expressions...>;
			using T = with_data_t<Database, Expressions...>;

			static Context& _(const T& t, Context& context)
			{
				// FIXME: If there is a recursive CTE, add a "RECURSIVE" here
				context << " WITH ";
				interpret_tuple(t._expressions, ',', context);
				return context;
			}
		};

	template<typename... Expressions>
		auto with(Expressions... cte)
		-> blank_with_t<void, Expressions...>
		{
			return { {cte...} };
		}
}

#endif

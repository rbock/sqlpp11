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

#ifndef SQLPP_INSERT_LIST_H
#define SQLPP_INSERT_LIST_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/set.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/interpretable_list.h>

namespace sqlpp
{
	namespace vendor
	{
		struct insert_default_values_t
		{
			using _is_insert_list = std::true_type;
			using _is_dynamic = std::false_type;
		}; 

		template<typename Context>
			struct interpreter_t<Context, insert_default_values_t>
			{
				using T = insert_default_values_t;

				static Context& _(const T& t, Context& context)
				{
					context << " DEFAULT VALUES";
					return context;
				}
			};

		template<typename Column>
			struct insert_column_t
			{
				Column _column;
			};

		template<typename Context, typename Column>
			struct interpreter_t<Context, insert_column_t<Column>>
			{
				using T = insert_column_t<Column>;

				static Context& _(const T& t, Context& context)
				{
					context << t._column._get_name();
					return context;
				}
			};


		template<typename Database, typename... Assignments>
			struct insert_list_t
			{
				using _is_insert_list = std::true_type;
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
				using _parameter_tuple_t = std::tuple<typename Assignments::value_type...>;

				// check for at least one order expression
				static_assert(_is_dynamic::value or sizeof...(Assignments), "at least one select expression required in set()");

				// check for duplicate assignments
				static_assert(not detail::has_duplicates<Assignments...>::value, "at least one duplicate argument detected in set()");

				// check for invalid assignments
				using _assignment_set = typename detail::make_set_if<is_assignment_t, Assignments...>::type;
				static_assert(_assignment_set::size::value == sizeof...(Assignments), "at least one argument is not an assignment in set()");

				// check for prohibited assignments
				using _prohibited_assignment_set = typename detail::make_set_if<must_not_insert_t, typename Assignments::column_type...>::type;
				static_assert(_prohibited_assignment_set::size::value == 0, "at least one assignment is prohibited by its column definition in set()");

				insert_list_t(Assignments... assignment):
					_columns({assignment._lhs}...),
					_values(assignment._rhs...)
					{}

				insert_list_t(const insert_list_t&) = default;
				insert_list_t(insert_list_t&&) = default;
				insert_list_t& operator=(const insert_list_t&) = default;
				insert_list_t& operator=(insert_list_t&&) = default;
				~insert_list_t() = default;

				template<typename Assignment>
					void add(Assignment&& assignment)
					{
						static_assert(is_assignment_t<typename std::decay<Assignment>::type>::value, "set() arguments require to be assigments");
						static_assert(not must_not_insert_t<typename std::decay<Assignment>::type>::value, "set() argument must not be used in insert");
						_dynamic_columns.emplace_back(insert_column_t<typename Assignment::column_type>{std::forward<typename Assignment::column_type>(assignment._lhs)});
						_dynamic_values.emplace_back(std::forward<typename Assignment::value_type>(assignment._rhs));
					}


				std::tuple<insert_column_t<typename Assignments::column_type>...> _columns;
				_parameter_tuple_t _values;
				typename vendor::interpretable_list_t<Database> _dynamic_columns;
				typename vendor::interpretable_list_t<Database> _dynamic_values;
			};

		template<typename Context, typename Database, typename... Assignments>
			struct interpreter_t<Context, insert_list_t<Database, Assignments...>>
			{
				using T = insert_list_t<Database, Assignments...>;

				static Context& _(const T& t, Context& context)
				{
					if (sizeof...(Assignments) + t._dynamic_columns.size() == 0)
					{
						interpret(insert_default_values_t(), context);
					}
					else
					{
						context << " (";
						interpret_tuple(t._columns, ",", context);
						if (sizeof...(Assignments) and not t._dynamic_columns.empty())
							context << ',';
						interpret_list(t._dynamic_columns, ',', context);
						context << ") VALUES(";
						interpret_tuple(t._values, ",", context);
						if (sizeof...(Assignments) and not t._dynamic_values.empty())
							context << ',';
						interpret_list(t._dynamic_values, ',', context);
						context << ")";
					}
					return context;
				}
			};
	}
}

#endif

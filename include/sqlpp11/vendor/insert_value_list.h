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

#ifndef SQLPP_INSERT_VALUE_LIST_H
#define SQLPP_INSERT_VALUE_LIST_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/logic.h>
#include <sqlpp11/vendor/assignment.h>
#include <sqlpp11/vendor/interpretable_list.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/insert_value.h>
#include <sqlpp11/vendor/simple_column.h>

namespace sqlpp
{
	namespace vendor
	{
		// COLUMN AND VALUE LIST
		struct insert_default_values_t
		{
			using _table_set = ::sqlpp::detail::type_set<>;
			using _is_dynamic = std::false_type;

			template<typename Policies>
				struct _methods_t
				{};
		}; 

		template<typename Database, typename... Assignments>
			struct insert_list_t
			{
				using _is_insert_list = std::true_type;
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
				using _parameter_tuple_t = std::tuple<Assignments...>;
				template<template<typename...> class Target>
					using copy_assignments_t = Target<Assignments...>; // FIXME: Nice idea to copy variadic template arguments?
				template<template<typename...> class Target, template<typename> class Wrap>
					using copy_wrapped_assignments_t = Target<Wrap<Assignments>...>;

				static_assert(_is_dynamic::value or sizeof...(Assignments), "at least one select expression required in set()");

				static_assert(not ::sqlpp::detail::has_duplicates<Assignments...>::value, "at least one duplicate argument detected in set()");

				static_assert(sqlpp::detail::all_t<is_assignment_t, Assignments...>::value, "at least one argument is not an assignment in set()");

					static_assert(sqlpp::detail::none_t<must_not_insert_t<typename Assignments::_column_t>::value...>::value, "at least one assignment is prohibited by its column definition in set()");

				using _column_table_set = typename ::sqlpp::detail::make_joined_set<typename Assignments::_column_t::_table_set...>::type;
				using _value_table_set = typename ::sqlpp::detail::make_joined_set<typename Assignments::value_type::_table_set...>::type;
				using _table_set = typename ::sqlpp::detail::make_joined_set<_column_table_set, _value_table_set>::type;
				static_assert(sizeof...(Assignments) ? (_column_table_set::size::value == 1) : true, "set() contains assignments for tables from several columns");
				static_assert(::sqlpp::detail::is_subset_of<_value_table_set, _column_table_set>::value, "set() contains values from foreign tables");
				
				insert_list_t(Assignments... assignment):
					_assignments(assignment...),
					_columns({assignment._lhs}...),
					_values(assignment._rhs...)
					{}

				insert_list_t(const insert_list_t&) = default;
				insert_list_t(insert_list_t&&) = default;
				insert_list_t& operator=(const insert_list_t&) = default;
				insert_list_t& operator=(insert_list_t&&) = default;
				~insert_list_t() = default;

				template<typename Policies>
					struct _methods_t
					{
						template<typename Assignment>
							void add_set(Assignment assignment)
							{
								static_assert(_is_dynamic::value, "add_set must not be called for static from()");
								static_assert(is_assignment_t<Assignment>::value, "add_set() arguments require to be assigments");
								static_assert(not must_not_insert_t<typename Assignment::_column_t>::value, "add_set() argument must not be used in insert");
								using _column_table_set = typename Assignment::_column_t::_table_set;
								using _value_table_set = typename Assignment::value_type::_table_set;
								static_assert(::sqlpp::detail::is_subset_of<_value_table_set, typename Policies::_table_set>::value, "add_set() contains a column from a foreign table");
								static_assert(::sqlpp::detail::is_subset_of<_column_table_set, typename Policies::_table_set>::value, "add_set() contains a value from a foreign table");

								using ok = ::sqlpp::detail::all_t<sqlpp::detail::identity_t, 
											_is_dynamic, 
											is_assignment_t<Assignment>, 
											::sqlpp::detail::not_t<must_not_insert_t, typename Assignment::_column_t>,
											::sqlpp::detail::is_subset_of<_value_table_set, typename Policies::_table_set>,
											::sqlpp::detail::is_subset_of<_column_table_set, typename Policies::_table_set>>;

								_add_set_impl(assignment, ok()); // dispatch to prevent compile messages after the static_assert
							}

					private:
						template<typename Assignment>
							void _add_set_impl(Assignment assignment, const std::true_type&)
							{
								static_cast<typename Policies::_statement_t*>(this)->_insert_value_list._dynamic_columns.emplace_back(simple_column_t<typename Assignment::_column_t>{assignment._lhs});
								static_cast<typename Policies::_statement_t*>(this)->_insert_value_list._dynamic_values.emplace_back(assignment._rhs);
							}

						template<typename Assignment>
							void _add_set_impl(Assignment assignment, const std::false_type&);
					};



				std::tuple<simple_column_t<typename Assignments::_column_t>...> _columns;
				std::tuple<typename Assignments::value_type...> _values;
				std::tuple<Assignments...> _assignments; // FIXME: Need to replace _columns and _values by _assignments (connector-container requires assignments)
				typename vendor::interpretable_list_t<Database> _dynamic_columns;
				typename vendor::interpretable_list_t<Database> _dynamic_values;
			};

		template<typename... Columns>
			struct column_list_t
			{
				using _is_column_list = std::true_type;
				using _parameter_tuple_t = std::tuple<Columns...>;

				static_assert(sizeof...(Columns), "at least one column required in columns()");

				static_assert(not ::sqlpp::detail::has_duplicates<Columns...>::value, "at least one duplicate argument detected in columns()");

				static_assert(::sqlpp::detail::all_t<is_column_t, Columns...>::value, "at least one argument is not a column in columns()");

				static_assert(::sqlpp::detail::none_t<must_not_insert_t<Columns>::value...>::value, "at least one column argument has a must_not_insert flag in its definition");

				using _value_tuple_t = std::tuple<vendor::insert_value_t<Columns>...>;
				using _table_set = typename ::sqlpp::detail::make_joined_set<typename Columns::_table_set...>::type;

				static_assert(_table_set::size::value == 1, "columns from multiple tables in columns()");

				column_list_t(Columns... columns):
					_columns(simple_column_t<Columns>{columns}...)
				{}

				column_list_t(const column_list_t&) = default;
				column_list_t(column_list_t&&) = default;
				column_list_t& operator=(const column_list_t&) = default;
				column_list_t& operator=(column_list_t&&) = default;
				~column_list_t() = default;

				template<typename Policies>
					struct _methods_t
					{
						template<typename... Assignments>
							void add_values(Assignments... assignments)
							{
								static_assert(::sqlpp::detail::all_t<is_assignment_t, Assignments...>::value, "add_values() arguments have to be assignments");
								using _arg_value_tuple = std::tuple<vendor::insert_value_t<typename Assignments::_column_t>...>;
								using _args_correct = std::is_same<_arg_value_tuple, _value_tuple_t>;
								static_assert(_args_correct::value, "add_values() arguments do not match columns() arguments");

								using ok = ::sqlpp::detail::all_t<sqlpp::detail::identity_t, 
											::sqlpp::detail::all_t<is_assignment_t, Assignments...>, 
											_args_correct>;

								_add_values_impl(ok(), assignments...); // dispatch to prevent compile messages after the static_assert
							}

					private:
						template<typename... Assignments>
							void _add_values_impl(const std::true_type&, Assignments... assignments)
							{
								return static_cast<typename Policies::_statement_t*>(this)->_insert_value_list._insert_values.emplace_back(vendor::insert_value_t<typename Assignments::_column_t>{assignments}...);
							}

						template<typename... Assignments>
							void _add_values_impl(const std::false_type&, Assignments... assignments);
					};

				bool empty() const
				{
					return _insert_values.empty();
				}

				std::tuple<simple_column_t<Columns>...> _columns;
				std::vector<_value_tuple_t> _insert_values;

			};

		struct no_insert_value_list_t
		{
			using _is_noop = std::true_type;
			using _table_set = ::sqlpp::detail::type_set<>;

			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
					using _new_statement_t = typename Policies::template _new_statement_t<no_insert_value_list_t, T>;

						auto default_values()
						-> _new_statement_t<insert_default_values_t>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), insert_default_values_t{} };
						}

					template<typename... Args>
						auto columns(Args... args)
						-> _new_statement_t<column_list_t<Args...>>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), column_list_t<Args...>{args...} };
						}

					template<typename... Args>
						auto set(Args... args)
						-> _new_statement_t<insert_list_t<void, Args...>>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), insert_list_t<void, Args...>{args...} };
						}

					template<typename... Args>
						auto dynamic_set(Args... args)
						-> _new_statement_t<insert_list_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_set must not be called in a static statement");
							return { *static_cast<typename Policies::_statement_t*>(this), vendor::insert_list_t<_database_t, Args...>{args...} };
						}
				};
		};

		// Interpreters
		template<typename Context>
			struct serializer_t<Context, insert_default_values_t>
			{
				using T = insert_default_values_t;

				static Context& _(const T& t, Context& context)
				{
					context << " DEFAULT VALUES";
					return context;
				}
			};

		template<typename Context, typename... Columns>
			struct serializer_t<Context, column_list_t<Columns...>>
			{
				using T = column_list_t<Columns...>;

				static Context& _(const T& t, Context& context)
				{
					context << " (";
					interpret_tuple(t._columns, ",", context);
					context << ")";
					context << " VALUES ";
					bool first = true;
					for (const auto& row : t._insert_values)
					{
						if (not first)
							context << ',';
						else
							first = false;
						context << '(';
						interpret_tuple(row, ",", context);
						context << ')';
					}

					return context;
				}
			};

		template<typename Context, typename Database, typename... Assignments>
			struct serializer_t<Context, insert_list_t<Database, Assignments...>>
			{
				using T = insert_list_t<Database, Assignments...>;

				static Context& _(const T& t, Context& context)
				{
					if (sizeof...(Assignments) + t._dynamic_columns.size() == 0)
					{
						serialize(insert_default_values_t(), context);
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

		template<typename Context>
			struct serializer_t<Context, no_insert_value_list_t>
			{
				using T = no_insert_value_list_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

	}
}

#endif

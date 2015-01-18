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
#include <sqlpp11/assignment.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/insert_value.h>
#include <sqlpp11/simple_column.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/policy_update.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename... Args>
			struct first_arg_impl
			{
				static_assert(wrong_t<first_arg_impl>::value, "At least one argument required");
			};

		template<typename T, typename... Args>
			struct first_arg_impl<T, Args...>
			{
				using type = T;
			};

		template<typename... Args>
			using first_arg_t = typename first_arg_impl<Args...>::type;
	}

	struct insert_default_values_data_t
	{};

	// COLUMN AND VALUE LIST
	struct insert_default_values_t
	{
		using _traits = make_traits<no_value_t>;
		using _recursive_traits = make_recursive_traits<>;

		// Data
		using _data_t = insert_default_values_data_t;

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
				using _data_t = insert_default_values_data_t;

				_impl_t<Policies> default_values;
				_impl_t<Policies>& operator()() { return default_values; }
				const _impl_t<Policies>& operator()() const { return default_values; }

				template<typename T>
					static auto _get_member(T t) -> decltype(t.default_values)
					{
						return t.default_values;
					}
			};

		template<typename Policies>
			struct _methods_t
			{
				static void _check_consistency() {}
			};
	}; 

	template<typename Database, typename... Assignments>
		struct insert_list_data_t
		{
			insert_list_data_t(Assignments... assignments):
				_assignments(assignments...),
				_columns({assignments._lhs}...),
				_values(assignments._rhs...)
				{}

			insert_list_data_t(const insert_list_data_t&) = default;
			insert_list_data_t(insert_list_data_t&&) = default;
			insert_list_data_t& operator=(const insert_list_data_t&) = default;
			insert_list_data_t& operator=(insert_list_data_t&&) = default;
			~insert_list_data_t() = default;

			std::tuple<Assignments...> _assignments; // FIXME: Need to replace _columns and _values by _assignments (connector-container requires assignments)
			std::tuple<simple_column_t<lhs_t<Assignments>>...> _columns;
			std::tuple<rhs_t<Assignments>...> _values;
			interpretable_list_t<Database> _dynamic_columns;
			interpretable_list_t<Database> _dynamic_values;
		};

	template<typename Database, typename... Assignments>
		struct insert_list_t
		{
			using _traits = make_traits<no_value_t, tag::is_insert_list>;
			using _recursive_traits = make_recursive_traits<lhs_t<Assignments>..., rhs_t<Assignments>...>;

			using _is_dynamic = is_database<Database>;

			template<template<typename...> class Target>
				using copy_assignments_t = Target<Assignments...>; // FIXME: Nice idea to copy variadic template arguments?
			template<template<typename...> class Target, template<typename> class Wrap>
				using copy_wrapped_assignments_t = Target<Wrap<Assignments>...>;

			// Data
			using _data_t = insert_list_data_t<Database, Assignments...>;

			// Member implementation with data and methods
			template <typename Policies>
				struct _impl_t
				{
					template<typename Assignment>
						void add_ntc(Assignment assignment)
						{
							add<Assignment, std::false_type>(assignment);
						}

					template<typename Assignment, typename TableCheckRequired = std::true_type>
						void add(Assignment assignment)
						{
							static_assert(_is_dynamic::value, "add must not be called for static from()");
							static_assert(is_assignment_t<Assignment>::value, "add() arguments require to be assigments");
							using _assigned_columns = detail::make_type_set_t<lhs_t<Assignments>...>;
							static_assert(not detail::is_element_of<lhs_t<Assignment>, _assigned_columns>::value, "Must not assign value to column twice");
							static_assert(not must_not_insert_t<lhs_t<Assignment>>::value, "add() argument must not be used in insert");
							static_assert(not TableCheckRequired::value or Policies::template _no_unknown_tables<Assignment>::value, "add() contains a column from a foreign table");

							using ok = detail::all_t<
								_is_dynamic::value, 
								is_assignment_t<Assignment>::value>;

							_add_impl(assignment, ok()); // dispatch to prevent compile messages after the static_assert
						}

				private:
					template<typename Assignment>
						void _add_impl(Assignment assignment, const std::true_type&)
						{
							_data._dynamic_columns.emplace_back(simple_column_t<lhs_t<Assignment>>{assignment._lhs});
							_data._dynamic_values.emplace_back(assignment._rhs);
						}

					template<typename Assignment>
						void _add_impl(Assignment assignment, const std::false_type&);
				public:
					_data_t _data;
				};

			// Member template for adding the named member to a statement
			template<typename Policies>
				struct _member_t
				{
					using _data_t = insert_list_data_t<Database, Assignments...>;

					_impl_t<Policies> insert_list;
					_impl_t<Policies>& operator()() { return insert_list; }
					const _impl_t<Policies>& operator()() const { return insert_list; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.insert_list)
						{
							return t.insert_list;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
					static void _check_consistency() {}
				};

		};

	template<typename... Columns>
		struct column_list_data_t
		{
			column_list_data_t(Columns... columns):
				_columns(simple_column_t<Columns>{columns}...)
				{}

			column_list_data_t(const column_list_data_t&) = default;
			column_list_data_t(column_list_data_t&&) = default;
			column_list_data_t& operator=(const column_list_data_t&) = default;
			column_list_data_t& operator=(column_list_data_t&&) = default;
			~column_list_data_t() = default;

			using _value_tuple_t = std::tuple<insert_value_t<Columns>...>;
			std::tuple<simple_column_t<Columns>...> _columns;
			std::vector<_value_tuple_t> _insert_values;
		};

	template<typename... Columns>
		struct column_list_t
		{
			using _traits = make_traits<no_value_t, tag::is_column_list>;
			using _recursive_traits = make_recursive_traits<Columns...>;

			using _value_tuple_t = typename column_list_data_t<Columns...>::_value_tuple_t;


			// Data
			using _data_t = column_list_data_t<Columns...>;

			// Member implementation with data and methods
			template <typename Policies>
				struct _impl_t
				{
					template<typename... Assignments>
						void add(Assignments... assignments)
						{
							static_assert(detail::all_t<is_assignment_t<Assignments>::value...>::value, "add_values() arguments have to be assignments");
							using _arg_value_tuple = std::tuple<insert_value_t<lhs_t<Assignments>>...>;
							using _args_correct = std::is_same<_arg_value_tuple, _value_tuple_t>;
							static_assert(_args_correct::value, "add_values() arguments do not match columns() arguments");

							using ok = detail::all_t<
								detail::all_t<is_assignment_t<Assignments>::value...>::value, 
								_args_correct::value>;

							_add_impl(ok(), assignments...); // dispatch to prevent compile messages after the static_assert
						}

				private:
					template<typename... Assignments>
						void _add_impl(const std::true_type&, Assignments... assignments)
						{
							return _data._insert_values.emplace_back(insert_value_t<lhs_t<Assignments>>{assignments._rhs}...);
						}

					template<typename... Assignments>
						void _add_impl(const std::false_type&, Assignments... assignments);
				public:
					_data_t _data;
				};

			// Member template for adding the named member to a statement
			template<typename Policies>
				struct _member_t
				{
					using _data_t = column_list_data_t<Columns...>;

					_impl_t<Policies> values;
					_impl_t<Policies>& operator()() { return values; }
					const _impl_t<Policies>& operator()() const { return values; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.values)
						{
							return t.values;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
					static void _check_consistency() {}
				};
		};

	// NO INSERT COLUMNS/VALUES YET
	struct no_insert_value_list_t
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

				_impl_t<Policies> no_insert_values;
				_impl_t<Policies>& operator()() { return no_insert_values; }
				const _impl_t<Policies>& operator()() const { return no_insert_values; }

				template<typename T>
					static auto _get_member(T t) -> decltype(t.no_insert_values)
					{
						return t.no_insert_values;
					}
			};

		template<typename Policies>
			struct _methods_t
			{
				using _database_t = typename Policies::_database_t;
				template<typename T>
					using _new_statement_t = new_statement<Policies, no_insert_value_list_t, T>;

				static void _check_consistency() 
				{
					static_assert(wrong_t<_methods_t>::value, "insert values required, e.g. set(...) or default_values()");
				}

				auto default_values() const
					-> _new_statement_t<insert_default_values_t>
					{
						return { static_cast<const derived_statement_t<Policies>&>(*this), insert_default_values_data_t{} };
					}

				template<typename... Columns>
					auto columns(Columns... columns) const
					-> _new_statement_t<column_list_t<Columns...>>
					{
						static_assert(sizeof...(Columns), "at least one column required in columns()");
						static_assert(not detail::has_duplicates<Columns...>::value, "at least one duplicate argument detected in columns()");
						static_assert(detail::all_t<is_column_t<Columns>::value...>::value, "at least one argument is not a column in columns()");
						static_assert(detail::none_t<must_not_insert_t<Columns>::value...>::value, "at least one column argument has a must_not_insert tag in its definition");
						using _column_required_tables = detail::make_joined_set_t<required_tables_of<Columns>...>;
						static_assert(_column_required_tables::size::value == 1, "columns() contains columns from several tables");

						using _table = typename detail::first_arg_t<Columns...>::_table;
						using required_columns = typename _table::_required_insert_columns;
						using set_columns = detail::make_type_set_t<Columns...>;
						static_assert(detail::is_subset_of<required_columns, set_columns>::value, "At least one required column is missing in columns()");

						return { static_cast<const derived_statement_t<Policies>&>(*this), column_list_data_t<Columns...>{columns...} };
					}

				template<typename... Assignments>
					auto set(Assignments... assignments) const
					-> _new_statement_t<insert_list_t<void, Assignments...>>
					{
						static_assert(sizeof...(Assignments), "at least one assignment expression required in set()");
						static_assert(detail::all_t<is_assignment_t<Assignments>::value...>::value, "at least one argument is not an assignment in set()");

						using _table = typename lhs_t<detail::first_arg_t<Assignments...>>::_table;
						using required_columns = typename _table::_required_insert_columns;
						using columns = detail::make_type_set_t<lhs_t<Assignments>...>;
						static_assert(detail::is_subset_of<required_columns, columns>::value, "At least one required column is missing in set()");
						return _set_impl<void>(assignments...);
					}

				template<typename... Assignments>
					auto dynamic_set(Assignments... assignments) const
					-> _new_statement_t<insert_list_t<_database_t, Assignments...>>
					{
						static_assert(not std::is_same<_database_t, void>::value, "dynamic_set must not be called in a static statement");
						static_assert(detail::all_t<is_assignment_t<Assignments>::value...>::value, "at least one argument is not an assignment in set()");
						return _set_impl<_database_t>(assignments...);
					}
			private:
				template<typename Database, typename... Assignments>
					auto _set_impl(Assignments... assignments) const
					-> _new_statement_t<insert_list_t<Database, Assignments...>>
					{
						static_assert(not detail::has_duplicates<lhs_t<Assignments>...>::value, "at least one duplicate column detected in set()");
						static_assert(detail::none_t<must_not_insert_t<lhs_t<Assignments>>::value...>::value, "at least one assignment is prohibited by its column definition in set()");

						using _column_required_tables = detail::make_joined_set_t<required_tables_of<lhs_t<Assignments>>...>;
						static_assert(sizeof...(Assignments) ? (_column_required_tables::size::value == 1) : true, "set() contains assignments for columns from several tables");

						return { static_cast<const derived_statement_t<Policies>&>(*this), insert_list_data_t<Database, Assignments...>{assignments...} };
					}
			};
	};

	// Interpreters
	template<typename Context>
		struct serializer_t<Context, insert_default_values_data_t>
		{
			using T = insert_default_values_data_t;

			static Context& _(const T& t, Context& context)
			{
				context << " DEFAULT VALUES";
				return context;
			}
		};

	template<typename Context, typename... Columns>
		struct serializer_t<Context, column_list_data_t<Columns...>>
		{
			using T = column_list_data_t<Columns...>;

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
		struct serializer_t<Context, insert_list_data_t<Database, Assignments...>>
		{
			using T = insert_list_data_t<Database, Assignments...>;

			static Context& _(const T& t, Context& context)
			{
				if (sizeof...(Assignments) + t._dynamic_columns.size() == 0)
				{
					serialize(insert_default_values_data_t(), context);
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

#endif

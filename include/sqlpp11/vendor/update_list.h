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

#ifndef SQLPP_UPDATE_LIST_H
#define SQLPP_UPDATE_LIST_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/interpretable_list.h>

namespace sqlpp
{
	namespace vendor
	{
		// UPDATE ASSIGNMENTS
		template<typename Database, typename... Assignments>
			struct update_list_t
			{
				using _traits = make_traits<no_value_t, ::sqlpp::tag::update_list>;
				using _recursive_traits = make_recursive_traits<Assignments...>;
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

				static_assert(_is_dynamic::value or sizeof...(Assignments), "at least one assignment expression required in set()");

				static_assert(not ::sqlpp::detail::has_duplicates<Assignments...>::value, "at least one duplicate argument detected in set()");

				static_assert(::sqlpp::detail::all_t<is_assignment_t<Assignments>::value...>::value, "at least one argument is not an assignment in set()");

				static_assert(::sqlpp::detail::none_t<must_not_update_t<typename Assignments::_column_t>::value...>::value, "at least one assignment is prohibited by its column definition in set()");

#warning reactivate tests
				/*
				using _column_table_set = typename ::sqlpp::detail::make_joined_set<typename Assignments::_column_t::_table_set...>::type;
				using _value_table_set = typename ::sqlpp::detail::make_joined_set<typename Assignments::value_type::_table_set...>::type;
				using _table_set = typename ::sqlpp::detail::make_joined_set<_column_table_set, _value_table_set>::type;
				static_assert(sizeof...(Assignments) ? (_column_table_set::size::value == 1) : true, "set() contains assignments for tables from several columns");
				static_assert(::sqlpp::detail::is_subset_of<_value_table_set, _column_table_set>::value, "set() contains values from foreign tables");
				*/

				update_list_t& _update_list() { return *this; }

				update_list_t(Assignments... assignments):
					_assignments(assignments...)
				{}

				update_list_t(const update_list_t&) = default;
				update_list_t(update_list_t&&) = default;
				update_list_t& operator=(const update_list_t&) = default;
				update_list_t& operator=(update_list_t&&) = default;
				~update_list_t() = default;

				template<typename Policies>
					struct _methods_t
					{
						template<typename Assignment>
							void add_set_ntc(Assignment assignment)
							{
								add_set<Assignment, std::false_type>(assignment);
							}

						template<typename Assignment, typename TableCheckRequired = std::true_type>
							void add_set(Assignment assignment)
							{
								static_assert(_is_dynamic::value, "add_set must not be called for static from()");
								static_assert(is_assignment_t<Assignment>::value, "invalid assignment argument in add_set()");
								static_assert(sqlpp::detail::not_t<must_not_update_t, typename Assignment::_column_t>::value, "add_set() argument must not be updated");
								static_assert(TableCheckRequired::value or Policies::template _no_unknown_tables<Assignment>::value, "assignment uses tables unknown to this statement in add_set()");

								using ok = ::sqlpp::detail::all_t<
											_is_dynamic::value, 
											is_assignment_t<Assignment>::value, 
											not must_not_update_t<typename Assignment::_column_t>::value>;

								_add_set_impl(assignment, ok()); // dispatch to prevent compile messages after the static_assert
							}

					private:
						template<typename Assignment>
							void _add_set_impl(Assignment assignment, const std::true_type&)
							{
								return static_cast<typename Policies::_statement_t*>(this)->_update_list()._dynamic_assignments.emplace_back(assignment);
							}

						template<typename Assignment>
							void _add_set_impl(Assignment assignment, const std::false_type&);
					};


				std::tuple<Assignments...> _assignments;
				typename vendor::interpretable_list_t<Database> _dynamic_assignments;
			};

		struct no_update_list_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::where>;
			using _recursive_traits = make_recursive_traits<>;

			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
					using _new_statement_t = typename Policies::template _new_statement_t<no_update_list_t, T>;

					template<typename... Args>
						auto set(Args... args)
						-> _new_statement_t<update_list_t<void, Args...>>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), update_list_t<void, Args...>{args...} };
						}

					template<typename... Args>
						auto dynamic_set(Args... args)
						-> _new_statement_t<update_list_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_set must not be called in a static statement");
							return { *static_cast<typename Policies::_statement_t*>(this), vendor::update_list_t<_database_t, Args...>{args...} };
						}
				};
		};

		// Interpreters
		template<typename Context, typename Database, typename... Assignments>
			struct serializer_t<Context, update_list_t<Database, Assignments...>>
			{
				using T = update_list_t<Database, Assignments...>;

				static Context& _(const T& t, Context& context)
				{
					context << " SET ";
					interpret_tuple(t._assignments, ",", context);
					if (sizeof...(Assignments) and not t._dynamic_assignments.empty())
						context << ',';
					interpret_list(t._dynamic_assignments, ',', context);
					return context;
				}
			};

		template<typename Context>
			struct serializer_t<Context, no_update_list_t>
			{
				using T = no_update_list_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

	}
}

#endif

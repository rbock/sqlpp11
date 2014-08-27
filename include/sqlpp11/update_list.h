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
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>

namespace sqlpp
{
	// UPDATE ASSIGNMENTS DATA
	template<typename Database, typename... Assignments>
		struct update_list_data_t
		{
			update_list_data_t(Assignments... assignments):
				_assignments(assignments...)
			{}

			update_list_data_t(const update_list_data_t&) = default;
			update_list_data_t(update_list_data_t&&) = default;
			update_list_data_t& operator=(const update_list_data_t&) = default;
			update_list_data_t& operator=(update_list_data_t&&) = default;
			~update_list_data_t() = default;

			std::tuple<Assignments...> _assignments;
			interpretable_list_t<Database> _dynamic_assignments;
		};

	// UPDATE ASSIGNMENTS
	template<typename Database, typename... Assignments>
		struct update_list_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::is_update_list>;
			using _recursive_traits = make_recursive_traits<Assignments...>;
			using _is_dynamic = is_database<Database>;

			// Data
			using _data_t = update_list_data_t<Database, Assignments...>;

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
							static_assert(is_assignment_t<Assignment>::value, "invalid assignment argument in add()");
							using _assigned_columns = detail::make_type_set_t<lhs_t<Assignments>...>;
							static_assert(not detail::is_element_of<lhs_t<Assignment>, _assigned_columns>::value, "Must not assign value to column twice");
							static_assert(sqlpp::detail::not_t<must_not_update_t, lhs_t<Assignment>>::value, "add() argument must not be updated");
							static_assert(TableCheckRequired::value or Policies::template _no_unknown_tables<Assignment>::value, "assignment uses tables unknown to this statement in add()");

							using ok = ::sqlpp::detail::all_t<
								_is_dynamic::value, 
								is_assignment_t<Assignment>::value>;

							_add_impl(assignment, ok()); // dispatch to prevent compile messages after the static_assert
						}

				private:
					template<typename Assignment>
						void _add_impl(Assignment assignment, const std::true_type&)
						{
							return _data._dynamic_assignments.emplace_back(assignment);
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
					using _data_t = update_list_data_t<Database, Assignments...>;

					_impl_t<Policies> assignments;
					_impl_t<Policies>& operator()() { return assignments; }
					const _impl_t<Policies>& operator()() const { return assignments; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.assignments)
						{
							return t.assignments;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
					static void _check_consistency() {}
				};
		};

	struct no_update_list_t
	{
		using _traits = make_traits<no_value_t, ::sqlpp::tag::is_where>;
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

				_impl_t<Policies> no_assignments;
				_impl_t<Policies>& operator()() { return no_assignments; }
				const _impl_t<Policies>& operator()() const { return no_assignments; }

				template<typename T>
					static auto _get_member(T t) -> decltype(t.no_assignments)
					{
						return t.no_assignments;
					}
			};

		template<typename Policies>
			struct _methods_t
			{
				using _database_t = typename Policies::_database_t;
				template<typename T>
					using _new_statement_t = new_statement<Policies, no_update_list_t, T>;

				static void _check_consistency()
				{
					static_assert(wrong_t<_methods_t>::value, "update assignments required, i.e. set(...)");
				}

				template<typename... Assignments>
					auto set(Assignments... assignments) const
					-> _new_statement_t<update_list_t<void, Assignments...>>
					{
						static_assert(sizeof...(Assignments), "at least one assignment expression required in set()");
						return _set_impl<void>(assignments...);
					}

				template<typename... Assignments>
					auto dynamic_set(Assignments... assignments) const
					-> _new_statement_t<update_list_t<_database_t, Assignments...>>
					{
						static_assert(not std::is_same<_database_t, void>::value, "dynamic_set() must not be called in a static statement");
						return _set_impl<_database_t>(assignments...);
					}

			private:
				template<typename Database, typename... Assignments>
					auto _set_impl(Assignments... assignments) const
					-> _new_statement_t<update_list_t<Database, Assignments...>>
					{
						static_assert(::sqlpp::detail::all_t<is_assignment_t<Assignments>::value...>::value, "at least one argument is not an assignment in set()");
						static_assert(not ::sqlpp::detail::has_duplicates<lhs_t<Assignments>...>::value, "at least one duplicate column detected in set()");
						static_assert(::sqlpp::detail::none_t<must_not_update_t<lhs_t<Assignments>>::value...>::value, "at least one assignment is prohibited by its column definition in set()");

						using _column_required_tables = ::sqlpp::detail::make_joined_set_t<required_tables_of<lhs_t<Assignments>>...>;
						static_assert(sizeof...(Assignments) ? (_column_required_tables::size::value == 1) : true, "set() contains assignments for columns from more than one table");

						return { static_cast<const derived_statement_t<Policies>&>(*this), update_list_data_t<Database, Assignments...>{assignments...} };
					}
			};
	};

	// Interpreters
	template<typename Context, typename Database, typename... Assignments>
		struct serializer_t<Context, update_list_data_t<Database, Assignments...>>
		{
			using T = update_list_data_t<Database, Assignments...>;

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
}

#endif

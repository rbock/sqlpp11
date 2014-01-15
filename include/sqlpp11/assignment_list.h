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

#ifndef SQLPP_ASSIGNMENT_LIST_H
#define SQLPP_ASSIGNMENT_LIST_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/set.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/detail/serializable_list.h>

namespace sqlpp
{
	template<typename Database, typename... Assignments>
		struct assignment_list_t
		{
			using _is_assignment_list = std::true_type;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
			using _parameter_tuple_t = std::tuple<Assignments...>;

			// check for at least one order expression
			static_assert(_is_dynamic::value or sizeof...(Assignments), "at least one assignment expression required in set()");

			// check for duplicate assignments
			static_assert(not detail::has_duplicates<Assignments...>::value, "at least one duplicate argument detected in set()");

			// check for invalid assignments
			using _assignment_set = typename detail::make_set_if<is_assignment_t, Assignments...>::type;
			static_assert(_assignment_set::size::value == sizeof...(Assignments), "at least one argument is not an assignment in set()");

			// check for prohibited assignments
			using _prohibited_assignment_set = typename detail::make_set_if<must_not_update_t, typename Assignments::column_type...>::type;
			static_assert(_prohibited_assignment_set::size::value == 0, "at least one assignment is prohibited by its column definition in set()");

			template<typename Assignment>
				void add(Assignment&& assignment)
				{
					static_assert(is_assignment_t<typename std::decay<Assignment>::type>::value, "set() arguments require to be assigments");
					static_assert(not must_not_update_t<typename std::decay<Assignment>::type::column_type>::value, "set() argument must not be updated");
					_dynamic_assignments.emplace_back(std::forward<Assignment>(assignment));
				}

			_parameter_tuple_t _assignments;
			typename detail::serializable_list<Database> _dynamic_assignments;
		};

	template<typename Context, typename Database, typename... Assignments>
		struct interpreter_t<Context, assignment_list_t<Database, Assignments...>>
		{
			using T = assignment_list_t<Database, Assignments...>;

			static Context& _(const T& t, Context& context)
			{
				context << " SET ";
				interpret_tuple(t._assignments, ",", context);
				if (sizeof...(Assignments) and not t._dynamic_assignments.empty())
					context << ',';
				interpret_serializable_list(t._dynamic_assignments, ',', context);
				return context;
			}
		};

}

#endif

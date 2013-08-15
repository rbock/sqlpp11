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

#ifndef SQLPP_INSERT_H
#define SQLPP_INSERT_H

#include <sstream>
#include <sqlpp11/noop.h>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/assignment_list.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/serialize_tuple.h>

namespace sqlpp
{
	// Hint: Columns/Values could be supported by .columns().values().values()...
	//       values could take values or assignments. The latter would be stripped
	//       The insert would store a vector of tuples of values.
	// Hint: Select could be supported by perfect forwarding to the methods of _select member (always changing the whole type of the 
	//       insert.
	template<
		typename Table = noop,
		typename Assignments = noop
		>
	struct insert_t;

	template<
		typename Table,
		typename Assignments
		>
		struct insert_t
		{
			static_assert(is_noop<Table>::value or is_table_t<Table>::value, "invalid 'Table' argument");
			static_assert(is_noop<Assignments>::value or is_assignment_list_t<Assignments>::value, "invalid 'Assignments' arguments");

			template<typename... Assignment> 
				using add_assignments_t = insert_t<Table, assignment_list_t<must_not_insert_t, typename std::decay<Assignment>::type...>>;

			template<typename... Assignment>
				add_assignments_t<Assignment...> set(Assignment&&... assignment)
				{
					static_assert(std::is_same<Assignments, noop>::value, "cannot call set() twice");
					return {
							_table,
							{std::tuple<typename std::decay<Assignment>::type...>{std::forward<Assignment>(assignment)...}},
					};
				}

			template<typename Db>
				const insert_t& serialize(std::ostream& os, Db& db) const
				{
					os << "INSERT INTO ";
					_table.serialize(os, db);
					if (is_noop<Assignments>::value)
						os << "() VALUES()";
					else
						_assignments.serialize(os, db);
					return *this;
				}

			template<typename Db>
				insert_t& serialize(std::ostream& os, Db& db)
				{
					static_cast<const insert_t*>(this)->serialize(os, db);
					return *this;
				}

			template<typename Db>
				std::size_t run(Db& db) const
				{
					constexpr bool calledSet = not is_noop<Assignments>::value;
					constexpr bool requireSet = Table::_required_insert_columns::size::value > 0;
					static_assert(calledSet or not requireSet, "calling set() required for given table");
					std::ostringstream oss;
					serialize(oss, db);
					return db.insert(oss.str());
				}

			Table _table;
			Assignments _assignments;
		};

	template<typename Table>
		constexpr insert_t<typename std::decay<Table>::type> insert_into(Table&& table)
		{
			return {std::forward<Table>(table)};
		}

}

#endif

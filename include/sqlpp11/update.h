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

#ifndef SQLPP_UPDATE_H
#define SQLPP_UPDATE_H

#include <sstream>
#include <sqlpp11/noop.h>
#include <sqlpp11/assignment_list.h>
#include <sqlpp11/where.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_update.h>

namespace sqlpp
{
	template<
		typename Database = void,
		typename Table = noop,
		typename Assignments = noop,
		typename Where = noop
		>
	struct update_t;

	template<
		typename Database,
		typename Table,
		typename Assignments,
		typename Where
		>
		struct update_t
		{
			static_assert(is_noop<Table>::value or is_table_t<Table>::value, "invalid 'Table' argument");
			static_assert(is_noop<Assignments>::value or is_assignment_list_t<Assignments>::value, "invalid 'Assignments' arguments");
			static_assert(is_noop<Where>::value or is_where_t<Where>::value, "invalid 'Where' argument");

			template<typename AssignmentsT> 
				using set_assignments_t = update_t<Database, Table, AssignmentsT, Where>;
			template<typename WhereT> 
				using set_where_t = update_t<Database, Table, Assignments, WhereT>;

			using _parameter_tuple_t = std::tuple<Table, Assignments, Where>;
			using _parameter_list_t = typename make_parameter_list_t<update_t>::type;

			template<typename... Assignment>
				auto set(Assignment&&... assignment)
				-> set_assignments_t<assignment_list_t<void, must_not_update_t, typename std::decay<Assignment>::type...>>
				{
					static_assert(std::is_same<Assignments, noop>::value, "cannot call set() twice");
					return {
							_table,
								{std::tuple<typename std::decay<Assignment>::type...>{std::forward<Assignment>(assignment)...}},
							_where,
					};
				}

			template<typename... Assignment>
				auto dynamic_set(Assignment&&... assignment)
				-> set_assignments_t<assignment_list_t<Database, must_not_update_t, typename std::decay<Assignment>::type...>>
				{
					static_assert(std::is_same<Assignments, noop>::value, "cannot call set() twice");
					return {
							_table,
								{std::tuple<typename std::decay<Assignment>::type...>{std::forward<Assignment>(assignment)...}},
							_where,
					};
				}

			template<typename Assignment>
				update_t& add_set(Assignment&& assignment)
				{
					static_assert(is_dynamic_t<Assignments>::value, "cannot call add_set() in a non-dynamic set");

					_assignments.add(std::forward<Assignment>(assignment));

					return *this;
				}

			template<typename... Expr>
				auto where(Expr&&... expr)
				-> set_where_t<where_t<void, typename std::decay<Expr>::type...>>
				{
					static_assert(not std::is_same<Assignments, noop>::value, "cannot call where() if set() hasn't been called yet");
					static_assert(std::is_same<Where, noop>::value, "cannot call where() twice");
					return {
							_table,
							_assignments,
							{std::tuple<typename std::decay<Expr>::type...>{std::forward<Expr>(expr)...}},
					};
				}

			template<typename... Expr>
				auto dynamic_where(Expr&&... expr)
				-> set_where_t<where_t<Database, typename std::decay<Expr>::type...>>
				{
					static_assert(not std::is_same<Assignments, noop>::value, "cannot call where() if set() hasn't been called yet");
					static_assert(std::is_same<Where, noop>::value, "cannot call where() twice");
					return {
						_table, 
							_assignments, 
							{std::tuple<typename std::decay<Expr>::type...>{std::forward<Expr>(expr)...}},
					};
				}

			template<typename Expr>
				update_t& add_where(Expr&& expr)
				{
					static_assert(is_dynamic_t<Where>::value, "cannot call add_where() in a non-dynamic where");

					_where.add(std::forward<Expr>(expr));

					return *this;
				}

			template<typename Db>
				const update_t& serialize(std::ostream& os, Db& db) const
				{
					os << "UPDATE ";
					_table.serialize(os, db);
					_assignments.serialize(os, db);
					_where.serialize(os, db);
					return *this;
				}

			template<typename Db>
				update_t& serialize(std::ostream& os, Db& db)
				{
					static_cast<const update_t*>(this)->serialize(os, db);
					return *this;
				}

			static constexpr size_t _get_static_no_of_parameters()
			{
				return _parameter_list_t::size::value;
			}

			size_t _get_no_of_parameters()
			{
				return _parameter_list_t::size::value; // FIXME: Need to add dynamic parameters here
			}

			template<typename Db>
				std::size_t run(Db& db) const
				{
					static_assert(not is_noop<Assignments>::value, "calling set() required before running update");
					static_assert(_get_static_no_of_parameters() == 0, "cannot run update directly with parameters, use prepare instead");
					return db.update(*this);
				}

			template<typename Db>
				auto prepare(Db& db)
				-> prepared_update_t<typename std::decay<Db>::type, update_t>
				{
					static_assert(not is_noop<Assignments>::value, "calling set() required before running update");

					_set_parameter_index(0);
					return {{}, db.prepare_update(*this)};
				}

			size_t _set_parameter_index(size_t index)
			{
				index = set_parameter_index(_table, index);
				index = set_parameter_index(_assignments, index);
				return index;
			}

			Table _table;
			Assignments _assignments;
			Where _where;
		};

	template<typename Table>
		constexpr update_t<void, typename std::decay<Table>::type> update(Table&& table)
		{
			return {std::forward<Table>(table)};
		}

	template<typename Db, typename Table>
		constexpr update_t<typename std::decay<Db>::type, typename std::decay<Table>::type> dynamic_update(Db&& db, Table&& table)
		{
			return {std::forward<Table>(table)};
		}

}

#endif

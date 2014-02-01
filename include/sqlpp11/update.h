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

#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_update.h>
#include <sqlpp11/vendor/update_list.h>
#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/where.h>

namespace sqlpp
{
	template<
		typename Database = void,
		typename Table = vendor::noop,
		typename Assignments = vendor::noop,
		typename Where = vendor::noop
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
			static_assert(vendor::is_noop<Table>::value or is_table_t<Table>::value, "invalid 'Table' argument");
			static_assert(vendor::is_noop<Assignments>::value or is_update_list_t<Assignments>::value, "invalid 'Assignments' arguments");
			static_assert(vendor::is_noop<Where>::value or is_where_t<Where>::value, "invalid 'Where' argument");

			template<typename AssignmentsT> 
				using set_assignments_t = update_t<Database, Table, AssignmentsT, Where>;
			template<typename WhereT> 
				using set_where_t = update_t<Database, Table, Assignments, WhereT>;

			using _parameter_tuple_t = std::tuple<Table, Assignments, Where>;
			using _parameter_list_t = typename make_parameter_list_t<update_t>::type;

			template<typename... Assignment>
				auto set(Assignment... assignment)
				-> set_assignments_t<vendor::update_list_t<void, Assignment...>>
				{
					static_assert(vendor::is_noop<Assignments>::value, "cannot call set() twice");
					return {
							_table,
								{std::tuple<Assignment...>{assignment...}},
							_where,
					};
				}

			template<typename... Assignment>
				auto dynamic_set(Assignment... assignment)
				-> set_assignments_t<vendor::update_list_t<Database, Assignment...>>
				{
					static_assert(vendor::is_noop<Assignments>::value, "cannot call set() twice");
					return {
							_table,
								{std::tuple<Assignment...>{assignment...}},
							_where,
					};
				}

			template<typename Assignment>
				update_t& add_set(Assignment assignment)
				{
					static_assert(is_dynamic_t<Assignments>::value, "cannot call add_set() in a non-dynamic set");

					_assignments.add(assignment);

					return *this;
				}

			template<typename... Expr>
				auto where(Expr... expr)
				-> set_where_t<vendor::where_t<void, Expr...>>
				{
					static_assert(not vendor::is_noop<Assignments>::value, "cannot call where() if set() hasn't been called yet");
					static_assert(vendor::is_noop<Where>::value, "cannot call where() twice");
					return {
							_table,
							_assignments,
							{std::tuple<Expr...>{expr...}},
					};
				}

			template<typename... Expr>
				auto dynamic_where(Expr... expr)
				-> set_where_t<vendor::where_t<Database, Expr...>>
				{
					static_assert(not vendor::is_noop<Assignments>::value, "cannot call where() if set() hasn't been called yet");
					static_assert(vendor::is_noop<Where>::value, "cannot call where() twice");
					return {
						_table, 
							_assignments, 
							{std::tuple<Expr...>{expr...}},
					};
				}

			template<typename Expr>
				update_t& add_where(Expr expr)
				{
					static_assert(is_dynamic_t<Where>::value, "cannot call add_where() in a non-dynamic where");

					_where.add(expr);

					return *this;
				}

			static constexpr size_t _get_static_no_of_parameters()
			{
				return _parameter_list_t::size::value;
			}

			size_t _get_no_of_parameters() const
			{
				return _parameter_list_t::size::value;
			}

			template<typename Db>
				std::size_t _run(Db& db) const
				{
					static_assert(not vendor::is_noop<Assignments>::value, "calling set() required before running update");
					static_assert(is_where_t<Where>::value, "cannot run update without having a where condition, use .where(true) to update all rows");
					static_assert(_get_static_no_of_parameters() == 0, "cannot run update directly with parameters, use prepare instead");
					return db.update(*this);
				}

			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_update_t<Db, update_t>
				{
					static_assert(not vendor::is_noop<Assignments>::value, "calling set() required before running update");

					return {{}, db.prepare_update(*this)};
				}

			Table _table;
			Assignments _assignments;
			Where _where;
		};

	namespace vendor
	{
		template<typename Context, 
			typename Database,
			typename Table,
			typename Assignments,
			typename Where
				>
				struct interpreter_t<Context, update_t<Database, Table, Assignments, Where>>
				{
					using T = update_t<Database, Table, Assignments, Where>;

					static Context& _(const T& t, Context& context)
					{
						context << "UPDATE ";
						interpret(t._table, context);
						interpret(t._assignments, context);
						interpret(t._where, context);
						return context;
					}
				};
	}

	template<typename Table>
		constexpr update_t<void, Table> update(Table table)
		{
			return {table};
		}

	template<typename Db, typename Table>
		constexpr update_t<Db, Table> dynamic_update(const Db&, Table table)
		{
			return {table};
		}

}

#endif

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

#ifndef SQLPP_REMOVE_H
#define SQLPP_REMOVE_H

#include <sstream>
#include <sqlpp11/noop.h>
#include <sqlpp11/using.h>
#include <sqlpp11/where.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_remove.h>

namespace sqlpp
{
	template<
		typename Database,
		typename Table,
		typename Using = noop,
		typename Where = noop
		>
	struct remove_t;

	template<
		typename Database,
		typename Table,
		typename Using,
		typename Where
		>
		struct remove_t
		{
			static_assert(is_noop<Table>::value or is_table_t<Table>::value, "invalid 'Table' argument");
			static_assert(is_noop<Using>::value or is_using_t<Using>::value, "invalid 'Using' argument");
			static_assert(is_noop<Where>::value or is_where_t<Where>::value, "invalid 'Where' argument");

			template<typename UsingT> 
				using set_using_t = remove_t<Database, Table, UsingT, Where>;
			template<typename WhereT> 
				using set_where_t = remove_t<Database, Table, Using, WhereT>;

			using _parameter_tuple_t = std::tuple<Table, Using, Where>;
			using _parameter_list_t = typename make_parameter_list_t<remove_t>::type;

			template<typename... Tab>
				auto using_(Tab&&... tab)
				-> set_using_t<using_t<void, typename std::decay<Tab>::type...>>
				{
					static_assert(std::is_same<Using, noop>::value, "cannot call using() twice");
					static_assert(std::is_same<Where, noop>::value, "cannot call using() after where()");
					return {
							_table,
							{std::tuple<typename std::decay<Tab>::type...>{std::forward<Tab>(tab)...}},
							_where
					};
				}

			template<typename... Tab>
				auto dynamic_using_(Tab&&... tab)
				-> set_using_t<using_t<Database, typename std::decay<Tab>::type...>>
				{
					static_assert(std::is_same<Using, noop>::value, "cannot call using() twice");
					static_assert(std::is_same<Where, noop>::value, "cannot call using() after where()");
					return {
						_table,
							{std::tuple<typename std::decay<Tab>::type...>{std::forward<Tab>(tab)...}},
							_where
					};
				}

			template<typename Tab>
				remove_t& add_using_(Tab&& table)
				{
					static_assert(is_dynamic_t<Using>::value, "cannot call add_using() in a non-dynamic using");
					_using.add(std::forward<Tab>(table));

					return *this;
				}

			template<typename... Expr>
				auto where(Expr&&... expr)
				-> set_where_t<where_t<void, typename std::decay<Expr>::type...>>
				{
					static_assert(std::is_same<Where, noop>::value, "cannot call where() twice");
					return {
							_table,
							_using,
							{std::tuple<typename std::decay<Expr>::type...>{std::forward<Expr>(expr)...}},
					};
				}

			template<typename... Expr>
			auto dynamic_where(Expr&&... expr)
				-> set_where_t<where_t<Database, typename std::decay<Expr>::type...>>
				{
					static_assert(std::is_same<Where, noop>::value, "cannot call where() twice");
					return {
						_table, 
							_using, 
							{std::tuple<typename std::decay<Expr>::type...>{std::forward<Expr>(expr)...}},
					};
				}

			template<typename Expr>
				remove_t& add_where(Expr&& expr)
				{
					static_assert(is_dynamic_t<Where>::value, "cannot call add_where() in a non-dynamic where");

					_where.add(std::forward<Expr>(expr));

					return *this;
				}


			template<typename Db>
				const remove_t& serialize(std::ostream& os, Db& db) const
				{
					os << "DELETE FROM ";
					_table.serialize(os, db);
					_using.serialize(os, db);
					_where.serialize(os, db);
					return *this;
				}

			template<typename Db>
				remove_t& serialize(std::ostream& os, Db& db)
				{
					static_cast<const remove_t*>(this)->serialize(os, db);
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
					static_assert(_get_static_no_of_parameters() == 0, "cannot run remove directly with parameters, use prepare instead");
					return db.remove(*this);
				}

			template<typename Db>
				auto prepare(Db& db)
				-> prepared_remove_t<typename std::decay<Db>::type, remove_t>
				{
					_set_parameter_index(0);
					return {{}, db.prepare_remove(*this)};
				}

			size_t _set_parameter_index(size_t index)
			{
				index = set_parameter_index(_table, index);
				index = set_parameter_index(_using, index);
				index = set_parameter_index(_where, index);
				return index;
			}


			Table _table;
			Using _using;
			Where _where;
		};

	template<typename Table>
		constexpr remove_t<void, typename std::decay<Table>::type> remove_from(Table&& table)
		{
			return {std::forward<Table>(table)};
		}

	template<typename Db, typename Table>
		constexpr remove_t<typename std::decay<Db>::type, typename std::decay<Table>::type> dynamic_remove_from(Db&& db, Table&& table)
		{
			return {std::forward<Table>(table)};
		}

}

#endif

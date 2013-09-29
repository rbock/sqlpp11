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

namespace sqlpp
{
	template<
		typename Table,
		typename Using = noop,
		typename Where = noop
		>
	struct remove_t;

	template<
		typename Table,
		typename Using,
		typename Where
		>
		struct remove_t
		{
			static_assert(is_noop<Table>::value or is_table_t<Table>::value, "invalid 'Table' argument");
			static_assert(is_noop<Using>::value or is_using_t<Using>::value, "invalid 'Using' argument");
			static_assert(is_noop<Where>::value or is_where_t<Where>::value, "invalid 'Where' argument");

			template<typename... Tab> 
				using set_using_t = remove_t<Table, using_t<typename std::decay<Tab>::type...>, Where>;
			template<typename Expr> 
				using set_where_t = remove_t<Table, Using, where_t<typename std::decay<Expr>::type>>;

			template<typename... Tab>
				set_using_t<Tab...> using_(Tab&&... tab)
				{
					static_assert(std::is_same<Using, noop>::value, "cannot call using() twice");
					static_assert(std::is_same<Where, noop>::value, "cannot call using() after where()");
					return {
							_table,
							{std::tuple<typename std::decay<Tab>::type...>{std::forward<Tab>(tab)...}},
							_where
					};
				}

			template<typename Expr>
				set_where_t<Expr> where(Expr&& where)
				{
					static_assert(std::is_same<Where, noop>::value, "cannot call where() twice");
					return {
							_table,
							_using,
							{std::forward<Expr>(where)},
					};
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

			template<typename Db>
				std::size_t run(Db& db) const
				{
					std::ostringstream oss;
					serialize(oss, db);
					return db.remove(oss.str());
				}

			Table _table;
			Using _using;
			Where _where;
		};

	template<typename Table>
		constexpr remove_t<typename std::decay<Table>::type> remove_from(Table&& table)
		{
			return {std::forward<Table>(table)};
		}

}

#endif

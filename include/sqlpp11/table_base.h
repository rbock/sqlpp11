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

#ifndef SQLPP_TABLE_BASE_H
#define SQLPP_TABLE_BASE_H

#include <ostream>
#include <sqlpp11/alias.h>
#include <sqlpp11/no_value.h>
#include <sqlpp11/column.h>
#include <sqlpp11/detail/set.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/join.h>

namespace sqlpp
{
	template<typename Table, typename... ColumnSpec>
	struct table_base_t: public ColumnSpec::_name_t::template _member_t<column_t<Table, ColumnSpec>>...
	{
		using _table_set = detail::set<Table>; // Hint need a set here to be similar to a join (which always represents more than one table)
		using _all_columns = typename detail::make_set<column_t<Table, ColumnSpec>...>::type;
		static_assert(_all_columns::size::value, "at least one column required per table");
		using _required_insert_columns = typename detail::make_set_if<require_insert_t, column_t<Table, ColumnSpec>...>::type;
		using _all_of_t = std::tuple<column_t<Table, ColumnSpec>...>;

		using _is_table = tag_yes;

		template<typename T>
			join_t<inner_join_t, Table, typename std::decay<T>::type> join(T&& t)
			{
				return { *static_cast<const Table*>(this), std::forward<T>(t) };
			}

		template<typename AliasProvider>
			struct alias_t: public ColumnSpec::_name_t::template _member_t<column_t<AliasProvider, ColumnSpec>>...
		{
			using _is_table = tag_yes;
			using _table_set = detail::set<alias_t>;

			struct _value_type: Table::_value_type
			{
				using _is_expression = tag_no;
				using _is_named_expression = copy_type_trait<Table, is_value_t>;
				using _is_alias = tag_yes;
			};

			using _name_t = typename AliasProvider::_name_t;
			using _all_of_t = std::tuple<column_t<AliasProvider, ColumnSpec>...>;

			alias_t(const Table& table):
				_table(table)
			{}

			alias_t(Table&& table):
				_table(std::move(table))
			{}

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << "("; _table.serialize(os, db); os << ") AS " << _name_t::_get_name();
				}

			Table _table;
		};

		template<typename AliasProvider>
			alias_t<AliasProvider> as(const AliasProvider&) const
			{
				return {*static_cast<const Table*>(this)};
			}

		template<typename Db>
			void serialize(std::ostream& os, Db& db) const
			{
				static_cast<const Table*>(this)->serialize(os, db);
			}

	};

	template<typename Table>
	auto all_of(Table&& t) -> typename std::decay<Table>::type::_all_of_t
	{
		return {};
	}

}

#endif


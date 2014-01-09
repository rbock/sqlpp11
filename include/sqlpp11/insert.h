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
#include <sqlpp11/insert_list.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_insert.h>

#include <sqlpp11/detail/serialize_tuple.h>

namespace sqlpp
{
	template<
		typename Database = void,
		typename Table = noop,
		typename InsertList = noop
		>
	struct insert_t;

	template<
		typename Database,
		typename Table,
		typename InsertList
		>
		struct insert_t
		{
			static_assert(is_noop<Table>::value or is_table_t<Table>::value, "invalid 'Table' argument");
			static_assert(is_noop<InsertList>::value or is_insert_list_t<InsertList>::value, "invalid 'InsertList' argument");

			template<typename AssignmentT> 
				using set_insert_list_t = insert_t<Database, Table, AssignmentT>;

			using _parameter_tuple_t = std::tuple<Table, InsertList>;
			using _parameter_list_t = typename make_parameter_list_t<insert_t>::type;

			template<typename... Assignment>
				auto set(Assignment&&... assignment)
				-> set_insert_list_t<insert_list_t<void, must_not_insert_t, typename std::decay<Assignment>::type...>>
				{
					static_assert(std::is_same<InsertList, noop>::value, "cannot call set() twice");
					// FIXME:  Need to check if all required columns are set
					return {
							_table,
							insert_list_t<void, must_not_insert_t, typename std::decay<Assignment>::type...>{std::forward<Assignment>(assignment)...},
					};
				}

			template<typename... Assignment>
				auto dynamic_set(Assignment&&... assignment)
				-> set_insert_list_t<insert_list_t<Database, must_not_insert_t, typename std::decay<Assignment>::type...>>
				{
					static_assert(std::is_same<InsertList, noop>::value, "cannot call set() twice");
					return {
							_table,
							insert_list_t<Database, must_not_insert_t, typename std::decay<Assignment>::type...>{std::forward<Assignment>(assignment)...},
					};
				}

			template<typename Assignment>
				insert_t add_set(Assignment&& assignment)
				{
					static_assert(is_dynamic_t<InsertList>::value, "cannot call add_set() in a non-dynamic set");

					_insert_list.add(std::forward<Assignment>(assignment));

					return *this;
				}

			template<typename Db>
				const insert_t& serialize(std::ostream& os, Db& db) const
				{
					os << "INSERT INTO ";
					_table.serialize(os, db);
					if (is_noop<InsertList>::value)
					{
						detail::serialize_empty_insert_list(os, db);
					}
					else
					{
						_insert_list.serialize(os, db);
					}
					return *this;
				}

			template<typename Db>
				insert_t& serialize(std::ostream& os, Db& db)
				{
					static_cast<const insert_t*>(this)->serialize(os, db);
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
					constexpr bool calledSet = not is_noop<InsertList>::value;
					constexpr bool requireSet = Table::_required_insert_columns::size::value > 0;
					static_assert(calledSet or not requireSet, "calling set() required for given table");
					static_assert(_get_static_no_of_parameters() == 0, "cannot run insert directly with parameters, use prepare instead");
					return db.insert(*this);
				}

			template<typename Db>
				auto prepare(Db& db)
				-> prepared_insert_t<typename std::decay<Db>::type, insert_t>
				{
					constexpr bool calledSet = not is_noop<InsertList>::value;
					constexpr bool requireSet = Table::_required_insert_columns::size::value > 0;
					static_assert(calledSet or not requireSet, "calling set() required for given table");

					_set_parameter_index(0);
					return {{}, db.prepare_insert(*this)};
				}

			size_t _set_parameter_index(size_t index)
			{
				index = set_parameter_index(_table, index);
				index = set_parameter_index(_insert_list, index);
				return index;
			}


			Table _table;
			InsertList _insert_list;
		};

	template<typename Table>
		insert_t<void, typename std::decay<Table>::type> insert_into(Table&& table)
		{
			return {std::forward<Table>(table)};
		}

	template<typename Database, typename Table>
		insert_t<typename std::decay<Database>::type, typename std::decay<Table>::type> dynamic_insert_into(Database&& db, Table&& table)
		{
			return {std::forward<Table>(table)};
		}

}

#endif

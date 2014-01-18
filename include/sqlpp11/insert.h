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

#include <sqlpp11/type_traits.h>
#include <sqlpp11/prepared_insert.h>
#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/insert_list.h>
#include <sqlpp11/parameter_list.h>

namespace sqlpp
{

	template<
		typename Database = void,
		typename Table = vendor::noop,
		typename InsertList = vendor::noop
		>
		struct insert_t
		{
			static_assert(vendor::is_noop<Table>::value or is_table_t<Table>::value, "invalid 'Table' argument");
			static_assert(vendor::is_noop<InsertList>::value or is_insert_list_t<InsertList>::value, "invalid 'InsertList' argument");

			template<typename AssignmentT> 
				using set_insert_list_t = insert_t<Database, Table, AssignmentT>;
			using use_default_values_t = insert_t<Database, Table, vendor::insert_default_values_t>;

			using _parameter_tuple_t = std::tuple<Table, InsertList>;
			using _parameter_list_t = typename make_parameter_list_t<insert_t>::type;

			auto default_values()
				-> use_default_values_t
				{
					static_assert(std::is_same<InsertList, vendor::noop>::value, "cannot call default_values() after set() or default_values()");
					// FIXME:  Need to check if all required columns are set
					return {
							_table,
								{},
					};
				}

			template<typename... Assignment>
				auto set(Assignment&&... assignment)
				-> set_insert_list_t<vendor::insert_list_t<void, typename std::decay<Assignment>::type...>>
				{
					static_assert(std::is_same<InsertList, vendor::noop>::value, "cannot call set() after set() or default_values()");
					// FIXME:  Need to check if all required columns are set
					return {
							_table,
							vendor::insert_list_t<void, typename std::decay<Assignment>::type...>{std::forward<Assignment>(assignment)...},
					};
				}

			template<typename... Assignment>
				auto dynamic_set(Assignment&&... assignment)
				-> set_insert_list_t<vendor::insert_list_t<Database, typename std::decay<Assignment>::type...>>
				{
					static_assert(std::is_same<InsertList, vendor::noop>::value, "cannot call set() after set() or default_values()");
					return {
							_table,
							vendor::insert_list_t<Database, typename std::decay<Assignment>::type...>{std::forward<Assignment>(assignment)...},
					};
				}

			template<typename Assignment>
				insert_t add_set(Assignment&& assignment)
				{
					static_assert(is_dynamic_t<InsertList>::value, "cannot call add_set() in a non-dynamic set");

					_insert_list.add(std::forward<Assignment>(assignment));

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
					// FIXME: check if set or default_values() has ben called
					constexpr bool calledSet = not vendor::is_noop<InsertList>::value;
					constexpr bool requireSet = Table::_required_insert_columns::size::value > 0;
					static_assert(calledSet or not requireSet, "calling set() required for given table");
					static_assert(_get_static_no_of_parameters() == 0, "cannot run insert directly with parameters, use prepare instead");
					return db.insert(*this);
				}

			template<typename Db>
				auto prepare(Db& db) const
				-> prepared_insert_t<typename std::decay<Db>::type, insert_t>
				{
					constexpr bool calledSet = not vendor::is_noop<InsertList>::value;
					constexpr bool requireSet = Table::_required_insert_columns::size::value > 0;
					static_assert(calledSet or not requireSet, "calling set() required for given table");

					return {{}, db.prepare_insert(*this)};
				}

			Table _table;
			InsertList _insert_list;
		};

	namespace vendor
	{
		template<typename Context, typename Database, typename Table, typename InsertList>
			struct interpreter_t<Context, insert_t<Database, Table, InsertList>>
			{
				using T = insert_t<Database, Table, InsertList>;

				static Context& _(const T& t, Context& context)
				{
					context << "INSERT INTO ";
					interpret(t._table, context);
					interpret(t._insert_list, context);
					return context;
				}
			};
	}

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

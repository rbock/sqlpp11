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
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_insert.h>
#include <sqlpp11/default_value.h>
#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/single_table.h>
#include <sqlpp11/vendor/insert_value_list.h>
#include <sqlpp11/vendor/policy_update.h>

namespace sqlpp
{
	// INSERT
	template<typename Database = void,
			typename Table = vendor::no_single_table_t,
			typename InsertValueList = vendor::no_insert_value_list_t
			>
		struct insert_t
		{
			static_assert(Table::_table_set::template is_superset_of<typename InsertValueList::_table_set>::value, "columns do not match the table they are to be inserted into");

			using _database_t = Database;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
			using _table_set = typename Table::_table_set;

			template<typename Needle, typename Replacement, typename... Policies>
				struct _policies_update_impl
				{
					using type =  insert_t<Database, vendor::policy_update_t<Policies, Needle, Replacement>...>;
				};

			template<typename Needle, typename Replacement>
				using _policies_update_t = typename _policies_update_impl<Needle, Replacement, Table, InsertValueList>::type;

			using _parameter_tuple_t = std::tuple<Table, InsertValueList>;
			using _parameter_list_t = typename make_parameter_list_t<insert_t>::type;

			// Constructors
			insert_t()
			{}

			template<typename X>
				insert_t(X x, Table table):
					_table(table),
					_insert_value_list(x._insert_value_list)
			{}

			template<typename X>
				insert_t(X x, InsertValueList insert_value_list):
					_table(x._table),
					_insert_value_list(insert_value_list)
			{}

			insert_t(const insert_t&) = default;
			insert_t(insert_t&&) = default;
			insert_t& operator=(const insert_t&) = default;
			insert_t& operator=(insert_t&&) = default;
			~insert_t() = default;

			// type update functions
			auto default_values()
				-> _policies_update_t<vendor::no_insert_value_list_t, vendor::insert_default_values_t>
				{
					static_assert(is_noop_t<InsertValueList>::value, "cannot combine default_values() with other methods");
					return { *this, vendor::insert_default_values_t{} };
				}

			template<typename... Args>
				auto columns(Args... args)
				-> _policies_update_t<vendor::no_insert_value_list_t, vendor::column_list_t<Args...>>
				{
					static_assert(is_noop_t<InsertValueList>::value, "cannot combine columns() with other methods");
					return { *this, vendor::column_list_t<Args...>{args...} };
				}

			template<typename... Args>
				auto set(Args... args)
				-> _policies_update_t<vendor::no_insert_value_list_t, vendor::insert_list_t<void, Args...>>
				{
					static_assert(is_noop_t<InsertValueList>::value, "cannot combine set() with other methods");
					return { *this, vendor::insert_list_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_set(Args... args)
				-> _policies_update_t<vendor::no_insert_value_list_t, vendor::insert_list_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<InsertValueList>::value, "cannot combine dynamic_set() with other methods");
					static_assert(_is_dynamic::value, "dynamic_set must not be called in a static statement");
					return { *this, vendor::insert_list_t<_database_t, Args...>{args...} };
				}

			// value adding methods
			template<typename... Args>
				void add_set(Args... args)
				{
					static_assert(is_insert_list_t<InsertValueList>::value, "cannot call add_set() before dynamic_set()");
					static_assert(is_dynamic_t<InsertValueList>::value, "cannot call add_set() before dynamic_set()");
					return _insert_value_list.add_set(*this, args...);
				}

			template<typename... Args>
				void add_values(Args... args)
				{
					static_assert(is_column_list_t<InsertValueList>::value, "cannot call add_values() before columns()");
					return _insert_value_list.add_values(args...);
				}

			// run and prepare
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
					static_assert(_get_static_no_of_parameters() == 0, "cannot run insert directly with parameters, use prepare instead");
					return db.insert(*this);
				}

			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_insert_t<Db, insert_t>
				{
					return {{}, db.prepare_insert(*this)};
				}

			InsertValueList _insert_value_list;
			Table _table;
		};

	namespace vendor
	{
		template<typename Context, typename Database, typename... Policies>
			struct interpreter_t<Context, insert_t<Database, Policies...>>
			{
				using T = insert_t<Database, Policies...>;

				static Context& _(const T& t, Context& context)
				{
					context << "INSERT INTO ";
					interpret(t._table, context);
					interpret(t._insert_value_list, context);
					return context;
				}
			};
	}

	template<typename Table>
		constexpr auto insert_into(Table table)
		-> insert_t<void, vendor::single_table_t<void, Table>>
		{
			return { insert_t<void>(), vendor::single_table_t<void, Table>{table} };
		}

	template<typename Database, typename Table>
		constexpr auto  dynamic_insert_into(const Database&, Table table)
		-> insert_t<Database, vendor::single_table_t<void, Table>>
		{
			return { insert_t<Database>(), vendor::single_table_t<void, Table>{table} };
		}

}

#endif

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
#include <sqlpp11/vendor/column_list.h>
#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/insert_list.h>
#include <sqlpp11/vendor/assignment.h>
#include <sqlpp11/vendor/insert_value_list.h>

namespace sqlpp
{

	template<
		typename Database = void,
		typename Table = vendor::noop,
		typename InsertList = vendor::noop,
		typename ColumnList = vendor::noop,
		typename ValueList = vendor::insert_value_list_t<vendor::noop>
		>
		struct insert_t
		{
			static_assert(vendor::is_noop<Table>::value or is_table_t<Table>::value, "invalid 'Table' argument");
			static_assert(vendor::is_noop<InsertList>::value or is_insert_list_t<InsertList>::value, "invalid 'InsertList' argument");
			static_assert(vendor::is_noop<ColumnList>::value or is_column_list_t<ColumnList>::value, "invalid 'ColumnList' argument");
			static_assert(vendor::is_noop<ValueList>::value or is_insert_value_list_t<ValueList>::value, "invalid 'ValueList' argument");

			using use_default_values_t = insert_t<Database, Table, vendor::insert_default_values_t>;
			template<typename AssignmentT> 
				using set_insert_list_t = insert_t<Database, Table, AssignmentT>;
			template<typename ColumnT, typename ValueT> 
				using set_column_value_list_t = insert_t<Database, Table, InsertList, ColumnT, ValueT>;

			using _parameter_tuple_t = std::tuple<Table, InsertList>;
			using _parameter_list_t = typename make_parameter_list_t<insert_t>::type;

			auto default_values()
				-> use_default_values_t
				{
					static_assert(std::is_same<InsertList, vendor::noop>::value, "cannot call default_values() after set() or default_values()");
					static_assert(vendor::is_noop<ColumnList>::value, "cannot call default_values() after columns()");
					static_assert(Table::_required_insert_columns::size::value == 0, "cannot use default_values, because some columns are configured to require values");
					return {
							_table,
								{},
								_column_list,
								_value_list,
					};
				}

			template<typename... Assignment>
				auto set(Assignment... assignment)
				-> set_insert_list_t<vendor::insert_list_t<void, Assignment...>>
				{
					static_assert(std::is_same<InsertList, vendor::noop>::value, "cannot call set() after set() or default_values()");
					static_assert(vendor::is_noop<ColumnList>::value, "cannot call set() after columns()");
					// FIXME:  Need to check if all required columns are set
					return {
							_table,
							vendor::insert_list_t<void, Assignment...>{assignment...},
							_column_list,
							_value_list,
					};
				}

			template<typename... Assignment>
				auto dynamic_set(Assignment... assignment)
				-> set_insert_list_t<vendor::insert_list_t<Database, Assignment...>>
				{
					static_assert(std::is_same<InsertList, vendor::noop>::value, "cannot call set() after set() or default_values()");
					static_assert(vendor::is_noop<ColumnList>::value, "cannot call set() after columns()");
					return {
							_table,
							vendor::insert_list_t<Database, Assignment...>{assignment...},
							_column_list,
							_value_list,
					};
				}

			template<typename Assignment>
				insert_t add_set(Assignment assignment)
				{
					static_assert(is_dynamic_t<InsertList>::value, "cannot call add_set() in a non-dynamic set");

					_insert_list.add(assignment);

					return *this;
				}

			template<typename... Column>
				auto columns(Column... columns)
				-> set_column_value_list_t<vendor::column_list_t<Column...>, vendor::insert_value_list_t<vendor::insert_value_t<Column>...>>
				{
					static_assert(vendor::is_noop<ColumnList>::value, "cannot call columns() twice");
					static_assert(vendor::is_noop<InsertList>::value, "cannot call columns() after set() or dynamic_set()");
					// FIXME:  Need to check if all required columns are set

					return {
						_table,
						_insert_list,
						{std::tuple<vendor::simple_column_t<Column>...>{{columns}...}},
						vendor::insert_value_list_t<vendor::insert_value_t<Column>...>{},
					};
				}

			template<typename... Value>
				insert_t& add_values(Value... values)
				{
					static_assert(is_insert_value_list_t<ValueList>::value, "cannot call add_values() before columns()");
					_value_list.add(typename ValueList::_value_tuple_t{values...});
					return *this;
				};

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
					static_assert(not (vendor::is_noop<InsertList>::value and vendor::is_noop<ColumnList>::value) , "calling set() or default_values()");
					static_assert(_get_static_no_of_parameters() == 0, "cannot run insert directly with parameters, use prepare instead");
					return db.insert(*this);
				}

			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_insert_t<Db, insert_t>
				{
					constexpr bool calledSet = not vendor::is_noop<InsertList>::value;
					constexpr bool requireSet = Table::_required_insert_columns::size::value > 0;
					static_assert(calledSet or not requireSet, "calling set() required for given table");

					return {{}, db.prepare_insert(*this)};
				}

			Table _table;
			InsertList _insert_list;
			ColumnList _column_list;
			ValueList _value_list;
		};

	namespace vendor
	{
		template<typename Context, typename Database, typename Table, typename InsertList, typename ColumnList, typename ValueList>
			struct interpreter_t<Context, insert_t<Database, Table, InsertList, ColumnList, ValueList>>
			{
				using T = insert_t<Database, Table, InsertList, ColumnList, ValueList>;

				static Context& _(const T& t, Context& context)
				{
					if (not vendor::is_noop<decltype(t._insert_list)>::value)
					{
						context << "INSERT INTO ";
						interpret(t._table, context);
						interpret(t._insert_list, context);
					}
					else if (not t._value_list.empty())
					{
						context << "INSERT INTO ";
						interpret(t._table, context);
						interpret(t._column_list, context);
						interpret(t._value_list, context);
					}
					else
					{
						context << "# empty insert";
					}
					return context;
				}
			};
	}

	template<typename Table>
		insert_t<void, Table> insert_into(Table table)
		{
			return {table};
		}

	template<typename Database, typename Table>
		insert_t<Database, Table> dynamic_insert_into(const Database& db, Table table)
		{
			return {table};
		}

}

#endif

/*
 * Copyright (c) 2013-2014, Roland Bock
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
#include <sqlpp11/vendor/single_table.h>
#include <sqlpp11/vendor/update_list.h>
#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/where.h>
#include <sqlpp11/vendor/policy_update.h>
#include <sqlpp11/detail/arg_selector.h>

namespace sqlpp
{
	namespace detail
	{
		template<
			typename Table,
			typename UpdateList,
			typename Where
			>
		struct check_update_t
		{
			static constexpr bool value = true;
		};
	}

	template<typename Database = void, 
			typename Table = vendor::no_single_table_t,
			typename UpdateList = vendor::no_update_list_t,
			typename Where = vendor::no_where_t>
		struct update_t
		{
			static_assert(::sqlpp::detail::is_superset_of<typename Table::_table_set, typename UpdateList::_table_set>::value, "updated columns do not match the table");
			static_assert(::sqlpp::detail::is_superset_of<typename Table::_table_set, typename Where::_table_set>::value, "where condition does not match updated table");

			using _database_t = Database;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

			template<typename Needle, typename Replacement, typename... Policies>
				struct _policies_update_impl
				{
					using type =  update_t<Database, vendor::policy_update_t<Policies, Needle, Replacement>...>;
				};

			template<typename Needle, typename Replacement>
				using _policies_update_t = typename _policies_update_impl<Needle, Replacement, Table, UpdateList, Where>::type;

			using _parameter_tuple_t = std::tuple<Table, UpdateList, Where>;
			using _parameter_list_t = typename make_parameter_list_t<update_t>::type;

			// Constructors
			update_t()
			{}

			template<typename Statement, typename T>
				update_t(Statement s, T t):
					_table(detail::arg_selector<Table>::_(s._table, t)),
					_update_list(detail::arg_selector<UpdateList>::_(s._update_list, t)),
					_where(detail::arg_selector<Where>::_(s._where, t))
			{}

			update_t(const update_t&) = default;
			update_t(update_t&&) = default;
			update_t& operator=(const update_t&) = default;
			update_t& operator=(update_t&&) = default;
			~update_t() = default;

			// type update functions
			template<typename... Args>
				auto set(Args... args)
				-> _policies_update_t<vendor::no_update_list_t, vendor::update_list_t<void, Args...>>
				{
					static_assert(is_noop_t<UpdateList>::value, "cannot call set()/dynamic_set() twice");
					return { *this, vendor::update_list_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_set(Args... args)
				-> _policies_update_t<vendor::no_update_list_t, vendor::update_list_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<UpdateList>::value, "cannot call set()/dynamic_set() twice");
					static_assert(_is_dynamic::value, "dynamic_set must not be called in a static statement");
					return { *this, vendor::update_list_t<_database_t, Args...>{args...} };
				}

			template<typename... Args>
				auto where(Args... args)
				-> _policies_update_t<vendor::no_where_t, vendor::where_t<void, Args...>>
				{
					static_assert(is_noop_t<Where>::value, "cannot call where()/dynamic_where() twice");
					return { *this, vendor::where_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_where(Args... args)
				-> _policies_update_t<vendor::no_where_t, vendor::where_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<Where>::value, "cannot call where()/dynamic_where() twice");
					static_assert(not std::is_same<_database_t, void>::value, "dynamic_where must not be called in a static statement");
					return { *this, vendor::where_t<_database_t, Args...>{args...} };
				}

			// value adding methods
			template<typename... Args>
				void add_set(Args... args)
				{
					static_assert(is_update_list_t<UpdateList>::value, "cannot call add_set() before dynamic_set()");
					static_assert(is_dynamic_t<UpdateList>::value, "cannot call add_set() before dynamic_set()");
					return _update_list.add_set(*this, args...);
				}

			template<typename... Args>
				void add_where(Args... args)
				{
					static_assert(is_where_t<Where>::value, "cannot call add_where() before dynamic_where()");
					static_assert(is_dynamic_t<Where>::value, "cannot call add_where() before dynamic_where()");
					return _where.add_where(*this, args...);
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
					static_assert(_get_static_no_of_parameters() == 0, "cannot run update directly with parameters, use prepare instead");
					return db.update(*this);
				}

			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_update_t<Db, update_t>
				{
					return {{}, db.prepare_update(*this)};
				}

			Table _table;
			UpdateList _update_list;
			Where _where;
		};

	namespace vendor
	{
		template<typename Context, typename Database, typename... Policies>
			struct serializer_t<Context, update_t<Database, Policies...>>
			{
				using T = update_t<Database, Policies...>;

				static Context& _(const T& t, Context& context)
				{
					context << "UPDATE ";
					serialize(t._table, context);
					serialize(t._update_list, context);
					serialize(t._where, context);
					return context;
				}
			};
	}

	template<typename Table>
		constexpr auto update(Table table)
		-> update_t<void, vendor::single_table_t<void, Table>>
		{
			return { update_t<void>(), vendor::single_table_t<void, Table>{table} };
		}

	template<typename Database, typename Table>
		constexpr auto  dynamic_update(const Database&, Table table)
		-> update_t<Database, vendor::single_table_t<void, Table>>
		{
			return { update_t<Database>(), vendor::single_table_t<void, Table>{table} };
		}

}

#endif

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
	template<typename Db,
			typename... Policies
				>
		struct update_t;

	namespace detail
	{
		template<typename Db = void, 
			typename Table = vendor::no_single_table_t,
			typename UpdateList = vendor::no_update_list_t,
			typename Where = vendor::no_where_t
				>
			struct update_policies_t
			{
				using _database_t = Db;
				using _table_t = Table;
				using _update_list_t = UpdateList;
				using _where_t = Where;

				using _statement_t = update_t<Db, Table, UpdateList, Where>;

				struct _methods_t:
					public _update_list_t::template _methods_t<update_policies_t>,
					public _where_t::template _methods_t<update_policies_t>
				{};

				template<typename Needle, typename Replacement, typename... Policies>
					struct _policies_update_t
					{
						using type =  update_t<Db, vendor::policy_update_t<Policies, Needle, Replacement>...>;
					};

				template<typename Needle, typename Replacement>
					using _new_statement_t = typename _policies_update_t<Needle, Replacement, Table, UpdateList, Where>::type;
			};
	}

	template<typename Db, 
			typename... Policies
				>
		struct update_t:
			public detail::update_policies_t<Db, Policies...>::_methods_t
		{
			using _policies_t = typename detail::update_policies_t<Db, Policies...>;
			using _database_t = typename _policies_t::_database_t;
			using _table_t = typename _policies_t::_table_t;
			using _update_list_t = typename _policies_t::_update_list_t;
			using _where_t = typename _policies_t::_where_t;

			using _is_dynamic = typename std::conditional<std::is_same<_database_t, void>::value, std::false_type, std::true_type>::type;

			using _parameter_tuple_t = std::tuple<Policies...>;
			using _parameter_list_t = typename make_parameter_list_t<update_t>::type;

			static_assert(::sqlpp::detail::is_superset_of<typename _table_t::_table_set, typename _update_list_t::_table_set>::value, "updated columns do not match the table");
			static_assert(::sqlpp::detail::is_superset_of<typename _table_t::_table_set, typename _where_t::_table_set>::value, "where condition does not match updated table");

			// Constructors
			update_t()
			{}

			template<typename Statement, typename T>
				update_t(Statement s, T t):
					_table(detail::arg_selector<_table_t>::_(s._table, t)),
					_update_list(detail::arg_selector<_update_list_t>::_(s._update_list, t)),
					_where(detail::arg_selector<_where_t>::_(s._where, t))
			{}

			update_t(const update_t&) = default;
			update_t(update_t&&) = default;
			update_t& operator=(const update_t&) = default;
			update_t& operator=(update_t&&) = default;
			~update_t() = default;

			// run and prepare
			static constexpr size_t _get_static_no_of_parameters()
			{
				return _parameter_list_t::size::value;
			}

			size_t _get_no_of_parameters() const
			{
				return _parameter_list_t::size::value;
			}

			template<typename Database>
				std::size_t _run(Database& db) const
				{
					static_assert(_get_static_no_of_parameters() == 0, "cannot run update directly with parameters, use prepare instead");
					return db.update(*this);
				}

			template<typename Database>
				auto _prepare(Database& db) const
				-> prepared_update_t<Database, update_t>
				{
					return {{}, db.prepare_update(*this)};
				}

			_table_t _table;
			_update_list_t _update_list;
			_where_t _where;
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

	template<typename Database, typename... Policies>
		using make_update_t = typename detail::update_policies_t<Database, Policies...>::_statement_t;

	template<typename Table>
		constexpr auto update(Table table)
		-> make_update_t<void, vendor::single_table_t<void, Table>>
		{
			return { update_t<void>(), vendor::single_table_t<void, Table>{table} };
		}

	template<typename Database, typename Table>
		constexpr auto  dynamic_update(const Database&, Table table)
		-> make_update_t<Database, vendor::single_table_t<void, Table>>
		{
			return { update_t<Database>(), vendor::single_table_t<void, Table>{table} };
		}

}

#endif

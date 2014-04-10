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

#ifndef SQLPP_REMOVE_H
#define SQLPP_REMOVE_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_remove.h>
#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/single_table.h>
#include <sqlpp11/vendor/using.h>
#include <sqlpp11/vendor/where.h>
#include <sqlpp11/vendor/policy_update.h>
#include <sqlpp11/detail/arg_selector.h>

namespace sqlpp
{
	template<typename Db,
			typename... Policies
				>
		struct remove_t;

	namespace detail
	{
		template<typename Db = void,
			typename Table = vendor::no_single_table_t,
			typename Using = vendor::no_using_t,
			typename Where = vendor::no_where_t
				>
			struct remove_policies_t 
			{
				using _database_t = Db;
				using _table_t = Table;
				using _using_t = Using;
				using _where_t = Where;

				using _statement_t = remove_t<Db, Table, Using, Where>;

				struct _methods_t:
					public _using_t::template _methods_t<remove_policies_t>,
					public _where_t::template _methods_t<remove_policies_t>
				{};

				template<typename Needle, typename Replacement, typename... Policies>
					struct _policies_update_t
					{
						using type =  remove_t<Db, vendor::policy_update_t<Policies, Needle, Replacement>...>;
					};

				template<typename Needle, typename Replacement>
					using _new_statement_t = typename _policies_update_t<Needle, Replacement, Table, Using, Where>::type;

			};
	}

	// REMOVE
	template<typename Db,
			typename... Policies
				>
		struct remove_t:
			public detail::remove_policies_t<Db, Policies...>::_methods_t
		{
			using _policies_t = typename detail::remove_policies_t<Db, Policies...>;
			using _database_t = typename _policies_t::_database_t;
			using _table_t = typename _policies_t::_table_t;
			using _using_t = typename _policies_t::_using_t;
			using _where_t = typename _policies_t::_where_t;

			using _is_dynamic = typename std::conditional<std::is_same<_database_t, void>::value, std::false_type, std::true_type>::type;

			using _parameter_tuple_t = std::tuple<Policies...>;
			using _parameter_list_t = typename make_parameter_list_t<remove_t>::type;

			// Constructors
			remove_t()
			{}

			template<typename Statement, typename T>
				remove_t(Statement s, T t):
					_table(detail::arg_selector<_table_t>::_(s._table, t)),
					_using(detail::arg_selector<_using_t>::_(s._using, t)),
					_where(detail::arg_selector<_where_t>::_(s._where, t))
			{}

			remove_t(const remove_t&) = default;
			remove_t(remove_t&&) = default;
			remove_t& operator=(const remove_t&) = default;
			remove_t& operator=(remove_t&&) = default;
			~remove_t() = default;

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
					static_assert(_get_static_no_of_parameters() == 0, "cannot run remove directly with parameters, use prepare instead");
					static_assert(is_where_t<_where_t>::value, "cannot run remove without having a where condition, use .where(true) to remove all rows");
					return db.remove(*this);
				}

			template<typename Database>
				auto _prepare(Database& db) const
				-> prepared_remove_t<Database, remove_t>
				{
					static_assert(is_where_t<_where_t>::value, "cannot run remove without having a where condition, use .where(true) to remove all rows");
					return {{}, db.prepare_remove(*this)};
				}

			_table_t _table;
			_using_t _using;
			_where_t _where;
		};

	namespace vendor
	{
		template<typename Context, typename Database, typename... Policies>
			struct serializer_t<Context, remove_t<Database, Policies...>>
			{
				using T = remove_t<Database, Policies...>;

				static Context& _(const T& t, Context& context)
				{
					context << "DELETE FROM ";
					serialize(t._table, context);
					serialize(t._using, context);
					serialize(t._where, context);
					return context;
				}
			};
	}

	template<typename Database, typename... Policies>
		using make_remove_t = typename detail::remove_policies_t<Database, Policies...>::_statement_t;

	template<typename Table>
		constexpr auto remove_from(Table table)
		-> make_remove_t<void, vendor::single_table_t<void, Table>>
		{
			return { make_remove_t<void>(), vendor::single_table_t<void, Table>{table} };
		}

	template<typename Database, typename Table>
		constexpr auto  dynamic_remove_from(const Database&, Table table)
		-> make_remove_t<Database, vendor::single_table_t<void, Table>>
		{
			return { make_remove_t<Database>(), vendor::single_table_t<void, Table>{table} };
		}

}

#endif

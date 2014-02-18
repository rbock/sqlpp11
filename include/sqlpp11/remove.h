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

#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_remove.h>
#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/single_table.h>
#include <sqlpp11/vendor/using.h>
#include <sqlpp11/vendor/where.h>
#include <sqlpp11/vendor/policy_update.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename Table, typename Using, typename Where>
			struct check_remove_t
			{
				static_assert(is_where_t<Where>::value, "cannot run remove without having a where condition, use .where(true) to remove all rows");
				static constexpr bool value = true;
			};
	}

	// REMOVE
	template<typename Database,
	 		typename Table = vendor::no_single_table_t,
	 		typename Using = vendor::no_using_t,
	 		typename Where = vendor::no_where_t
				>
		struct remove_t
		{
			using _database_t = Database;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

			template<typename Needle, typename Replacement, typename... Policies>
				struct _policies_update_impl
				{
					using type =  remove_t<Database, vendor::policy_update_t<Policies, Needle, Replacement>...>;
				};

			template<typename Needle, typename Replacement>
				using _policies_update_t = typename _policies_update_impl<Needle, Replacement, Table, Using, Where>::type;

			using _parameter_tuple_t = std::tuple<Table, Using, Where>;
			using _parameter_list_t = typename make_parameter_list_t<remove_t>::type;

			// Constructors
			remove_t()
			{}

			template<typename X>
				remove_t(X x, Table table):
					_table(table),
					_using(x._using),
					_where(x._where)
			{}

			template<typename X>
				remove_t(X x, Using using_):
					_table(x._table),
					_using(using_),
					_where(x._where)
			{}

			template<typename X>
				remove_t(X x, Where where):
					_table(x._table),
					_using(x._using),
					_where(where)
			{}

			remove_t(const remove_t&) = default;
			remove_t(remove_t&&) = default;
			remove_t& operator=(const remove_t&) = default;
			remove_t& operator=(remove_t&&) = default;
			~remove_t() = default;

			// type update functions
			template<typename... Args>
				auto using_(Args... args)
				-> _policies_update_t<vendor::no_using_t, vendor::using_t<void, Args...>>
				{
					static_assert(is_noop_t<Using>::value, "cannot call using_()/dynamic_using() twice");
					return { *this, vendor::using_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_using(Args... args)
				-> _policies_update_t<vendor::no_using_t, vendor::using_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<Using>::value, "cannot call using_()/dynamic_using() twice");
					static_assert(not std::is_same<_database_t, void>::value, "dynamic_using must not be called in a static statement");
					return { *this, vendor::using_t<_database_t, Args...>{args...} };
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
				void add_using(Args... args)
				{
					static_assert(is_using_t<Using>::value, "cannot call add_using() before dynamic_using()");
					static_assert(is_dynamic_t<Using>::value, "cannot call add_using() before dynamic_using()");
					return _using.add_using(args...);
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
					static_assert(_get_static_no_of_parameters() == 0, "cannot run remove directly with parameters, use prepare instead");
					//static_assert(detail::check_remove_t<Policies...>::value, "Cannot run this remove expression");
					return db.remove(*this);
				}

			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_remove_t<Db, remove_t>
				{
					//static_assert(detail::check_remove_t<Policies...>::value, "Cannot run this remove expression");
					return {{}, db.prepare_remove(*this)};
				}

			Table _table;
			Using _using;
			Where _where;
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

	template<typename Table>
		constexpr auto remove_from(Table table)
		-> remove_t<void, vendor::single_table_t<void, Table>>
		{
			return { remove_t<void>(), vendor::single_table_t<void, Table>{table} };
		}

	template<typename Database, typename Table>
		constexpr auto  dynamic_remove_from(const Database&, Table table)
		-> remove_t<Database, vendor::single_table_t<void, Table>>
		{
			return { remove_t<Database>(), vendor::single_table_t<void, Table>{table} };
		}

}

#endif

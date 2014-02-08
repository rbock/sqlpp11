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
#include <sqlpp11/vendor/crtp_wrapper.h>
#include <sqlpp11/vendor/policy.h>
#include <sqlpp11/vendor/policy_update.h>

namespace sqlpp
{
	namespace detail
	{
		template<
			typename Table,
			typename InsertValueList
				>
				struct check_insert_t
				{
					//static_assert(not (vendor::is_noop<InsertList>::value and vendor::is_noop<ColumnList>::value) , "calling set() or default_values()");
					static constexpr bool value = true;
				};
	}

	template<typename Database, typename... Policies>
		struct insert_t: public vendor::policy_t<Policies>..., public vendor::crtp_wrapper_t<insert_t<Database, Policies...>, Policies>...
		{
			template<typename Needle, typename Replacement>
				using _policy_update_t = insert_t<Database, vendor::policy_update_t<Policies, Needle, Replacement>...>;

			using _database_t = Database;
			using _parameter_tuple_t = std::tuple<Policies...>;
			using _parameter_list_t = typename make_parameter_list_t<insert_t>::type;

			insert_t()
			{}

			template<typename Whatever>
				insert_t(insert_t i, Whatever whatever):
					vendor::policy_t<Policies>(i, whatever)...
			{}

			template<typename Insert, typename Whatever>
				insert_t(Insert i, Whatever whatever):
					vendor::policy_t<Policies>(i, whatever)...
			{}

			insert_t(const insert_t&) = default;
			insert_t(insert_t&&) = default;
			insert_t& operator=(const insert_t&) = default;
			insert_t& operator=(insert_t&&) = default;
			~insert_t() = default;

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
					static_assert(detail::check_insert_t<Policies...>::value, "Cannot run this insert expression");
					return db.insert(*this);
				}

			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_insert_t<Db, insert_t>
				{
					static_assert(detail::check_insert_t<Policies...>::value, "Cannot prepare this insert expression");
					return {{}, db.prepare_insert(*this)};
				}
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
					interpret(t._single_table(), context);
					interpret(t._insert_value_list(), context);
					return context;
				}
			};
	}

	template<typename Database>
		using blank_insert_t = insert_t<Database, vendor::no_single_table_t, vendor::no_insert_value_list_t>;

	template<typename Table>
		constexpr auto insert_into(Table table)
		-> insert_t<void, vendor::single_table_t<void, Table>, vendor::no_insert_value_list_t>
		{
			return { blank_insert_t<void>(), vendor::single_table_t<void, Table>{table} };
		}

	template<typename Database, typename Table>
		constexpr auto  dynamic_insert_into(const Database&, Table table)
		-> insert_t<Database, vendor::single_table_t<void, Table>, vendor::no_insert_value_list_t>
		{
			return { blank_insert_t<Database>(), vendor::single_table_t<void, Table>{table} };
		}

}

#endif

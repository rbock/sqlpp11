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

#ifndef SQLPP_UPDATE_H
#define SQLPP_UPDATE_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_update.h>
#include <sqlpp11/vendor/single_table.h>
#include <sqlpp11/vendor/update_list.h>
#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/where.h>
#include <sqlpp11/vendor/crtp_wrapper.h>
#include <sqlpp11/vendor/policy.h>
#include <sqlpp11/vendor/policy_update.h>

namespace sqlpp
{
	namespace detail
	{
		template<
			typename Table,
			typename Assignments,
			typename Where
			>
		struct check_update_t
		{
			static constexpr bool value = true;
		};
	}

	template<typename Database, typename... Policies>
		struct update_t: public vendor::policy_t<Policies>..., public vendor::crtp_wrapper_t<update_t<Database, Policies...>, Policies>...
		{
			template<typename Needle, typename Replacement>
				using _policy_update_t = update_t<Database, vendor::policy_update_t<Policies, Needle, Replacement>...>;

			using _database_t = Database;
			using _parameter_tuple_t = std::tuple<Policies...>;
			using _parameter_list_t = typename make_parameter_list_t<update_t>::type;

			update_t()
			{}

			template<typename Whatever>
				update_t(update_t r, Whatever whatever):
					vendor::policy_t<Policies>(r, whatever)...
			{}

			template<typename Remove, typename Whatever>
				update_t(Remove r, Whatever whatever):
					vendor::policy_t<Policies>(r, whatever)...
			{}

			update_t(const update_t&) = default;
			update_t(update_t&&) = default;
			update_t& operator=(const update_t&) = default;
			update_t& operator=(update_t&&) = default;
			~update_t() = default;

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
					static_assert(detail::check_update_t<Policies...>::value, "Cannot run this update expression");
					return db.update(*this);
				}

			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_update_t<Database, update_t>
				{
					static_assert(detail::check_update_t<Policies...>::value, "Cannot run this update expression");
					return {{}, db.prepare_update(*this)};
				}
		};

	namespace vendor
	{
		template<typename Context, typename Database, typename... Policies>
			struct interpreter_t<Context, update_t<Database, Policies...>>
			{
				using T = update_t<Database, Policies...>;

				static Context& _(const T& t, Context& context)
				{
					context << "UPDATE ";
					interpret(t._single_table(), context);
					interpret(t._update_list(), context);
					interpret(t._where(), context);
					return context;
				}
			};
	}

	template<typename Database>
		using blank_update_t = update_t<Database, vendor::no_single_table_t, vendor::no_update_list_t, vendor::no_where_t>;

	template<typename Table>
		constexpr auto update(Table table)
		-> update_t<void, vendor::single_table_t<void, Table>, vendor::no_update_list_t, vendor::no_where_t>
		{
			return { blank_update_t<void>(), vendor::single_table_t<void, Table>{table} };
		}

	template<typename Database, typename Table>
		constexpr auto  dynamic_update(const Database&, Table table)
		-> update_t<Database, vendor::single_table_t<void, Table>, vendor::no_update_list_t, vendor::no_where_t>
		{
			return { blank_update_t<Database>(), vendor::single_table_t<void, Table>{table} };
		}

}

#endif

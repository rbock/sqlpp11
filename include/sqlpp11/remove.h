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
#include <sqlpp11/vendor/crtp_wrapper.h>
#include <sqlpp11/vendor/policy.h>
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

	template<typename Database, typename... Policies>
		struct remove_t: public vendor::policy_t<Policies>..., public vendor::crtp_wrapper_t<remove_t<Database, Policies...>, Policies>...
		{
			template<typename Needle, typename Replacement>
				using _policy_update_t = remove_t<Database, vendor::policy_update_t<Policies, Needle, Replacement>...>;

			using _database_t = Database;
			using _parameter_tuple_t = std::tuple<Policies...>;
			using _parameter_list_t = typename make_parameter_list_t<remove_t>::type;

			remove_t()
			{}

			template<typename Whatever>
				remove_t(remove_t r, Whatever whatever):
					vendor::policy_t<Policies>(r, whatever)...
			{}

			template<typename Remove, typename Whatever>
				remove_t(Remove r, Whatever whatever):
					vendor::policy_t<Policies>(r, whatever)...
			{}

			remove_t(const remove_t&) = default;
			remove_t(remove_t&&) = default;
			remove_t& operator=(const remove_t&) = default;
			remove_t& operator=(remove_t&&) = default;
			~remove_t() = default;

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
					static_assert(detail::check_remove_t<Policies...>::value, "Cannot run this remove expression");
					return db.remove(*this);
				}

			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_remove_t<Database, remove_t>
				{
					static_assert(detail::check_remove_t<Policies...>::value, "Cannot run this remove expression");
					return {{}, db.prepare_remove(*this)};
				}
		};

	namespace vendor
	{
		template<typename Context, typename Database, typename... Policies>
			struct interpreter_t<Context, remove_t<Database, Policies...>>
			{
				using T = remove_t<Database, Policies...>;

				static Context& _(const T& t, Context& context)
				{
					context << "DELETE FROM";
					interpret(t._single_table(), context);
					interpret(t._using(), context);
					interpret(t._where(), context);
					return context;
				}
			};
	}

	template<typename Database>
		using blank_remove_t = remove_t<Database, vendor::no_single_table_t, vendor::no_using_t, vendor::no_where_t>;

	template<typename Table>
		constexpr auto remove_from(Table table)
		-> remove_t<void, vendor::single_table_t<void, Table>, vendor::no_using_t, vendor::no_where_t>
		{
			return { blank_remove_t<void>(), vendor::single_table_t<void, Table>{table} };
		}

	template<typename Database, typename Table>
		constexpr auto  dynamic_remove_from(const Database&, Table table)
		-> remove_t<Database, vendor::single_table_t<void, Table>, vendor::no_using_t, vendor::no_where_t>
		{
			return { blank_remove_t<Database>(), vendor::single_table_t<void, Table>{table} };
		}

}

#endif

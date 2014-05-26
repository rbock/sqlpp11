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
#warning: need to use another table provider, since delete can be used with several tables
#include <sqlpp11/vendor/single_table.h>
#include <sqlpp11/vendor/extra_tables.h>
#include <sqlpp11/vendor/using.h>
#include <sqlpp11/vendor/where.h>
#include <sqlpp11/vendor/policy_update.h>

#include <sqlpp11/detail/get_last.h>
#include <sqlpp11/detail/pick_arg.h>

namespace sqlpp
{
	template<typename Db, typename... Policies>
		struct remove_t;

	namespace detail
	{
		template<typename Db = void, typename... Policies>
			struct remove_policies_t 
			{
				using _database_t = Db;
				using _statement_t = remove_t<Db, Policies...>;

				struct _methods_t: public Policies::template _methods_t<remove_policies_t>...
				{};

				template<typename Needle, typename Replacement>
					struct _policies_update_t
					{
						static_assert(detail::is_element_of<Needle, make_type_set_t<Policies...>>::value, "policies update for non-policy class detected");
						using type =  remove_t<Db, vendor::policy_update_t<Policies, Needle, Replacement>...>;
					};

				template<typename Needle, typename Replacement>
					using _new_statement_t = typename _policies_update_t<Needle, Replacement>::type;

				using _all_required_tables = detail::make_joined_set_t<required_tables_of<Policies>...>;
				using _all_provided_tables = detail::make_joined_set_t<provided_tables_of<Policies>...>;
				using _all_extra_tables = detail::make_joined_set_t<extra_tables_of<Policies>...>;

				using _known_tables = detail::make_joined_set_t<_all_provided_tables, _all_extra_tables>;

				template<typename Expression>
					using _no_unknown_tables = detail::is_subset_of<required_tables_of<Expression>, _known_tables>;

				// The tables not covered by the from.
				using _required_tables = detail::make_difference_set_t<
					_all_required_tables,
					_all_provided_tables // Hint: extra_tables are not used here because they are just a helper for dynamic .add_*()
							>;

				using _traits = make_traits<no_value_t>; // FIXME

				struct _recursive_traits
				{
					using _parameters = std::tuple<>; // FIXME
					using _required_tables = _required_tables;
					using _provided_tables = detail::type_set<>;
					using _extra_tables = detail::type_set<>;
				};
			};
	}

	// REMOVE
	template<typename Db, typename... Policies>
		struct remove_t:
			public Policies...,
			public detail::remove_policies_t<Db, Policies...>::_methods_t
		{
			using _policies_t = typename detail::remove_policies_t<Db, Policies...>;
			using _database_t = typename _policies_t::_database_t;

			using _is_dynamic = typename std::conditional<std::is_same<_database_t, void>::value, std::false_type, std::true_type>::type;

			using _parameter_list_t = typename make_parameter_list_t<remove_t>::type;

			// Constructors
			constexpr remove_t()
			{}

			template<typename Statement, typename Term>
				remove_t(Statement statement, Term term):
					Policies(detail::pick_arg<Policies>(statement, term))...
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

			void _check_consistency() const
			{
#warning reactivate checks
#if 0
				static_assert(is_where_t<_where_t>::value, "cannot run update without having a where condition, use .where(true) to update all rows");

				// FIXME: Read more details about what is allowed and what not in SQL DELETE
				static_assert(is_table_subset_of_table<_where_t>::value, "where requires additional tables");
#endif
			}

			template<typename Database>
				std::size_t _run(Database& db) const
				{
					_check_consistency();

					static_assert(_get_static_no_of_parameters() == 0, "cannot run remove directly with parameters, use prepare instead");
					return db.remove(*this);
				}

			template<typename Database>
				auto _prepare(Database& db) const
				-> prepared_remove_t<Database, remove_t>
				{
					_check_consistency();

					return {{}, db.prepare_remove(*this)};
				}
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
					using swallow = int[]; 
					(void) swallow{(serialize(static_cast<const Policies&>(t), context), 0)...};
					return context;
				}
			};
	}

	template<typename Database>
		using blank_remove_t = remove_t<Database,
			vendor::no_single_table_t,
			vendor::no_using_t,
			vendor::no_extra_tables_t,
			vendor::no_where_t>;

	constexpr auto remove()
		-> blank_remove_t<void>
		{
			return { blank_remove_t<void>() };
		}

	template<typename Table>
		constexpr auto remove_from(Table table)
		-> decltype(blank_remove_t<void>().from(table))
		{
			return { blank_remove_t<void>().from(table) };
		}

	template<typename Database>
		constexpr auto  dynamic_remove(const Database&)
		-> decltype(blank_remove_t<Database>())
		{
			return { blank_remove_t<Database>() };
		}

	template<typename Database, typename Table>
		constexpr auto  dynamic_remove_from(const Database&, Table table)
		-> decltype(blank_remove_t<Database>().from(table))
		{
			return { blank_remove_t<Database>().from(table) };
		}
}

#endif

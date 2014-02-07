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

#ifndef SQLPP_FROM_H
#define SQLPP_FROM_H

#include <sqlpp11/select_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/interpretable_list.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/detail/logic.h>
#include <sqlpp11/vendor/policy_update.h>
#include <sqlpp11/vendor/crtp_wrapper.h>

namespace sqlpp
{
	namespace vendor
	{
		// FROM
		template<typename Database, typename... Tables>
			struct from_t
			{
				using _is_from = std::true_type;
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

				static_assert(_is_dynamic::value or sizeof...(Tables), "at least one table or join argument required in from()");

				static_assert(not ::sqlpp::detail::has_duplicates<Tables...>::value, "at least one duplicate argument detected in from()");

				static_assert(::sqlpp::detail::and_t<is_table_t, Tables...>::value, "at least one argument is not a table or join in from()");

				// FIXME: Joins contain two tables. This is not being dealt with at the moment when looking at duplicates, for instance

				from_t(Tables... tables):
					_tables(tables...)
				{}

				from_t(const from_t&) = default;
				from_t(from_t&&) = default;
				from_t& operator=(const from_t&) = default;
				from_t& operator=(from_t&&) = default;
				~from_t() = default;

				template<typename Table>
					void add_from(Table table)
					{
						static_assert(_is_dynamic::value, "add_from can only be called for dynamic_from");
						static_assert(is_table_t<Table>::value, "from arguments require to be tables or joins");
						_dynamic_tables.emplace_back(table);
					}

				const from_t& _from() const { return *this; }
				std::tuple<Tables...> _tables;
				vendor::interpretable_list_t<Database> _dynamic_tables;
			};

		struct no_from_t
		{
			using _is_from = std::true_type;
			const no_from_t& _from() const { return *this; }
		};

		// CRTP Wrappers
		template<typename Derived, typename Database, typename... Args>
			struct crtp_wrapper_t<Derived, from_t<Database, Args...>>
			{
			};

		template<typename Derived>
			struct crtp_wrapper_t<Derived, no_from_t>
			{
				template<typename... Args>
					auto from(Args... args)
					-> vendor::update_policies_t<Derived, no_from_t, from_t<void, Args...>>
					{
						return { static_cast<Derived&>(*this), from_t<void, Args...>(args...) };
					}

				template<typename... Args>
					auto dynamic_from(Args... args)
					-> vendor::update_policies_t<Derived, no_from_t, from_t<get_database_t<Derived>, Args...>>
					{
						static_assert(not std::is_same<get_database_t<Derived>, void>::value, "dynamic_from must not be called in a static statement");
						return { static_cast<Derived&>(*this), from_t<get_database_t<Derived>, Args...>(args...) };
					}
			};

		// Interpreters
		template<typename Context, typename Database, typename... Tables>
			struct interpreter_t<Context, from_t<Database, Tables...>>
			{
				using T = from_t<Database, Tables...>;

				static Context& _(const T& t, Context& context)
				{
					if (sizeof...(Tables) == 0 and t._dynamic_tables.empty())
						return context;
					context << " FROM ";
					interpret_tuple(t._tables, ',', context);
					if (sizeof...(Tables) and not t._dynamic_tables.empty())
						context << ',';
					interpret_list(t._dynamic_tables, ',', context);
					return context;
				}
			};

		template<typename Context>
			struct interpreter_t<Context, no_from_t>
			{
				using T = no_from_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

	}
}

#endif

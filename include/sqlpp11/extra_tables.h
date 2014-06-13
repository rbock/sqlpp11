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

#ifndef SQLPP_EXTRA_TABLES_H
#define SQLPP_EXTRA_TABLES_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/logic.h>
#include <sqlpp11/policy_update.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename... Tables>
			struct extra_tables_data_t
			{
				extra_tables_data_t()
				{}

				extra_tables_data_t(const extra_tables_data_t&) = default;
				extra_tables_data_t(extra_tables_data_t&&) = default;
				extra_tables_data_t& operator=(const extra_tables_data_t&) = default;
				extra_tables_data_t& operator=(extra_tables_data_t&&) = default;
				~extra_tables_data_t() = default;

			};

		// EXTRA_TABLES
		template<typename... Tables>
			struct extra_tables_t
			{
				using _traits = make_traits<no_value_t, ::sqlpp::tag::extra_tables>;
				struct _recursive_traits
				{
					using _parameters = std::tuple<>;
					using _required_tables = ::sqlpp::detail::type_set<>;
					using _provided_tables = ::sqlpp::detail::type_set<>;
					using _extra_tables = ::sqlpp::detail::type_set<Tables...>;
				};

				using _recursive_traits = make_recursive_traits<Tables...>;

				// FIXME: extra_tables must not require tables!

				static_assert(sizeof...(Tables), "at least one table or join argument required in extra_tables()");
				static_assert(not ::sqlpp::detail::has_duplicates<Tables...>::value, "at least one duplicate argument detected in extra_tables()");
				static_assert(::sqlpp::detail::all_t<is_table_t<Tables>::value...>::value, "at least one argument is not a table or join in extra_tables()");

				// Data
				using _data_t = extra_tables_data_t<Tables...>;

				// Member implementation with data and methods
				template <typename Policies>
					struct _impl_t
				{
					_data_t _data;
				};

				// Member template for adding the named member to a statement
				template<typename Policies>
					struct _member_t
					{
						using _data_t = extra_tables_data_t<Tables...>;

						_impl_t<Policies> extra_tables;
						_impl_t<Policies>& operator()() { return extra_tables; }
						const _impl_t<Policies>& operator()() const { return extra_tables; }

						template<typename T>
							static auto _get_member(T t) -> decltype(t.extra_tables)
							{
								return t.extra_tables;
							}
					};

				// Additional methods for the statement
				template<typename Policies>
					struct _methods_t
					{
					};
			};

		// NO EXTRA TABLES YET
		struct no_extra_tables_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::noop>;
			using _recursive_traits = make_recursive_traits<>;

			// Data
			using _data_t = no_data_t;

			// Member implementation with data and methods
			template<typename Policies>
				struct _impl_t
				{
					_data_t _data;
				};

			// Member template for adding the named member to a statement
			template<typename Policies>
				struct _member_t
				{
					using _data_t = no_data_t;

					_impl_t<Policies> no_extra_tables;
					_impl_t<Policies>& operator()() { return no_extra_tables; }
					const _impl_t<Policies>& operator()() const { return no_extra_tables; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.no_extra_tables)
						{
							return t.no_extra_tables;
						}
				};

			template<typename Policies>
				struct _methods_t
				{
					template<typename T>
					using _new_statement_t = typename Policies::template _new_statement_t<no_extra_tables_t, T>;

					template<typename... Args>
						auto extra_tables(Args...)
						-> _new_statement_t<extra_tables_t<Args...>>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), extra_tables_data_t<Args...>{} };
						}
				};
		};

		// Interpreters
		template<typename Context, typename Database, typename... Tables>
			struct serializer_t<Context, extra_tables_data_t<Database, Tables...>>
			{
				using T = extra_tables_data_t<Database, Tables...>;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

	}
}

#endif

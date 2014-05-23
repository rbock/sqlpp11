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

#ifndef SQLPP_USING_H
#define SQLPP_USING_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/interpretable_list.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/vendor/policy_update.h>

namespace sqlpp
{
	namespace vendor
	{
		// USING
		template<typename Database, typename... Tables>
			struct using_t
			{
				using _traits = make_traits<no_value_t, ::sqlpp::tag::using_>;
				using _recursive_traits = make_recursive_traits<Tables...>;

				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

				static_assert(_is_dynamic::value or sizeof...(Tables), "at least one table argument required in using()");

				static_assert(not ::sqlpp::detail::has_duplicates<Tables...>::value, "at least one duplicate argument detected in using()");

				static_assert(::sqlpp::detail::all_t<is_table_t<Tables>::value...>::value, "at least one argument is not an table in using()");

				using_t& _using() { return *this; }

				using_t(Tables... tables):
					_tables(tables...)
				{}

				using_t(const using_t&) = default;
				using_t(using_t&&) = default;
				using_t& operator=(const using_t&) = default;
				using_t& operator=(using_t&&) = default;
				~using_t() = default;

				template<typename Policies>
					struct _methods_t
					{
						template<typename Table>
							void add_using(Table table)
							{
								static_assert(_is_dynamic::value, "add_using must not be called for static using()");
								static_assert(is_table_t<Table>::value, "invalid table argument in add_using()");

								using ok = ::sqlpp::detail::all_t<_is_dynamic::value, is_table_t<Table>::value>;

								_add_using_impl(table, ok()); // dispatch to prevent compile messages after the static_assert
							}

					private:
						template<typename Table>
							void _add_using_impl(Table table, const std::true_type&)
							{
								return static_cast<typename Policies::_statement_t*>(this)->_using()._dynamic_tables.emplace_back(table);
							}

						template<typename Table>
							void _add_using_impl(Table table, const std::false_type&);
					};


				std::tuple<Tables...> _tables;
				vendor::interpretable_list_t<Database> _dynamic_tables;
			};

		struct no_using_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::where>;
			using _recursive_traits = make_recursive_traits<>;

			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
					using _new_statement_t = typename Policies::template _new_statement_t<no_using_t, T>;

					template<typename... Args>
						auto using_(Args... args)
						-> _new_statement_t<using_t<void, Args...>>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), using_t<void, Args...>{args...} };
						}

					template<typename... Args>
						auto dynamic_using(Args... args)
						-> _new_statement_t<using_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_using must not be called in a static statement");
							return { *static_cast<typename Policies::_statement_t*>(this), vendor::using_t<_database_t, Args...>{args...} };
						}
				};
		};

		// Interpreters
		template<typename Context, typename Database, typename... Tables>
			struct serializer_t<Context, using_t<Database, Tables...>>
			{
				using T = using_t<Database, Tables...>;

				static Context& _(const T& t, Context& context)
				{
					if (sizeof...(Tables) == 0 and t._dynamic_tables.empty())
						return context;
					context << " USING ";
					interpret_tuple(t._tables, ',', context);
					if (sizeof...(Tables) and not t._dynamic_tables.empty())
						context << ',';
					interpret_list(t._dynamic_tables, ',', context);
					return context;
				}
			};

		template<typename Context>
			struct serializer_t<Context, no_using_t>
			{
				using T = no_using_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};
	}
}

#endif

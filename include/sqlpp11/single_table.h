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

#ifndef SQLPP_SINGLE_TABLE_H
#define SQLPP_SINGLE_TABLE_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/no_value.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/prepared_insert.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
	// A SINGLE TABLE DATA
	template<typename Database, typename Table>
		struct single_table_data_t
		{
			single_table_data_t(Table table):
				_table(table)
			{}

			single_table_data_t(const single_table_data_t&) = default;
			single_table_data_t(single_table_data_t&&) = default;
			single_table_data_t& operator=(const single_table_data_t&) = default;
			single_table_data_t& operator=(single_table_data_t&&) = default;
			~single_table_data_t() = default;

			Table _table;
		};

	// A SINGLE TABLE
	template<typename Database, typename Table>
		struct single_table_t
		{
			using _traits = make_traits<no_value_t, tag::is_single_table>;
			using _recursive_traits = make_recursive_traits<Table>;

			static_assert(is_table_t<Table>::value, "argument has to be a table");
			static_assert(required_tables_of<Table>::size::value == 0, "table depends on another table");

			using _data_t = single_table_data_t<Database, Table>;

			struct _name_t {};

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
					using _data_t = single_table_data_t<Database, Table>;

					_impl_t<Policies> from;
					_impl_t<Policies>& operator()() { return from; }
					const _impl_t<Policies>& operator()() const { return from; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.from)
						{
							return t.from;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
					static void _check_consistency() {}
				};

		};

	// NO INTO YET
	struct no_single_table_t
	{
		using _traits = make_traits<no_value_t, tag::is_noop>;
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

				_impl_t<Policies> no_from;
				_impl_t<Policies>& operator()() { return no_from; }
				const _impl_t<Policies>& operator()() const { return no_from; }

				template<typename T>
					static auto _get_member(T t) -> decltype(t.no_from)
					{
						return t.no_from;
					}
			};

		template<typename Policies>
			struct _methods_t
			{
				using _database_t = typename Policies::_database_t;
				template<typename T>
					using _new_statement_t = new_statement<Policies, no_single_table_t, T>;

				static void _check_consistency() {}

				template<typename... Args>
					auto from(Args... args) const
					-> _new_statement_t<single_table_t<void, Args...>>
					{
						return { static_cast<const derived_statement_t<Policies>&>(*this), single_table_data_t<void, Args...>{args...} };
					}
			};
	};

	// Interpreters
	template<typename Context, typename Database, typename Table>
		struct serializer_t<Context, single_table_data_t<Database, Table>>
		{
			using T = single_table_data_t<Database, Table>;

			static Context& _(const T& t, Context& context)
			{
				serialize(t._table, context);
				return context;
			}
		};

}

#endif

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

#ifndef SQLPP_FROM_H
#define SQLPP_FROM_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/logic.h>
#include <sqlpp11/detail/sum.h>
#include <sqlpp11/policy_update.h>

namespace sqlpp
{
	// FROM DATA
	template<typename Database, typename... Tables>
		struct from_data_t
		{
			from_data_t(Tables... tables):
				_tables(tables...)
			{}

			from_data_t(const from_data_t&) = default;
			from_data_t(from_data_t&&) = default;
			from_data_t& operator=(const from_data_t&) = default;
			from_data_t& operator=(from_data_t&&) = default;
			~from_data_t() = default;

			std::tuple<Tables...> _tables;
			interpretable_list_t<Database> _dynamic_tables;
		};

	// FROM
	template<typename Database, typename... Tables>
		struct from_t
		{
			using _traits = make_traits<no_value_t, tag::is_from>;
			using _recursive_traits = make_recursive_traits<Tables...>;
			using _is_dynamic = is_database<Database>;

			// Data
			using _data_t = from_data_t<Database, Tables...>;

			// Member implementation with data and methods
			template<typename Policies>
				struct _impl_t
				{
					template<typename Table>
						void add(Table table)
						{
							static_assert(_is_dynamic::value, "from::add() must not be called for static from()");
							static_assert(is_table_t<Table>::value, "invalid table argument in from::add()");
							using _known_tables = detail::make_joined_set_t<provided_tables_of<Tables>...>; // Hint: Joins contain more than one table
							using _known_table_names = detail::transform_set_t<name_of, _known_tables>;
							static_assert(not detail::is_element_of<typename Table::_alias_t, _known_table_names>::value, "Must not use the same table name twice in from()");
							using _serialize_check = sqlpp::serialize_check_t<typename Database::_serializer_context_t, Table>;
							_serialize_check::_();

							using ok = logic::all_t<_is_dynamic::value, is_table_t<Table>::value, _serialize_check::type::value>;

							_add_impl(table, ok()); // dispatch to prevent compile messages after the static_assert
						}

				private:
					template<typename Table>
						void _add_impl(Table table, const std::true_type&)
						{
							return _data._dynamic_tables.emplace_back(table);
						}

					template<typename Table>
						void _add_impl(Table table, const std::false_type&);

				public:
					_data_t _data;
				};

			// Base template to be inherited by the statement
			template<typename Policies>
				struct _base_t
				{
					using _data_t = from_data_t<Database, Tables...>;

					_impl_t<Policies> from;
					_impl_t<Policies>& operator()() { return from; }
					const _impl_t<Policies>& operator()() const { return from; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.from)
						{
							return t.from;
						}

					// FIXME: We might want to check if we have too many tables define in the FROM
					using _consistency_check = consistent_t;
				};
		};

	struct no_from_t
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

		// Base template to be inherited by the statement
		template<typename Policies>
			struct _base_t
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

				using _database_t = typename Policies::_database_t;

				template<typename... T>
					using _check = logic::all_t<is_table_t<T>::value...>;

				template<typename Check, typename T>
					using _new_statement_t = new_statement_t<Check::value, Policies, no_from_t, T>;

				using _consistency_check = consistent_t;

				template<typename... Tables>
					auto from(Tables... tables) const
					-> _new_statement_t<_check<Tables...>, from_t<void, Tables...>>
					{
						static_assert(_check<Tables...>::value, "at least one argument is not a table or join in from()");
						static_assert(sizeof...(Tables), "at least one table or join argument required in from()");
						return _from_impl<void>(_check<Tables...>{}, tables...);
					}

				template<typename... Tables>
					auto dynamic_from(Tables... tables) const
					-> _new_statement_t<_check<Tables...>, from_t<_database_t, Tables...>>
					{
						static_assert(not std::is_same<_database_t, void>::value, "dynamic_from must not be called in a static statement");
						static_assert(_check<Tables...>::value, "at least one argument is not a table or join in from()");
						return _from_impl<_database_t>(_check<Tables...>{}, tables...);
					}

			private:
				template<typename Database, typename... Tables>
					auto _from_impl(const std::false_type&, Tables... tables) const
					-> bad_statement;

				template<typename Database, typename... Tables>
					auto _from_impl(const std::true_type&, Tables... tables) const
					-> _new_statement_t<std::true_type, from_t<Database, Tables...>>
					{
						static_assert(required_tables_of<from_t<Database, Tables...>>::size::value == 0, "at least one table depends on another table in from()");

						static constexpr std::size_t _number_of_tables = detail::sum(provided_tables_of<Tables>::size::value...);
						using _unique_tables = detail::make_joined_set_t<provided_tables_of<Tables>...>;
						using _unique_table_names = detail::transform_set_t<name_of, _unique_tables>;
						static_assert(_number_of_tables == _unique_tables::size::value, "at least one duplicate table detected in from()");
						static_assert(_number_of_tables == _unique_table_names::size::value, "at least one duplicate table name detected in from()");

						return { static_cast<const derived_statement_t<Policies>&>(*this), from_data_t<Database, Tables...>{tables...} };
					}

			};
	};

	// Interpreters
	template<typename Context, typename Database, typename... Tables>
		struct serializer_t<Context, from_data_t<Database, Tables...>>
		{
			using _serialize_check = serialize_check_of<Context, Tables...>;
			using T = from_data_t<Database, Tables...>;

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

	template<typename... T>
		auto from(T&&... t) -> decltype(statement_t<void, no_from_t>().from(std::forward<T>(t)...))
		{
			return statement_t<void, no_from_t>().from(std::forward<T>(t)...);
		}
}

#endif

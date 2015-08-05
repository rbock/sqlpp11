/*
 * Copyright (c) 2013-2015, Roland Bock
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
#include <sqlpp11/logic.h>
#include <sqlpp11/policy_update.h>

namespace sqlpp
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
			using _traits = make_traits<no_value_t, tag::is_extra_tables>;
			using _nodes = detail::type_vector<>;
			using _required_ctes = detail::make_joined_set_t<required_ctes_of<Tables>...>;
			using _extra_tables = detail::type_set<Tables...>;

			// Data
			using _data_t = extra_tables_data_t<Tables...>;

			// Member implementation with data and methods
			template <typename Policies>
				struct _impl_t
				{
					_data_t _data;
				};

			// Base template to be inherited by the statement
			template<typename Policies>
				struct _base_t
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

					using _consistency_check = consistent_t;
				};
		};

	// NO EXTRA TABLES YET
	struct no_extra_tables_t
	{
		using _traits = make_traits<no_value_t, tag::is_noop>;
		using _nodes = detail::type_vector<>;

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

				_impl_t<Policies> no_extra_tables;
				_impl_t<Policies>& operator()() { return no_extra_tables; }
				const _impl_t<Policies>& operator()() const { return no_extra_tables; }

				template<typename T>
					static auto _get_member(T t) -> decltype(t.no_extra_tables)
					{
						return t.no_extra_tables;
					}

				template<typename Check, typename T>
					using _new_statement_t = new_statement_t<Check::value, Policies, no_extra_tables_t, T>;

				template<typename... T>
					using _check = logic::all_t<is_table_t<T>::value...>;

				using _consistency_check = consistent_t;

				template<typename... Tables>
					auto extra_tables(Tables... tables) const
					-> _new_statement_t<_check<Tables...>, extra_tables_t<Tables...>>
					{
						static_assert(_check<Tables...>::value, "at least one argument is not a table or join in extra_tables()");

						return _extra_tables_impl<void>(_check<Tables...>{}, tables...);
					}

			private:
				template<typename Database, typename... Tables>
					auto _extra_tables_impl(const std::false_type&, Tables... tables) const
					-> bad_statement;

				template<typename Database, typename... Tables>
					auto _extra_tables_impl(const std::true_type&, Tables...) const
					-> _new_statement_t<std::true_type, extra_tables_t<Tables...>>
					{
						static_assert(required_tables_of<extra_tables_t<Tables...>>::size::value == 0, "at least one table depends on another table in extra_tables()");

						static constexpr std::size_t _number_of_tables = detail::sum(provided_tables_of<Tables>::size::value...);
						using _unique_tables = detail::make_joined_set_t<provided_tables_of<Tables>...>;
						using _unique_table_names = detail::transform_set_t<name_of, _unique_tables>;
						static_assert(_number_of_tables == _unique_tables::size::value, "at least one duplicate table detected in extra_tables()");
						static_assert(_number_of_tables == _unique_table_names::size::value, "at least one duplicate table name detected in extra_tables()");

						return { static_cast<const derived_statement_t<Policies>&>(*this), extra_tables_data_t<Tables...>{} };
					}
			};
	};

	// Interpreters
	template<typename Context, typename Database, typename... Tables>
		struct serializer_t<Context, extra_tables_data_t<Database, Tables...>>
		{
			using _serialize_check = serialize_check_of<Context, Tables...>;
			using T = extra_tables_data_t<Database, Tables...>;

			static Context& _(const T&, Context& context)
			{
				return context;
			}
		};

}

#endif

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

#ifndef SQLPP_SELECT_H
#define SQLPP_SELECT_H

#include <sqlpp11/result.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_select.h>

#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/select_flag_list.h>
#include <sqlpp11/vendor/select_column_list.h>
#include <sqlpp11/vendor/from.h>
#include <sqlpp11/vendor/extra_tables.h>
#include <sqlpp11/vendor/where.h>
#include <sqlpp11/vendor/group_by.h>
#include <sqlpp11/vendor/having.h>
#include <sqlpp11/vendor/order_by.h>
#include <sqlpp11/vendor/limit.h>
#include <sqlpp11/vendor/offset.h>
#include <sqlpp11/vendor/expression.h>
#include <sqlpp11/vendor/serializer.h>
#include <sqlpp11/vendor/wrong.h>
#include <sqlpp11/vendor/policy_update.h>

#include <sqlpp11/detail/arg_selector.h>
#include <sqlpp11/detail/get_last.h>

namespace sqlpp
{
	template<typename Db, typename... Policies>
		struct select_t;

#warning STEPS:
#warning do the same with insert, update and remove
#warning deal with different return types in the connector (select could be a single value, update could be a range of rows)
	namespace detail
	{
		template<typename Db = void, typename... Policies>
			struct select_policies_t
			{
				using _database_t = Db;
				using _statement_t = select_t<Db, Policies...>;

				struct _methods_t: public Policies::template _methods_t<select_policies_t>...
				{};

				template<typename Needle, typename Replacement>
					struct _policies_update_t
					{
						static_assert(detail::is_element_of<Needle, make_type_set_t<Policies...>>::value, "policies update for non-policy class detected");
						using type =  select_t<Db, vendor::policy_update_t<Policies, Needle, Replacement>...>;
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

				using _result_provider = detail::get_last_if<is_return_value_t, vendor::no_select_column_list_t, Policies...>;

				// A select can be used as a pseudo table if
				//   - at least one column is selected
				//   - the select is complete (leaks no tables)
				using _can_be_used_as_table = typename std::conditional<
					is_select_column_list_t<_result_provider>::value and _required_tables::size::value == 0,
					std::true_type,
					std::false_type
					>::type;

				using _value_type = typename std::conditional<
					detail::none_t<is_missing_t<Policies>::value...>::value,
					value_type_of<_result_provider>,
					no_value_t // if a required statement part is missing (columns in a select), then the statement cannot be used as a value
						>::type;

				using _traits = make_traits<_value_type>;

				struct _recursive_traits
				{
					using _parameters = std::tuple<>; // FIXME
					using _required_tables = _required_tables;
					using _provided_tables = detail::type_set<>;
					using _extra_tables = detail::type_set<>;
				};

			};
	}

	namespace detail
	{
		template<typename Target, typename Statement, typename Term>
			Target pick_arg_impl(Statement statement, Term term, const std::true_type&)
			{
				return term;
			};

		template<typename Target, typename Statement, typename Term>
			Target pick_arg_impl(Statement statement, Term term, const std::false_type&)
			{
				return static_cast<Target>(statement);
			};

		template<typename Target, typename Statement, typename Term>
			Target pick_arg(Statement statement, Term term)
			{
				return pick_arg_impl<Target>(statement, term, std::is_same<Target, Term>());
			};
	}

	// SELECT
	template<typename Db,
			typename... Policies
				>
		struct select_t:
			public Policies...,
			public detail::select_policies_t<Db, Policies...>::_value_type::template expression_operators<select_t<Db, Policies...>>,
			public detail::select_policies_t<Db, Policies...>::_methods_t
	{
			using _policies_t = typename detail::select_policies_t<Db, Policies...>;

			using _traits = make_traits<value_type_of<_policies_t>, ::sqlpp::tag::select>;

			using _recursive_traits = typename _policies_t::_recursive_traits;

			using _database_t = Db;
			using _is_dynamic = typename std::conditional<std::is_same<_database_t, void>::value, std::false_type, std::true_type>::type;

#warning replace _column_list_t by a more generic name
			using _column_list_t = typename _policies_t::_result_provider;

			using _parameter_tuple_t = std::tuple<Policies...>;
			using _parameter_list_t = typename make_parameter_list_t<select_t>::type;
			
			template<typename Database>
				using _result_row_t = typename _column_list_t::template _result_row_t<Database>;
			using _dynamic_names_t = typename _column_list_t::_dynamic_names_t;

			using _is_select = std::true_type;
			using _requires_braces = std::true_type;

			using _value_type = typename detail::select_policies_t<Db, Policies...>::_value_type;
			using _name_t = typename _column_list_t::_name_t;

			// Constructors
			select_t()
			{}

			template<typename Statement, typename Term>
				select_t(Statement statement, Term term):
					Policies(detail::pick_arg<Policies>(statement, term))...
			{}

			select_t(const select_t& r) = default;
			select_t(select_t&& r) = default;
			select_t& operator=(const select_t& r) = default;
			select_t& operator=(select_t&& r) = default;
			~select_t() = default;

			// PseudoTable
			template<typename AliasProvider>
				struct _pseudo_table_t
				{
					using table = typename _column_list_t::template _pseudo_table_t<select_t>;
					using alias = typename table::template _alias_t<AliasProvider>;
				};

			template<typename AliasProvider>
				typename _pseudo_table_t<AliasProvider>::alias as(const AliasProvider& aliasProvider) const
				{
					static_assert(_policies_t::_can_be_used_as_table::value, "select cannot be used as table, incomplete from()");
					return typename _pseudo_table_t<AliasProvider>::table(
							*this).as(aliasProvider);
				}

			const _dynamic_names_t& get_dynamic_names() const
			{
				return static_cast<const _column_list_t&>(*this)._dynamic_columns._dynamic_expression_names;
			}

			static constexpr size_t _get_static_no_of_parameters()
			{
				return _parameter_list_t::size::value;
			}

			size_t _get_no_of_parameters() const
			{
				return _parameter_list_t::size::value;
			}

			size_t get_no_of_result_columns() const
			{
				return _column_list_t::static_size() + get_dynamic_names().size();
			}

			void _check_consistency() const
			{
#warning check for missing terms here, and for missing tables
				static_assert(not required_tables_of<_policies_t>::size::value, "one sub expression contains tables which are not in the from()");
			}

			// Execute
			template<typename Database>
				auto _run(Database& db) const
				-> result_t<decltype(db.select(*this)), _result_row_t<Database>>
				{
					_check_consistency();
					static_assert(_get_static_no_of_parameters() == 0, "cannot run select directly with parameters, use prepare instead");

					return {db.select(*this), get_dynamic_names()};
				}

			// Prepare
			template<typename Database>
				auto _prepare(Database& db) const
				-> prepared_select_t<Database, select_t>
				{
					_check_consistency();

					return {{}, get_dynamic_names(), db.prepare_select(*this)};
				}
		};

	namespace vendor
	{
		template<typename Context, typename Database, typename... Policies>
			struct serializer_t<Context, select_t<Database, Policies...>>
			{
				using T = select_t<Database, Policies...>;

				static Context& _(const T& t, Context& context)
				{
					context << "SELECT ";

					using swallow = int[]; 
					(void) swallow{(serialize(static_cast<const Policies&>(t), context), 0)...};

					return context;
				}
			};
	}

	template<typename Database>
		using blank_select_t = select_t<Database,
			vendor::no_select_flag_list_t, 
			vendor::no_select_column_list_t, 
			vendor::no_from_t,
			vendor::no_extra_tables_t,
			vendor::no_where_t, 
			vendor::no_group_by_t, 
			vendor::no_having_t,
			vendor::no_order_by_t, 
			vendor::no_limit_t, 
			vendor::no_offset_t>;


	blank_select_t<void> select() // FIXME: These should be constexpr
	{
		return { };
	}

	template<typename... Columns>
		auto select(Columns... columns)
		-> decltype(blank_select_t<void>().columns(detail::make_select_column_list_t<void, Columns...>(columns...)))
		{
			return blank_select_t<void>().columns(detail::make_select_column_list_t<void, Columns...>(columns...));
		}

	template<typename Database>
		blank_select_t<Database> dynamic_select(const Database&)
		{
			return { };
		}

	template<typename Database, typename... Columns>
		auto dynamic_select(const Database&, Columns... columns)
		-> decltype(blank_select_t<Database>().columns(detail::make_select_column_list_t<void, Columns...>(columns...)))
		{
			return blank_select_t<Database>().columns(detail::make_select_column_list_t<void, Columns...>(columns...));
		}

}
#endif

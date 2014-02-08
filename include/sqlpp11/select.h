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

#ifndef SQLPP_SELECT_H
#define SQLPP_SELECT_H

#include <sqlpp11/result.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_select.h>

#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/select_flag_list.h>
#include <sqlpp11/vendor/select_column_list.h>
#include <sqlpp11/vendor/from.h>
#include <sqlpp11/vendor/where.h>
#include <sqlpp11/vendor/group_by.h>
#include <sqlpp11/vendor/having.h>
#include <sqlpp11/vendor/order_by.h>
#include <sqlpp11/vendor/limit.h>
#include <sqlpp11/vendor/offset.h>
#include <sqlpp11/vendor/expression.h>
#include <sqlpp11/vendor/interpreter.h>
#include <sqlpp11/vendor/wrong.h>
#include <sqlpp11/vendor/crtp_wrapper.h>
#include <sqlpp11/vendor/policy.h>
#include <sqlpp11/vendor/policy_update.h>

#include <sqlpp11/detail/copy_tuple_args.h>

namespace sqlpp
{
	namespace detail
	{
		template<
			typename FlagList, 
			typename ColumnList, 
			typename From,
			typename Where, 
			typename GroupBy, 
			typename Having,
			typename OrderBy, 
			typename Limit, 
			typename Offset
			>
			struct select_helper_t
			{
				using _column_list_t = ColumnList;
				using _from_t = ColumnList;
				template<typename Database>
					struct can_run_t
					{
						//static_assert(is_where_t<Where>::value, "cannot select remove without having a where condition, use .where(true) to remove all rows");
						//static_assert(not vendor::is_noop<ColumnList>::value, "cannot run select without having selected anything");
						//static_assert(is_from_t<From>::value, "cannot run select without a from()");
						//static_assert(is_where_t<Where>::value, "cannot run select without having a where condition, use .where(true) to select all rows");
						// FIXME: Check for missing aliases (if references are used)
						// FIXME: Check for missing tables, well, actually, check for missing tables at the where(), order_by(), etc.

						static constexpr bool value = true;
					};
			};
	}

	// SELECT
	template<typename Database, typename... Policies>
		struct select_t: public vendor::policy_t<Policies>..., public vendor::crtp_wrapper_t<select_t<Database, Policies...>, Policies>...,
										 public detail::select_helper_t<Policies...>::_column_list_t::_value_type::template operators<select_t<Database, Policies...>>
		{
			template<typename Needle, typename Replacement>
				using _policy_update_t = select_t<Database, vendor::policy_update_t<Policies, Needle, Replacement>...>;

			using _database_t = Database;
			using _parameter_tuple_t = std::tuple<Policies...>;
			using _parameter_list_t = typename make_parameter_list_t<select_t>::type;
			
			using _column_list_t = typename detail::select_helper_t<Policies...>::_column_list_t;
			using _result_row_t = typename _column_list_t::_result_row_t;
			using _dynamic_names_t = typename _column_list_t::_dynamic_names_t;

			using _is_select = std::true_type;
			using _requires_braces = std::true_type;

			// FIXME: introduce checks whether this select could really be used as a value
			using _value_type = typename _column_list_t::_value_type;
			using _name_t = typename _column_list_t::_name_t;

			select_t()
			{}

			template<typename Whatever>
				select_t(select_t r, Whatever whatever):
					vendor::policy_t<Policies>(r, whatever)...
			{}

			template<typename Remove, typename Whatever>
				select_t(Remove r, Whatever whatever):
					vendor::policy_t<Policies>(r, whatever)...
			{}

			select_t(const select_t& r) = default;
			select_t(select_t&& r) = default;
			select_t& operator=(const select_t& r) = default;
			select_t& operator=(select_t&& r) = default;
			~select_t() = default;

			// Indicators
			template<typename AliasProvider>
				struct _pseudo_table_t
				{
					using table = typename _column_list_t::template _pseudo_table_t<select_t>;
					using alias = typename table::template _alias_t<AliasProvider>;
				};

			template<typename AliasProvider>
				typename _pseudo_table_t<AliasProvider>::alias as(const AliasProvider& aliasProvider) const
				{
					return typename _pseudo_table_t<AliasProvider>::table(
							*this).as(aliasProvider);
				}

			const _dynamic_names_t& get_dynamic_names() const
			{
				return _column_list_t::_dynamic_columns._dynamic_expression_names;
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
				return _result_row_t::static_size() + get_dynamic_names().size();
			}

			// Execute
			template<typename Db>
				auto _run(Db& db) const
				-> result_t<decltype(db.select(*this)), _result_row_t>
				{
					static_assert(detail::select_helper_t<Policies...>::template can_run_t<Db>::value, "Cannot execute select statement");
					static_assert(_get_static_no_of_parameters() == 0, "cannot run select directly with parameters, use prepare instead");
					return {db.select(*this), get_dynamic_names()};
				}

			// Prepare
			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_select_t<Db, select_t>
				{
					static_assert(detail::select_helper_t<Policies...>::template can_run_t<Db>::value, "Cannot prepare select statement");

					return {{}, get_dynamic_names(), db.prepare_select(*this)};
				}
		};

	namespace vendor
	{
		template<typename Context, typename Database, typename... Policies>
			struct interpreter_t<Context, select_t<Database, Policies...>>
			{
				using T = select_t<Database, Policies...>;

				static Context& _(const T& t, Context& context)
				{
					context << "SELECT ";

					interpret(t._flag_list(), context);
					interpret(t._column_list(), context);
					interpret(t._from(), context);
					interpret(t._where(), context);
					interpret(t._group_by(), context);
					interpret(t._having(), context);
					interpret(t._order_by(), context);
					interpret(t._limit(), context);
					interpret(t._offset(), context);

					return context;
				}
			};
	}

	template<typename Database>
		using blank_select_t = select_t<Database, 
					vendor::no_select_flag_list_t, 
					vendor::no_select_column_list_t, 
					vendor::no_from_t,
					vendor::no_where_t, 
					vendor::no_group_by_t,
					vendor::no_having_t,
					vendor::no_order_by_t,
					vendor::no_limit_t,
					vendor::no_offset_t>;

	namespace detail
	{
		template<typename Database, typename... Columns>
			using make_select_column_list_t = 
			copy_tuple_args_t<vendor::select_column_list_t, Database, 
			decltype(std::tuple_cat(as_tuple<Columns>::_(std::declval<Columns>())...))>;
	}


	blank_select_t<void> select() // FIXME: These should be constexpr
	{
		return { blank_select_t<void>() };
	}

	template<typename... Columns>
		auto select(Columns... columns)
		-> vendor::update_policies_t<blank_select_t<void>, 
		       vendor::no_select_column_list_t, 
					 detail::make_select_column_list_t<void, Columns...>>
		{
			return { blank_select_t<void>(), detail::make_select_column_list_t<void, Columns...>(std::tuple_cat(detail::as_tuple<Columns>::_(columns)...)) };
		}

	template<typename Database>
		blank_select_t<Database> dynamic_select(const Database&)
		{
			return { blank_select_t<Database>() };
		}

	template<typename Database, typename... Columns>
		auto dynamic_select(const Database&, Columns... columns)
		-> vendor::update_policies_t<blank_select_t<Database>, vendor::no_select_column_list_t, detail::make_select_column_list_t<void, Columns...>>
		{
			return { blank_select_t<Database>(), detail::make_select_column_list_t<void, Columns...>(std::tuple_cat(detail::as_tuple<Columns>::_(columns)...)) };
		}

}
#endif

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

#include <sqlpp11/detail/copy_tuple_args.h>
#include <sqlpp11/detail/arg_selector.h>

namespace sqlpp
{
	template<typename Db,
			typename... Policies
				>
		struct select_t;

	namespace detail
	{
		template<typename Db = void,
			typename FlagList = vendor::no_select_flag_list_t, 
			typename ColumnList = vendor::no_select_column_list_t, 
			typename From = vendor::no_from_t,
			typename ExtraTables = vendor::no_extra_tables_t,
			typename Where = vendor::no_where_t, 
			typename GroupBy = vendor::no_group_by_t, 
			typename Having = vendor::no_having_t,
			typename OrderBy = vendor::no_order_by_t, 
			typename Limit = vendor::no_limit_t, 
			typename Offset = vendor::no_offset_t
				>
			struct select_policies_t
			{
				using _database_t = Db;
				using _flag_list_t = FlagList;
				using _column_list_t = ColumnList;
				using _from_t = From;
				using _extra_tables_t = ExtraTables;
				using _where_t = Where;
				using _group_by_t = GroupBy;
				using _having_t = Having;
				using _order_by_t = OrderBy;
				using _limit_t = Limit;
				using _offset_t = Offset;

				using _statement_t = select_t<Db, FlagList, ColumnList, From, ExtraTables, Where, GroupBy, Having, OrderBy, Limit, Offset>;

				struct _methods_t:
					public _flag_list_t::template _methods_t<select_policies_t>,
					public _column_list_t::template _methods_t<select_policies_t>,
					public _from_t::template _methods_t<select_policies_t>,
					public _extra_tables_t::template _methods_t<select_policies_t>,
					public _where_t::template _methods_t<select_policies_t>,
					public _group_by_t::template _methods_t<select_policies_t>,
					public _having_t::template _methods_t<select_policies_t>,
					public _order_by_t::template _methods_t<select_policies_t>,
					public _limit_t::template _methods_t<select_policies_t>,
					public _offset_t::template _methods_t<select_policies_t>
				{};

				template<typename Needle, typename Replacement, typename... Policies>
					struct _policies_update_t
					{
						static_assert(detail::is_element_of<Needle, make_type_set_t<Policies...>>::value, "policies update for non-policy class detected");
						using type =  select_t<Db, vendor::policy_update_t<Policies, Needle, Replacement>...>;
					};

				template<typename Needle, typename Replacement>
					using _new_statement_t = typename _policies_update_t<Needle, Replacement, FlagList, ColumnList, From, ExtraTables, Where, GroupBy, Having, OrderBy, Limit, Offset>::type;

				static_assert(is_noop_t<ColumnList>::value or sqlpp::is_select_column_list_t<ColumnList>::value, "column list of select is neither naught nor a valid column list");
				static_assert(is_noop_t<From>::value or sqlpp::is_from_t<From>::value, "from() part of select is neither naught nor a valid from()");

				using _known_tables = detail::make_joined_set_t<typename _from_t::_table_set, typename _extra_tables_t::_table_set>;

				template<typename Expression>
					using _no_unknown_tables = detail::is_subset_of<typename Expression::_table_set, _known_tables>;

				using _required_tables = 
					detail::make_joined_set_t<
						typename _flag_list_t::_table_set,
						typename _column_list_t::_table_set,
						typename _where_t::_table_set,
						typename _group_by_t::_table_set,
						typename _having_t::_table_set,
						typename _order_by_t::_table_set,
						typename _limit_t::_table_set,
						typename _offset_t::_table_set
							>;

				// The tables not covered by the from.
				using _table_set = detail::make_difference_set_t<
					_required_tables,
					typename _from_t::_table_set // Hint: extra_tables_t is not used here because it is just a helper for dynamic .add_*() methods and should not change the structural integrity
							>;

				// A select can be used as a pseudo table if
				//   - at least one column is selected
				//   - the select is complete (leaks no tables)
				using _can_be_used_as_table = typename std::conditional<
					is_select_column_list_t<_column_list_t>::value and _table_set::size::value == 0,
					std::true_type,
					std::false_type
					>::type;

				using _value_type = typename std::conditional<
					is_select_column_list_t<_column_list_t>::value and is_subset_of<typename _column_list_t::_table_set, typename _from_t::_table_set>::value,
					typename ColumnList::_value_type,
					no_value_t // If something is selected that requires a table, then we require a from for this to be a value
						>::type;
			};
	}

	// SELECT
	template<typename Db,
			typename... Policies
				>
		struct select_t:
			public detail::select_policies_t<Db, Policies...>::_value_type::template expression_operators<select_t<Db, Policies...>>,
			public detail::select_policies_t<Db, Policies...>::_methods_t
		{
			using _policies_t = typename detail::select_policies_t<Db, Policies...>;
			using _database_t = typename _policies_t::_database_t;
			using _flag_list_t = typename _policies_t::_flag_list_t;
			using _column_list_t = typename _policies_t::_column_list_t;
			using _from_t = typename _policies_t::_from_t;
			using _extra_tables_t = typename _policies_t::_extra_tables_t;
			using _where_t = typename _policies_t::_where_t;
			using _group_by_t = typename _policies_t::_group_by_t;
			using _having_t = typename _policies_t::_having_t;
			using _order_by_t = typename _policies_t::_order_by_t;
			using _limit_t = typename _policies_t::_limit_t;
			using _offset_t = typename _policies_t::_offset_t;

			using _is_dynamic = typename std::conditional<std::is_same<_database_t, void>::value, std::false_type, std::true_type>::type;

			using _parameter_tuple_t = std::tuple<Policies...>;
			using _parameter_list_t = typename make_parameter_list_t<select_t>::type;
			using _table_set = typename _policies_t::_table_set;
			
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

			template<typename Statement, typename T>
				select_t(Statement s, T t):
					_flag_list(detail::arg_selector<_flag_list_t>::_(s._flag_list, t)),
					_column_list(detail::arg_selector<_column_list_t>::_(s._column_list, t)),
					_from(detail::arg_selector<_from_t>::_(s._from, t)),
					_where(detail::arg_selector<_where_t>::_(s._where, t)),
					_group_by(detail::arg_selector<_group_by_t>::_(s._group_by, t)),
					_having(detail::arg_selector<_having_t>::_(s._having, t)),
					_order_by(detail::arg_selector<_order_by_t>::_(s._order_by, t)),
					_limit(detail::arg_selector<_limit_t>::_(s._limit, t)),
					_offset(detail::arg_selector<_offset_t>::_(s._offset, t))
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
				return _column_list._dynamic_columns._dynamic_expression_names;
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

			template<typename A>
				struct is_table_subset_of_from
				{
					static constexpr bool value = ::sqlpp::detail::is_subset_of<typename A::_table_set, typename _from_t::_table_set>::value;
				};

			// Execute
			template<typename Database>
				auto _run(Database& db) const
				-> result_t<decltype(db.select(*this)), _result_row_t<Database>>
				{
#warning: need to check in add_xy method as well
#warning: need add_wxy_without_table_check
					static_assert(is_table_subset_of_from<_flag_list_t>::value, "flags require additional tables in from()");
					static_assert(is_table_subset_of_from<_column_list_t>::value, "selected columns require additional tables in from()");
					static_assert(is_table_subset_of_from<_where_t>::value, "where() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<_group_by_t>::value, "group_by() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<_having_t>::value, "having() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<_order_by_t>::value, "order_by() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<_limit_t>::value, "limit() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<_offset_t>::value, "offset() expression requires additional tables in from()");
					static_assert(not _table_set::size::value, "one sub expression contains tables which are not in the from()");
					static_assert(_get_static_no_of_parameters() == 0, "cannot run select directly with parameters, use prepare instead");

					return {db.select(*this), get_dynamic_names()};
				}

			// Prepare
			template<typename Database>
				auto _prepare(Database& db) const
				-> prepared_select_t<Database, select_t>
				{

					return {{}, get_dynamic_names(), db.prepare_select(*this)};
				}

			_flag_list_t _flag_list;
			_column_list_t _column_list;
			_from_t _from;
			_where_t _where;
			_group_by_t _group_by;
			_having_t _having;
			_order_by_t _order_by;
			_limit_t _limit;
			_offset_t _offset;
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

					serialize(t._flag_list, context);
					serialize(t._column_list, context);
					serialize(t._from, context);
					serialize(t._where, context);
					serialize(t._group_by, context);
					serialize(t._having, context);
					serialize(t._order_by, context);
					serialize(t._limit, context);
					serialize(t._offset, context);

					return context;
				}
			};
	}

	template<typename Database, typename... Policies>
		using make_select_t = typename detail::select_policies_t<Database, Policies...>::_statement_t;

	namespace detail
	{
		template<typename Database, typename... Columns>
			using make_select_column_list_t = 
			copy_tuple_args_t<vendor::select_column_list_t, Database, 
			decltype(std::tuple_cat(as_tuple<Columns>::_(std::declval<Columns>())...))>;
	}

	make_select_t<void> select() // FIXME: These should be constexpr
	{
		return { };
	}

	template<typename... Columns>
		auto select(Columns... columns)
		-> make_select_t<void, vendor::no_select_flag_list_t, detail::make_select_column_list_t<void, Columns...>>
		{
			return { make_select_t<void>(), detail::make_select_column_list_t<void, Columns...>(std::tuple_cat(detail::as_tuple<Columns>::_(columns)...)) };
		}

	template<typename Database>
		make_select_t<Database> dynamic_select(const Database&)
		{
			return { make_select_t<Database>() };
		}

	template<typename Database, typename... Columns>
		auto dynamic_select(const Database&, Columns... columns)
		-> make_select_t<Database, vendor::no_select_flag_list_t, detail::make_select_column_list_t<void, Columns...>>
		{
			return { make_select_t<Database>(), detail::make_select_column_list_t<void, Columns...>(std::tuple_cat(detail::as_tuple<Columns>::_(columns)...)) };
		}

}
#endif

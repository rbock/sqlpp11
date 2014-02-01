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
#include <sqlpp11/select_fwd.h>
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

#include <sqlpp11/detail/make_flag_tuple.h>
#include <sqlpp11/detail/make_expression_tuple.h>

#include <sstream>

namespace sqlpp
{
	template<
		typename Database,
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
		struct select_t
		: public ColumnList::_value_type::template operators<select_t<
		                 Database,
										 FlagList, 
										 ColumnList, 
										 From, 
										 Where, 
										 GroupBy, 
										 Having, 
										 OrderBy, 
										 Limit, 
										 Offset>>
		{
			using _Database = Database;
			using _From = From;

			static_assert(is_select_flag_list_t<FlagList>::value, "invalid list of select flags");
			static_assert(is_select_column_list_t<ColumnList>::value, "invalid list of select expressions");
			static_assert(vendor::is_noop<From>::value or is_from_t<From>::value, "invalid 'from' argument");
			static_assert(vendor::is_noop<Where>::value or is_where_t<Where>::value, "invalid 'where' argument");
			static_assert(vendor::is_noop<GroupBy>::value or is_group_by_t<GroupBy>::value, "invalid 'group by' arguments");
			static_assert(vendor::is_noop<Having>::value or is_having_t<Having>::value, "invalid 'having' arguments");
			static_assert(vendor::is_noop<OrderBy>::value or is_order_by_t<OrderBy>::value, "invalid 'order by' arguments");
			static_assert(vendor::is_noop<Limit>::value or is_limit_t<Limit>::value, "invalid 'limit' arguments");
			static_assert(vendor::is_noop<Offset>::value or is_offset_t<Offset>::value, "invalid 'offset' arguments");

			using _is_select = std::true_type;
			using _requires_braces = std::true_type;

			template<typename FlagListT> 
				using set_flag_list_t = select_t<Database, FlagListT, ColumnList, From, Where, GroupBy, Having, OrderBy, Limit, Offset>;
			template<typename ColumnListT> 
				using set_column_list_t = select_t<Database, FlagList, ColumnListT, From, Where, GroupBy, Having, OrderBy, Limit, Offset>;
			template<typename FromT> 
				using set_from_t = select_t<Database, FlagList, ColumnList, FromT, Where, GroupBy, Having, OrderBy, Limit, Offset>;
			template<typename WhereT>
				using set_where_t = select_t<Database, FlagList, ColumnList, From, WhereT, GroupBy, Having, OrderBy, Limit, Offset>;
			template<typename GroupByT>
				using set_group_by_t = select_t<Database, FlagList, ColumnList, From, Where, GroupByT, Having, OrderBy, Limit, Offset>;
			template<typename HavingT>
				using set_having_t = select_t<Database, FlagList, ColumnList, From, Where, GroupBy, HavingT, OrderBy, Limit, Offset>;
			template<typename OrderByT>
				using set_order_by_t = select_t<Database, FlagList, ColumnList, From, Where, GroupBy, Having, OrderByT, Limit, Offset>;
			template<typename LimitT>
				using set_limit_t = select_t<Database, FlagList, ColumnList, From, Where, GroupBy, Having, OrderBy, LimitT, Offset>;
			template<typename OffsetT>
			using set_offset_t = select_t<Database, FlagList, ColumnList, From, Where, GroupBy, Having, OrderBy, Limit, OffsetT>;

			using _result_row_t = typename ColumnList::_result_row_t;
			using _dynamic_names_t = typename ColumnList::_dynamic_names_t;
			using _parameter_tuple_t = std::tuple<ColumnList, Where, GroupBy, Having, OrderBy, Limit, Offset>;
			using _parameter_list_t = typename make_parameter_list_t<select_t>::type;

			// Indicators
			using _value_type = typename std::conditional<
				vendor::is_noop<From>::value, 
				no_value_t, // If there is no from, the select is not complete (this logic is a bit simple, but better than nothing)
				typename ColumnList::_value_type>::type;

			using _name_t = typename ColumnList::_name_t;

			// The standard constructors, assigment operators and destructor
			constexpr select_t(FlagList flag_list, ColumnList column_list, From from,
					Where where, GroupBy group_by, Having having,
					OrderBy order_by, Limit limit, Offset offset):
				_flags(flag_list),
				_columns(column_list),
				_from(from),
				_where(where),
				_group_by(group_by),
				_having(having),
				_order_by(order_by),
				_limit(limit),
				_offset(offset)
			{
			}

			select_t(const select_t& rhs) = default;
			select_t(select_t&& rhs) = default;
			select_t& operator=(const select_t& rhs) = default;
			select_t& operator=(select_t&& rhs) = default;
			~select_t() = default;

			// select functions
			template<typename... Flag>
			auto flags(Flag... flag)
				-> set_flag_list_t<vendor::select_flag_list_t<void, std::tuple<Flag...>>>
				{
					static_assert(not FlagList::size::value, "cannot call dynamic_flags() after specifying them the first time");
					static_assert(not ColumnList::size::value, "cannot call columns() after specifying them the first time");
					return {
							{std::tuple<Flag...>{flag...}}, 
							_columns, 
							_from,
							_where, 
							_group_by, 
							_having, 
							_order_by, 
							_limit,
							_offset
							};
				}

			template<typename... Flag>
			auto dynamic_flags(Flag... flag)
				-> set_flag_list_t<vendor::select_flag_list_t<Database, std::tuple<Flag...>>>
				{
					static_assert(not std::is_same<Database, void>::value, "cannot call dynamic_flags() in a non-dynamic select");
					static_assert(not FlagList::size::value, "cannot call dynamic_flags() after specifying them the first time");
					static_assert(not ColumnList::size::value, "cannot call columns() after specifying them the first time");
					return {
							{std::tuple<Flag...>{flag...}}, 
							_columns, 
							_from,
							_where, 
							_group_by, 
							_having, 
							_order_by, 
							_limit,
							_offset
							};
				}

			template<typename Flag>
				select_t& add_flag(Flag flag)
				{
					static_assert(is_dynamic_t<FlagList>::value, "cannot call add_flag() in a non-dynamic column list");

					_flags.add(flag);

					return *this;
				}

			template<typename... Column>
			auto columns(Column... column)
				-> set_column_list_t<vendor::select_column_list_t<void, std::tuple<Column...>>>
				{
					static_assert(not ColumnList::size::value, "cannot call columns() after specifying them the first time");
					return {
							_flags, 
							{std::tuple<Column...>{column...}}, 
							_from,
							_where, 
							_group_by, 
							_having, 
							_order_by, 
							_limit,
							_offset
							};
				}

			template<typename... Column>
			auto dynamic_columns(Column... column)
				-> set_column_list_t<vendor::select_column_list_t<Database, std::tuple<Column...>>>
				{
					static_assert(not std::is_same<Database, void>::value, "cannot call dynamic_columns() in a non-dynamic select");
					static_assert(not ColumnList::size::value, "cannot call dynamic_columns() after specifying them the first time");
					return {
							_flags, 
							{std::tuple<Column...>{column...}}, 
							_from,
							_where, 
							_group_by, 
							_having, 
							_order_by, 
							_limit,
							_offset
							};
				}

			template<typename NamedExpr>
				select_t& add_column(NamedExpr namedExpr)
				{
					static_assert(is_dynamic_t<ColumnList>::value, "cannot call add_column() in a non-dynamic column list");

					_columns.add(namedExpr);

					return *this;
				}

			template<typename... Table>
				auto from(Table... table)
				-> set_from_t<vendor::from_t<void, Table...>>
				{
					static_assert(not vendor::is_noop<ColumnList>::value, "cannot call from() without having selected anything");
					static_assert(vendor::is_noop<From>::value, "cannot call from() twice for a single select");
					return {
							_flags, 
							_columns, 
							{std::tuple<Table...>{table...}}, 
							_where, 
							_group_by, 
							_having, 
							_order_by, 
							_limit,
							_offset
							};
				}

			template<typename... Table>
				auto dynamic_from(Table... table)
				-> set_from_t<vendor::from_t<Database, Table...>>
				{
					static_assert(not std::is_same<Database, void>::value, "cannot call dynamic_from() in a non-dynamic select");
					static_assert(not vendor::is_noop<ColumnList>::value, "cannot call from() without having selected anything");
					static_assert(vendor::is_noop<From>::value, "cannot call from() twice for a single select");
					return {
							_flags, 
							_columns, 
							{std::tuple<Table...>{table...}}, 
							_where, 
							_group_by, 
							_having, 
							_order_by, 
							_limit,
							_offset
							};
				}

			template<typename Table>
				select_t& add_from(Table table)
				{
					static_assert(not vendor::is_noop<ColumnList>::value, "cannot call add_from() without having selected anything");
					static_assert(is_dynamic_t<From>::value, "cannot call add_from() in a non-dynamic from");

					_from.add(table);

					return *this;
				}

			template<typename... Expr>
				auto where(Expr... expr)
				-> set_where_t<vendor::where_t<void, Expr...>> 
				{
					static_assert(not vendor::is_noop<From>::value, "cannot call where() without a from()");
					static_assert(vendor::is_noop<Where>::value, "cannot call where() or dynamic_where() twice for a single select");
					return {
							_flags, 
							_columns, 
							_from, 
							{std::tuple<Expr...>{expr...}},
							_group_by,
							_having,
							_order_by,
							_limit,
							_offset,
							};
				}

			template<typename... Expr>
				auto dynamic_where(Expr... expr)
				-> set_where_t<vendor::where_t<Database, Expr...>>
				{
					static_assert(not vendor::is_noop<From>::value, "cannot call dynamic_where() without a from()");
					static_assert(vendor::is_noop<Where>::value, "cannot call where() or dynamic_where() twice for a single select");
					return {
							_flags, 
							_columns, 
							_from, 
							{std::tuple<Expr...>{expr...}},
							_group_by,
							_having,
							_order_by,
							_limit,
							_offset,
							};
				}

			template<typename Expr>
				select_t& add_where(Expr expr)
				{
					static_assert(is_dynamic_t<Where>::value, "cannot call add_where() with a non-dynamic where");

					_where.add(expr);

					return *this;
				}

			template<typename... Col>
				auto group_by(Col... column)
				-> set_group_by_t<vendor::group_by_t<void, Col...>>
				{
					static_assert(not vendor::is_noop<From>::value, "cannot call group_by() without a from()");
					static_assert(vendor::is_noop<GroupBy>::value, "cannot call group_by() twice for a single select");
					return {
							_flags, 
							_columns,
							_from,
							_where,
							{std::tuple<Col...>{column...}},
							_having,
							_order_by,
							_limit,
							_offset,
							};
				}

			template<typename... Col>
				auto dynamic_group_by(Col... column)
				-> set_group_by_t<vendor::group_by_t<Database, Col...>>
				{
					static_assert(not vendor::is_noop<From>::value, "cannot call group_by() without a from()");
					static_assert(vendor::is_noop<GroupBy>::value, "cannot call group_by() twice for a single select");
					return {
							_flags, 
							_columns,
							_from,
							_where,
							{std::tuple<Col...>{column...}},
							_having,
							_order_by,
							_limit,
							_offset,
							};
				}

			template<typename Expr>
				select_t& add_group_by(Expr expr)
				{
					static_assert(is_dynamic_t<GroupBy>::value, "cannot call add_group_by() in a non-dynamic group_by");

					_group_by.add(expr);

					return *this;
				}

			template<typename... Expr>
				auto having(Expr... expr)
				-> set_having_t<vendor::having_t<void, Expr...>> 
				{
					static_assert(not vendor::is_noop<GroupBy>::value, "cannot call having() without a group_by");
					static_assert(vendor::is_noop<Having>::value, "cannot call having() twice for a single select");
					return {
							_flags, 
							_columns,
							_from,
							_where,
							_group_by,
							{std::tuple<Expr...>{expr...}},
							_order_by,
							_limit,
							_offset,
							};
				}

			template<typename... Expr>
				auto dynamic_having(Expr... expr)
				-> set_having_t<vendor::having_t<Database, Expr...>> 
				{
					static_assert(not vendor::is_noop<GroupBy>::value, "cannot call having() without a group_by");
					static_assert(vendor::is_noop<Having>::value, "cannot call having() twice for a single select");
					return {
						_flags, 
							_columns,
							_from,
							_where,
							_group_by,
							{std::tuple<Expr...>{expr...}},
							_order_by,
							_limit,
							_offset,
					};
				}

			template<typename Expr>
				select_t& add_having(Expr expr)
				{
					static_assert(is_dynamic_t<Having>::value, "cannot call add_having() in a non-dynamic having");

					_having.add(expr);

					return *this;
				}

			template<typename... OrderExpr>
				auto order_by(OrderExpr... expr)
				-> set_order_by_t<vendor::order_by_t<void, OrderExpr...>>
				{
					static_assert(not vendor::is_noop<From>::value, "cannot call order_by() without a from()");
					static_assert(vendor::is_noop<OrderBy>::value, "cannot call order_by() twice for a single select");
					return {
							_flags, 
							_columns,
							_from,
							_where,
							_group_by,
							_having,
							{std::tuple<OrderExpr...>{expr...}},
							_limit,
							_offset,
							};
				}

			template<typename... OrderExpr>
				auto dynamic_order_by(OrderExpr... expr)
				-> set_order_by_t<vendor::order_by_t<Database, OrderExpr...>>
				{
					static_assert(not vendor::is_noop<From>::value, "cannot call order_by() without a from()");
					static_assert(vendor::is_noop<OrderBy>::value, "cannot call order_by() twice for a single select");
					return {
							_flags, 
							_columns,
							_from,
							_where,
							_group_by,
							_having,
							{std::tuple<OrderExpr...>{expr...}},
							_limit,
							_offset,
							};
				}

			template<typename Expr>
				select_t& add_order_by(Expr expr)
				{
					static_assert(is_dynamic_t<OrderBy>::value, "cannot call add_order_by() in a non-dynamic order_by");

					_order_by.add(expr);

					return *this;
				}

			template<typename Expr>
				auto limit(Expr limit)
				-> set_limit_t<vendor::limit_t<typename vendor::wrap_operand<Expr>::type>>
				{
					static_assert(not vendor::is_noop<From>::value, "cannot call limit() without a from()");
					static_assert(vendor::is_noop<Limit>::value, "cannot call limit() twice for a single select");
					return {
						_flags, 
							_columns,
							_from,
							_where,
							_group_by,
							_having,
							_order_by,
							{limit},
							_offset,
					};
				}

			auto dynamic_limit(std::size_t limit = 0)
				->set_limit_t<vendor::dynamic_limit_t>
			{
				static_assert(not vendor::is_noop<From>::value, "cannot call limit() without a from()");
				static_assert(vendor::is_noop<Limit>::value, "cannot call limit() twice for a single select");
				return {
						_flags, 
						_columns,
						_from,
						_where,
						_group_by,
						_having,
						_order_by,
						{limit},
						_offset,
				};
			}

			select_t& set_limit(std::size_t limit)
			{
				static_assert(is_dynamic_t<Limit>::value, "cannot call set_limit() in a non-dynamic limit");

				_limit.set(limit);

				return *this;
			}

			template<typename Expr>
			auto offset(Expr offset)
				-> set_offset_t<vendor::offset_t<typename vendor::wrap_operand<Expr>::type>>
			{
				static_assert(not vendor::is_noop<Limit>::value, "cannot call offset() without a limit");
				static_assert(vendor::is_noop<Offset>::value, "cannot call offset() twice for a single select");
				return {
						_flags, 
						_columns,
						_from,
						_where,
						_group_by,
						_having,
						_order_by,
						_limit,
						{offset},
				};
			}

			auto dynamic_offset(std::size_t offset = 0)
				-> set_offset_t<vendor::dynamic_offset_t>
			{
				static_assert(not vendor::is_noop<Limit>::value, "cannot call offset() without a limit");
				static_assert(vendor::is_noop<Offset>::value, "cannot call offset() twice for a single select");
				return {
						_flags, 
						_columns,
						_from,
						_where,
						_group_by,
						_having,
						_order_by,
						_limit,
						{offset},
				};
			}

			select_t& set_offset(std::size_t offset)
			{
				static_assert(is_dynamic_t<Offset>::value, "cannot call set_offset() in a non-dynamic limit");

				_offset.set(offset);

				return *this;
			}

			template<typename AliasProvider>
				struct _pseudo_table_t
				{
					using table = typename ColumnList::template _pseudo_table_t<select_t>;
					using alias = typename table::template _alias_t<AliasProvider>;
				};

			template<typename AliasProvider>
				typename _pseudo_table_t<AliasProvider>::alias as(const AliasProvider& aliasProvider) const
				{
					return typename _pseudo_table_t<AliasProvider>::table(
							*this).as(aliasProvider);
				}

			const typename ColumnList::_dynamic_names_t& get_dynamic_names() const
			{
				return _columns._dynamic_columns._dynamic_expression_names;
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
					static_assert(not vendor::is_noop<ColumnList>::value, "cannot run select without having selected anything");
					static_assert(is_from_t<From>::value, "cannot run select without a from()");
					static_assert(is_where_t<Where>::value, "cannot run select without having a where condition, use .where(true) to select all rows");
					static_assert(_get_static_no_of_parameters() == 0, "cannot run select directly with parameters, use prepare instead");
					// FIXME: Check for missing aliases (if references are used)
					// FIXME: Check for missing tables, well, actually, check for missing tables at the where(), order_by(), etc.

					return {db.select(*this), get_dynamic_names()};
				}

			// Prepare
			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_select_t<Db, select_t>
				{
					static_assert(not vendor::is_noop<ColumnList>::value, "cannot run select without having selected anything");
					static_assert(is_from_t<From>::value, "cannot run select without a from()");
					// FIXME: Check for missing aliases (if references are used)
					// FIXME: Check for missing tables, well, actually, check for missing tables at the where(), order_by(), etc.

					return {{}, get_dynamic_names(), db.prepare_select(*this)};
				}

			FlagList _flags;
			ColumnList _columns;
			From _from;
			Where _where;
			GroupBy _group_by;
			Having _having;
			OrderBy _order_by;
			Limit _limit;
			Offset _offset;
		};

	namespace vendor
	{
		template<typename Context, 
			typename Database,
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
				struct interpreter_t<Context, select_t<Database,
			FlagList, 
			ColumnList, 
			From, 
			Where, 
			GroupBy, 
			Having, 
			OrderBy, 
			Limit, 
			Offset>>
			{
				using T = select_t<Database,
				FlagList, 
				ColumnList, 
				From, 
				Where, 
				GroupBy, 
				Having, 
				OrderBy, 
				Limit, 
				Offset>;

				static Context& _(const T& t, Context& context)
				{
					context << "SELECT ";

					interpret(t._flags, context);
					interpret(t._columns, context);
					interpret(t._from, context);
					interpret(t._where, context);
					interpret(t._group_by, context);
					interpret(t._having, context);
					interpret(t._order_by, context);
					interpret(t._limit, context);
					interpret(t._offset, context);

					return context;
				}
			};
	}


	// construct select flag list
	namespace detail
	{
		template<typename Database, typename... Expr>
			using make_select_flag_list_t = 
			vendor::select_flag_list_t<Database, decltype(make_flag_tuple(std::declval<Expr>()...))>;
	}

	// construct select expression list
	namespace detail
	{
		template<typename Database, typename... Expr>
			using make_select_column_list_t = 
			vendor::select_column_list_t<Database, decltype(make_expression_tuple(std::declval<Expr>()...))>;
	}

	auto select()
		-> select_t<void, vendor::select_flag_list_t<void, std::tuple<>>, vendor::select_column_list_t<void, std::tuple<>>>
		{
			return { {}, vendor::select_column_list_t<void, std::tuple<>>{}, {}, {}, {}, {}, {}, {}, {} };
		}

	template<typename... NamedExpr>
		auto select(NamedExpr... namedExpr)
		-> select_t<void, detail::make_select_flag_list_t<void, NamedExpr...>, detail::make_select_column_list_t<void, NamedExpr...>>
		{
			return { 
				{ detail::make_flag_tuple(namedExpr...) }, 
					{ detail::make_expression_tuple(namedExpr...) },
				{}, {}, {}, {}, {}, {}, {}
			};
		}

	template<typename Db, typename... NamedExpr>
		auto dynamic_select(const Db& db, NamedExpr... namedExpr)
		-> select_t<Db, detail::make_select_flag_list_t<Db, NamedExpr...>, detail::make_select_column_list_t<Db, NamedExpr...>>
		{
			return { 
				{ detail::make_flag_tuple(namedExpr...) }, 
					{ detail::make_expression_tuple(namedExpr...) },
				{}, {}, {}, {}, {}, {}, {}
			};
		}

}
#endif

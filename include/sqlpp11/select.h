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
	namespace detail
	{
		template<
			typename ColumnList, 
			typename From
			>
			struct select_helper_t
			{
				static_assert(is_noop_t<ColumnList>::value or sqlpp::is_select_column_list_t<ColumnList>::value, "column list of select is neither naught nor a valid column list");
				static_assert(is_noop_t<From>::value or sqlpp::is_from_t<From>::value, "from() part of select is neither naught nor a valid from()");
				using _value_type = typename std::conditional<
					sqlpp::is_from_t<From>::value,
					typename ColumnList::_value_type,
					no_value_t // If there is no from, the select is not complete (this logic is a bit simple, but better than nothing)
						>::type;
			};
	}

	// SELECT
	template<typename Database = void,
			typename FlagList = vendor::no_select_flag_list_t, 
			typename ColumnList = vendor::no_select_column_list_t, 
			typename From = vendor::no_from_t,
			typename Where = vendor::no_where_t, 
			typename GroupBy = vendor::no_group_by_t, 
			typename Having = vendor::no_having_t,
			typename OrderBy = vendor::no_order_by_t, 
			typename Limit = vendor::no_limit_t, 
			typename Offset = vendor::no_offset_t
				>
		struct select_t: public detail::select_helper_t<ColumnList, From>::_value_type::template expression_operators<select_t<Database, FlagList, ColumnList, From, Where, GroupBy, Having, OrderBy, Limit, Offset>>
		{
			using _database_t = Database;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

			template<typename Needle, typename Replacement, typename... Policies>
				struct _policies_update_impl
				{
					using type =  select_t<Database, vendor::policy_update_t<Policies, Needle, Replacement>...>;
				};

			template<typename Needle, typename Replacement>
				using _policies_update_t = typename _policies_update_impl<Needle, Replacement, FlagList, ColumnList, From, Where, GroupBy, Having, OrderBy, Limit, Offset>::type;

			using _parameter_tuple_t = std::tuple<FlagList, ColumnList, From, Where, GroupBy, Having, OrderBy, Limit, Offset>;
			using _parameter_list_t = typename make_parameter_list_t<select_t>::type;
			using _table_set = ::sqlpp::detail::type_set<>;
			
			using _column_list_t = ColumnList;
			template<typename Db>
				using _result_row_t = typename _column_list_t::template _result_row_t<Db>;
			using _dynamic_names_t = typename _column_list_t::_dynamic_names_t;

			using _is_select = std::true_type;
			using _requires_braces = std::true_type;

			using _value_type = typename detail::select_helper_t<ColumnList, From>::_value_type;
			using _name_t = typename _column_list_t::_name_t;

			// Constructors
			select_t()
			{}

			template<typename Statement, typename T>
				select_t(Statement s, T t):
					_flag_list(detail::arg_selector<FlagList>::_(s._flag_list, t)),
					_column_list(detail::arg_selector<ColumnList>::_(s._column_list, t)),
					_from(detail::arg_selector<From>::_(s._from, t)),
					_where(detail::arg_selector<Where>::_(s._where, t)),
					_group_by(detail::arg_selector<GroupBy>::_(s._group_by, t)),
					_having(detail::arg_selector<Having>::_(s._having, t)),
					_order_by(detail::arg_selector<OrderBy>::_(s._order_by, t)),
					_limit(detail::arg_selector<Limit>::_(s._limit, t)),
					_offset(detail::arg_selector<Offset>::_(s._offset, t))
			{}

			select_t(const select_t& r) = default;
			select_t(select_t&& r) = default;
			select_t& operator=(const select_t& r) = default;
			select_t& operator=(select_t&& r) = default;
			~select_t() = default;

			// type update functions
			template<typename... Args>
				auto flags(Args... args)
				-> _policies_update_t<vendor::no_select_flag_list_t, vendor::select_flag_list_t<void, Args...>>
				{
					static_assert(is_noop_t<FlagList>::value, "flags()/dynamic_flags() must not be called twice");
					return { *this, vendor::select_flag_list_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_flags(Args... args)
				-> _policies_update_t<vendor::no_select_flag_list_t, vendor::select_flag_list_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<FlagList>::value, "flags()/dynamic_flags() must not be called twice");
					static_assert(_is_dynamic::value, "dynamic_flags must not be called in a static statement");
					return { *this, vendor::select_flag_list_t<_database_t, Args...>{args...} };
				}

			template<typename... Args>
				auto columns(Args... args)
				-> _policies_update_t<vendor::no_select_column_list_t, vendor::select_column_list_t<void, Args...>>
				{
					static_assert(is_noop_t<ColumnList>::value, "columns()/dynamic_columns() must not be called twice");
					return { *this, vendor::select_column_list_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_columns(Args... args)
				-> _policies_update_t<vendor::no_select_column_list_t, vendor::select_column_list_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<ColumnList>::value, "columns()/dynamic_columns() must not be called twice");
					static_assert(_is_dynamic::value, "dynamic_columns must not be called in a static statement");
					return { *this, vendor::select_column_list_t<_database_t, Args...>{args...} };
				}

			template<typename... Args>
				auto from(Args... args)
				-> _policies_update_t<vendor::no_from_t, vendor::from_t<void, Args...>>
				{
					return { *this, vendor::from_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_from(Args... args)
				-> _policies_update_t<vendor::no_from_t, vendor::from_t<_database_t, Args...>>
				{
					static_assert(not std::is_same<_database_t, void>::value, "dynamic_from must not be called in a static statement");
					return { *this, vendor::from_t<_database_t, Args...>{args...} };
				}

			template<typename... Args>
				auto where(Args... args)
				-> _policies_update_t<vendor::no_where_t, vendor::where_t<void, Args...>>
				{
					static_assert(is_noop_t<Where>::value, "cannot call where()/dynamic_where() twice");
					return { *this, vendor::where_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_where(Args... args)
				-> _policies_update_t<vendor::no_where_t, vendor::where_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<Where>::value, "cannot call where()/dynamic_where() twice");
					static_assert(not std::is_same<_database_t, void>::value, "dynamic_where must not be called in a static statement");
					return { *this, vendor::where_t<_database_t, Args...>{args...} };
				}

			template<typename... Args>
				auto group_by(Args... args)
				-> _policies_update_t<vendor::no_group_by_t, vendor::group_by_t<void, Args...>>
				{
					static_assert(is_noop_t<GroupBy>::value, "cannot call group_by()/dynamic_group_by() twice");
					return { *this, vendor::group_by_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_group_by(Args... args)
				-> _policies_update_t<vendor::no_group_by_t, vendor::group_by_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<GroupBy>::value, "cannot call group_by()/dynamic_group_by() twice");
					static_assert(not std::is_same<_database_t, void>::value, "dynamic_group_by must not be called in a static statement");
					return { *this, vendor::group_by_t<_database_t, Args...>{args...} };
				}

			template<typename... Args>
				auto having(Args... args)
				-> _policies_update_t<vendor::no_having_t, vendor::having_t<void, Args...>>
				{
					static_assert(is_noop_t<Having>::value, "cannot call having()/dynamic_having() twice");
					return { *this, vendor::having_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_having(Args... args)
				-> _policies_update_t<vendor::no_having_t, vendor::having_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<Having>::value, "cannot call having()/dynamic_having() twice");
					static_assert(not std::is_same<_database_t, void>::value, "dynamic_having must not be called in a static statement");
					return { *this, vendor::having_t<_database_t, Args...>{args...} };
				}

			template<typename... Args>
				auto order_by(Args... args)
				-> _policies_update_t<vendor::no_order_by_t, vendor::order_by_t<void, Args...>>
				{
					static_assert(is_noop_t<OrderBy>::value, "cannot call order_by()/dynamic_order_by() twice");
					return { *this, vendor::order_by_t<void, Args...>{args...} };
				}

			template<typename... Args>
				auto dynamic_order_by(Args... args)
				-> _policies_update_t<vendor::no_order_by_t, vendor::order_by_t<_database_t, Args...>>
				{
					static_assert(is_noop_t<OrderBy>::value, "cannot call order_by()/dynamic_order_by() twice");
					static_assert(not std::is_same<_database_t, void>::value, "dynamic_order_by must not be called in a static statement");
					return { *this, vendor::order_by_t<_database_t, Args...>{args...} };
				}

			template<typename Arg>
				auto limit(Arg arg)
				-> _policies_update_t<vendor::no_limit_t, vendor::limit_t<typename vendor::wrap_operand<Arg>::type>>
				{
					static_assert(is_noop_t<Limit>::value, "cannot call limit()/dynamic_limit() twice");
					return { *this, vendor::limit_t<typename vendor::wrap_operand<Arg>::type>{{arg}} };
				}

				auto dynamic_limit()
				-> _policies_update_t<vendor::no_limit_t, vendor::dynamic_limit_t<_database_t>>
				{
					static_assert(is_noop_t<Limit>::value, "cannot call limit()/dynamic_limit() twice");
					static_assert(not std::is_same<_database_t, void>::value, "dynamic_limit must not be called in a static statement");
					return { *this, vendor::dynamic_limit_t<_database_t>{} };
				}

			template<typename Arg>
				auto offset(Arg arg)
				-> _policies_update_t<vendor::no_offset_t, vendor::offset_t<typename vendor::wrap_operand<Arg>::type>>
				{
					static_assert(is_noop_t<Offset>::value, "cannot call offset()/dynamic_offset() twice");
					return { *this, vendor::offset_t<typename vendor::wrap_operand<Arg>::type>{{arg}} };
				}

				auto dynamic_offset()
				-> _policies_update_t<vendor::no_offset_t, vendor::dynamic_offset_t<_database_t>>
				{
					static_assert(is_noop_t<Offset>::value, "cannot call offset()/dynamic_offset() twice");
					static_assert(not std::is_same<_database_t, void>::value, "dynamic_offset must not be called in a static statement");
					return { *this, vendor::dynamic_offset_t<_database_t>{} };
				}

			// value adding methods
			template<typename... Args>
				void add_flag(Args... args)
				{
					static_assert(is_select_flag_list_t<FlagList>::value, "cannot call add_flag() before dynamic_flags()");
					static_assert(is_dynamic_t<FlagList>::value, "cannot call add_flag() before dynamic_flags()");
					return _flag_list.add_flag(*this, args...);
				}

			template<typename... Args>
				void add_column(Args... args)
				{
					static_assert(is_select_column_list_t<ColumnList>::value, "cannot call add_column() before dynamic_columns()");
					static_assert(is_dynamic_t<ColumnList>::value, "cannot call add_column() before dynamic_columns()");
					return _column_list.add_column(*this, args...);
				}

			template<typename... Args>
				void add_from(Args... args)
				{
					static_assert(is_from_t<From>::value, "cannot call add_from() before dynamic_from()");
					static_assert(is_dynamic_t<From>::value, "cannot call add_using() before dynamic_from()");
					return _from.add_from(*this, args...);
				}

			template<typename... Args>
				void add_where(Args... args)
				{
					static_assert(is_where_t<Where>::value, "cannot call add_where() before dynamic_where()");
					static_assert(is_dynamic_t<Where>::value, "cannot call add_where() before dynamic_where()");
					return _where.add_where(*this, args...);
				}

			template<typename... Args>
				void add_group_by(Args... args)
				{
					static_assert(is_group_by_t<GroupBy>::value, "cannot call add_group_by() before dynamic_group_by()");
					static_assert(is_dynamic_t<GroupBy>::value, "cannot call add_group_by() before dynamic_group_by()");
					return _group_by.add_group_by(*this, args...);
				}

			template<typename... Args>
				void add_having(Args... args)
				{
					static_assert(is_having_t<Having>::value, "cannot call add_having() before dynamic_having()");
					static_assert(is_dynamic_t<Having>::value, "cannot call add_having() before dynamic_having()");
					return _having.add_having(*this, args...);
				}

			template<typename... Args>
				void add_order_by(Args... args)
				{
					static_assert(is_order_by_t<OrderBy>::value, "cannot call add_order_by() before dynamic_order_by()");
					static_assert(is_dynamic_t<OrderBy>::value, "cannot call add_order_by() before dynamic_order_by()");
					return _order_by.add_order_by(*this, args...);
				}

			template<typename Arg>
				void set_limit(Arg arg)
				{
					static_assert(is_limit_t<Limit>::value, "cannot call add_limit() before dynamic_limit()");
					static_assert(is_dynamic_t<Limit>::value, "cannot call add_limit() before dynamic_limit()");
					return _limit.set_limit(arg);
				}

			template<typename Arg>
				void set_offset(Arg arg)
				{
					static_assert(is_offset_t<Offset>::value, "cannot call add_offset() before dynamic_offset()");
					static_assert(is_dynamic_t<Offset>::value, "cannot call add_offset() before dynamic_offset()");
					return _offset.set_offset(arg);
				}

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

					/*
					static_assert(column_list::_table_set::template is_subset_t<_from_t::_table_set>::value
					static_assert(detail::is_subset_of<column_list::_table_set, _from_t::_table_set>::value
							subset_of_t sollte ein eigenes template sein, das macht so etwas wie obiges sicher einfacher lesbar
							also: use any and all instead of and_t and or_t
							*/
					//static_assert(is_where_t<Where>::value, "cannot select select without having a where condition, use .where(true) to remove all rows");
					//static_assert(not vendor::is_noop<ColumnList>::value, "cannot run select without having selected anything");
					//static_assert(is_from_t<From>::value, "cannot run select without a from()");
					//static_assert(is_where_t<Where>::value, "cannot run select without having a where condition, use .where(true) to select all rows");
					// FIXME: Check for missing aliases (if references are used)
					// FIXME: Check for missing tables, well, actually, check for missing tables at the where(), order_by(), etc.
			template<typename A>
				struct is_table_subset_of_from
				{
					static constexpr bool value = ::sqlpp::detail::is_subset_of<typename A::_table_set, typename From::_table_set>::value;
				};

			// Execute
			template<typename Db>
				auto _run(Db& db) const
				-> result_t<decltype(db.select(*this)), _result_row_t<Db>>
				{
#warning: need to check in add_xy method as well
#warning: need add_wxy_without_table_check
#warning: might want to add an .extra_tables() method to say which tables might also be used here, say via dynamic_from or because this is a subselect
					static_assert(is_table_subset_of_from<ColumnList>::value, "selected columns require additional tables in from()");
					static_assert(is_table_subset_of_from<Where>::value, "where() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<GroupBy>::value, "group_by() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<Having>::value, "having() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<OrderBy>::value, "order_by() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<Limit>::value, "limit() expression requires additional tables in from()");
					static_assert(is_table_subset_of_from<Offset>::value, "offset() expression requires additional tables in from()");
					static_assert(_get_static_no_of_parameters() == 0, "cannot run select directly with parameters, use prepare instead");
					return {db.select(*this), get_dynamic_names()};
				}

			// Prepare
			template<typename Db>
				auto _prepare(Db& db) const
				-> prepared_select_t<Db, select_t>
				{

					return {{}, get_dynamic_names(), db.prepare_select(*this)};
				}

			FlagList _flag_list;
			ColumnList _column_list;
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

	select_t<void> select() // FIXME: These should be constexpr
	{
		return { select_t<void>() };
	}

	template<typename... Columns>
		auto select(Columns... columns)
		-> select_t<void, vendor::no_select_flag_list_t, detail::make_select_column_list_t<void, Columns...>>
		{
			return { select_t<void>(), detail::make_select_column_list_t<void, Columns...>(std::tuple_cat(detail::as_tuple<Columns>::_(columns)...)) };
		}

	template<typename Database>
		select_t<Database> dynamic_select(const Database&)
		{
			return { select_t<Database>() };
		}

	template<typename Database, typename... Columns>
		auto dynamic_select(const Database&, Columns... columns)
		-> select_t<Database, vendor::no_select_flag_list_t, detail::make_select_column_list_t<void, Columns...>>
		{
			return { select_t<Database>(), detail::make_select_column_list_t<void, Columns...>(std::tuple_cat(detail::as_tuple<Columns>::_(columns)...)) };
		}

}
#endif

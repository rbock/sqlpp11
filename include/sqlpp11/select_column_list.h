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

#ifndef SQLPP_SELECT_COLUMN_LIST_H
#define SQLPP_SELECT_COLUMN_LIST_H

#include <tuple>
#include <sqlpp11/result_row.h>
#include <sqlpp11/table.h>
#include <sqlpp11/no_value.h>
#include <sqlpp11/field.h>
#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/select_pseudo_table.h>
#include <sqlpp11/named_interpretable.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/detail/copy_tuple_args.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename... Columns>
			struct select_traits
			{
				using _traits = make_traits<no_value_t, tag::select_column_list, tag::return_value>;
				struct _name_t {};
			};

		template<typename Column>
			struct select_traits<Column>
			{
				using _traits = make_traits<value_type_of<Column>, tag::select_column_list, tag::return_value, tag::expression, tag::named_expression>;
				using _name_t = typename Column::_name_t;
			};
	}

	template<typename Db>
		struct dynamic_select_column_list
		{
			using _names_t = std::vector<std::string>;
			std::vector<named_interpretable_t<Db>> _dynamic_columns;
			_names_t _dynamic_expression_names;

			template<typename Expr>
				void emplace_back(Expr expr)
				{
					_dynamic_expression_names.push_back(Expr::_name_t::_get_name());
					_dynamic_columns.emplace_back(expr);
				}

			bool empty() const
			{
				return _dynamic_columns.empty();
			}
		};

	template<>
		struct dynamic_select_column_list<void>
		{
			struct _names_t
			{
				static constexpr size_t size() { return 0; }
			};
			_names_t _dynamic_expression_names;

			static constexpr bool empty()
			{
				return true;
			}
		};

	template<typename Context, typename Db>
		struct serializer_t<Context, dynamic_select_column_list<Db>>
		{
			using T = dynamic_select_column_list<Db>;

			static Context& _(const T& t, Context& context)
			{
				bool first = true;
				for (const auto column : t._dynamic_columns)
				{
					if (first)
						first = false;
					else
						context << ',';
					serialize(column, context);
				}
				return context;
			}
		};

	template<typename Context>
		struct serializer_t<Context, dynamic_select_column_list<void>>
		{
			using T = dynamic_select_column_list<void>;

			static Context& _(const T& t, Context& context)
			{
				return context;
			}
		};

	// SELECTED COLUMNS DATA
	template<typename Database, typename... Columns>
		struct select_column_list_data_t
		{
			select_column_list_data_t(Columns... columns):
				_columns(columns...)
			{}

			select_column_list_data_t(std::tuple<Columns...> columns):
				_columns(columns)
			{}

			select_column_list_data_t(const select_column_list_data_t&) = default;
			select_column_list_data_t(select_column_list_data_t&&) = default;
			select_column_list_data_t& operator=(const select_column_list_data_t&) = default;
			select_column_list_data_t& operator=(select_column_list_data_t&&) = default;
			~select_column_list_data_t() = default;

			std::tuple<Columns...> _columns;
			dynamic_select_column_list<Database> _dynamic_columns;
		};


	// SELECTED COLUMNS
	template<typename Database, typename... Columns>
		struct select_column_list_t
		{
			using _traits = typename ::sqlpp::detail::select_traits<Columns...>::_traits;
			using _recursive_traits = make_recursive_traits<Columns...>;

			using _name_t = typename ::sqlpp::detail::select_traits<Columns...>::_name_t;

			using _is_dynamic = is_database<Database>;

			static_assert(_is_dynamic::value or sizeof...(Columns), "at least one select expression required");
			static_assert(not ::sqlpp::detail::has_duplicates<Columns...>::value, "at least one duplicate argument detected");
			static_assert(::sqlpp::detail::all_t<(is_named_expression_t<Columns>::value or is_multi_column_t<Columns>::value)...>::value, "at least one argument is not a named expression");
			static_assert(not ::sqlpp::detail::has_duplicates<typename Columns::_name_t...>::value, "at least one duplicate name detected");

			struct _column_type {};

			// Data
			using _data_t = select_column_list_data_t<Database, Columns...>;

			// Member implementation with data and methods
			template<typename Policies>
				struct _impl_t
				{
					template<typename NamedExpression>
						void add_ntc(NamedExpression namedExpression)
						{
							add<NamedExpression, std::false_type>(namedExpression);
						}

					template<typename NamedExpression, typename TableCheckRequired = std::true_type>
						void add(NamedExpression namedExpression)
						{
							static_assert(_is_dynamic::value, "selected_columns::add() can only be called for dynamic_column");
							static_assert(is_named_expression_t<NamedExpression>::value, "invalid named expression argument in selected_columns::add()");
							static_assert(TableCheckRequired::value or Policies::template _no_unknown_tables<NamedExpression>::value, "named expression uses tables unknown to this statement in selected_columns::add()");
							using column_names = ::sqlpp::detail::make_type_set_t<typename Columns::_name_t...>;
							static_assert(not ::sqlpp::detail::is_element_of<typename NamedExpression::_name_t, column_names>::value, "a column of this name is present in the select already");

							using ok = ::sqlpp::detail::all_t<
								_is_dynamic::value, 
								is_named_expression_t<NamedExpression>::value
									>;

							_add_impl(namedExpression, ok()); // dispatch to prevent compile messages after the static_assert
						}

					//private:
					template<typename NamedExpression>
						void _add_impl(NamedExpression namedExpression, const std::true_type&)
						{
							return _data._dynamic_columns.emplace_back(namedExpression);
						}

					template<typename NamedExpression>
						void _add_column_impl(NamedExpression namedExpression, const std::false_type&);

				public:
					_data_t _data;
				};

			// Member template for adding the named member to a statement
			template<typename Policies>
				struct _member_t
				{
					using _data_t = select_column_list_data_t<Database, Columns...>;

					_impl_t<Policies> selected_columns;
					_impl_t<Policies>& operator()() { return selected_columns; }
					const _impl_t<Policies>& operator()() const { return selected_columns; }

					template<typename T>
						static auto _get_member(T t) -> decltype(t.selected_columns)
						{
							return t.selected_columns;
						}
				};

			// Additional methods for the statement
			template<typename Policies>
				struct _methods_t
				{
					static void _check_consistency() {}
				};

			// Result methods
			template<typename Policies>
				struct _result_methods_t
				{
					using _statement_t = typename Policies::_statement_t;

					const _statement_t& _get_statement() const
					{
						return static_cast<const _statement_t&>(*this);
					}

					template<typename Db>
						using _result_row_t = typename std::conditional<_is_dynamic::value,
									dynamic_result_row_t<Db, make_field_t<Columns>...>,
									result_row_t<Db, make_field_t<Columns>...>>::type;

					using _dynamic_names_t = typename dynamic_select_column_list<Database>::_names_t;

					template<typename AliasProvider>
						struct _deferred_table_t
						{
							using table = select_pseudo_table_t<_statement_t, Columns...>;
							using alias = typename table::template _alias_t<AliasProvider>;
						};

					template<typename AliasProvider>
						using _table_t = typename _deferred_table_t<AliasProvider>::table;

					template<typename AliasProvider>
						using _alias_t = typename _deferred_table_t<AliasProvider>::alias;

					template<typename AliasProvider>
						_alias_t<AliasProvider> as(const AliasProvider& aliasProvider) const
						{
							static_assert(Policies::_can_be_used_as_table::value, "statement cannot be used as table, e.g. due to missing tables");
							static_assert(::sqlpp::detail::none_t<is_multi_column_t<Columns>::value...>::value, "cannot use multi-columns in sub selects");
							return _table_t<AliasProvider>(_get_statement()).as(aliasProvider);
						}

					const _dynamic_names_t& get_dynamic_names() const
					{
						return _get_statement().selected_columns._data._dynamic_columns._dynamic_expression_names;
					}

					size_t get_no_of_result_columns() const
					{
						return sizeof...(Columns) + get_dynamic_names().size();
					}

					// Execute
					template<typename Db>
						auto _run(Db& db) const
						-> result_t<decltype(db.select(this->_get_statement())), _result_row_t<Db>>
						{
							_statement_t::_check_consistency();
							static_assert(_statement_t::_get_static_no_of_parameters() == 0, "cannot run select directly with parameters, use prepare instead");

							return {db.select(_get_statement()), get_dynamic_names()};
						}

					// Prepare
					template<typename Db>
						auto _prepare(Db& db) const
						-> prepared_select_t<Db, _statement_t>
						{
							_statement_t::_check_consistency();

							return {{}, get_dynamic_names(), db.prepare_select(_get_statement())};
						}
				};

		};

	namespace detail
	{
		template<typename Database, typename... Columns>
			using make_select_column_list_t = 
			copy_tuple_args_t<select_column_list_t, Database, 
			decltype(std::tuple_cat(as_tuple<Columns>::_(std::declval<Columns>())...))>;
	}

	struct no_select_column_list_t
	{
		using _traits = make_traits<no_value_t, ::sqlpp::tag::noop, ::sqlpp::tag::missing>;
		using _recursive_traits = make_recursive_traits<>;

		struct _name_t {};

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

				_impl_t<Policies> no_selected_columns;
				_impl_t<Policies>& operator()() { return no_selected_columns; }
				const _impl_t<Policies>& operator()() const { return no_selected_columns; }

				template<typename T>
					static auto _get_member(T t) -> decltype(t.no_selected_columns)
					{
						return t.no_selected_columns;
					}
			};

		// Additional methods for the statement
		template<typename Policies>
			struct _methods_t
			{
				using _database_t = typename Policies::_database_t;
				template<typename T>
					using _new_statement_t = typename Policies::template _new_statement_t<no_select_column_list_t, T>;

				static void _check_consistency() {}

				template<typename... Args>
					auto columns(Args... args)
					-> _new_statement_t<::sqlpp::detail::make_select_column_list_t<void, Args...>>
					{
						return { *static_cast<typename Policies::_statement_t*>(this), typename ::sqlpp::detail::make_select_column_list_t<void, Args...>::_data_t{std::tuple_cat(::sqlpp::detail::as_tuple<Args>::_(args)...)} };
					}

				template<typename... Args>
					auto dynamic_columns(Args... args)
					-> _new_statement_t<::sqlpp::detail::make_select_column_list_t<_database_t, Args...>>
					{
						static_assert(not std::is_same<_database_t, void>::value, "dynamic_columns must not be called in a static statement");
						return { *static_cast<typename Policies::_statement_t*>(this), typename ::sqlpp::detail::make_select_column_list_t<_database_t, Args...>::_data_t{std::tuple_cat(::sqlpp::detail::as_tuple<Args>::_(args)...)} };
					}
			};
	};

	// Interpreters
	template<typename Context, typename Database, typename... Columns>
		struct serializer_t<Context, select_column_list_data_t<Database, Columns...>>
		{
			using T = select_column_list_data_t<Database, Columns...>;

			static Context& _(const T& t, Context& context)
			{
				interpret_tuple(t._columns, ',', context);
				if (sizeof...(Columns) and not t._dynamic_columns.empty())
					context << ',';
				serialize(t._dynamic_columns, context);
				return context;
			}
		};

}

#endif

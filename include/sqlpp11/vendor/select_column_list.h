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
#include <sqlpp11/vendor/field.h>
#include <sqlpp11/vendor/expression_fwd.h>
#include <sqlpp11/vendor/select_pseudo_table.h>
#include <sqlpp11/vendor/named_interpretable.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/policy_update.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/detail/copy_tuple_args.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename... Rest>
			struct get_first_argument_if_unique
			{
				using _traits = make_traits<no_value_t, tag::select_column_list>;
				struct _name_t {};
			};

		template<typename T>
			struct get_first_argument_if_unique<T>
			{
				using _traits = make_traits<value_type_of<T>, tag::select_column_list, tag::expression, tag::named_expression>;
				using _name_t = typename T::_name_t;
			};
	}

	namespace vendor
	{
		template<typename Db>
			struct dynamic_select_column_list
			{
				using _names_t = std::vector<std::string>;
				std::vector<vendor::named_interpretable_t<Db>> _dynamic_columns;
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


		// SELECT COLUMNS
		template<typename Database, typename... Columns>
			struct select_column_list_t
			{
				// get_first_argument_if_unique is kind of ugly
				using _traits = typename ::sqlpp::detail::get_first_argument_if_unique<Columns...>::_traits;
				using _recursive_traits = make_recursive_traits<Columns...>;

				using _name_t = typename ::sqlpp::detail::get_first_argument_if_unique<Columns...>::_name_t;

				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

				static_assert(not ::sqlpp::detail::has_duplicates<Columns...>::value, "at least one duplicate argument detected");

				template<typename T>
					using is_valid_expression_t = std::integral_constant<bool, is_named_expression_t<T>::value or is_multi_column_t<T>::value>;
				static_assert(::sqlpp::detail::all_t<is_valid_expression_t<Columns>::value...>::value, "at least one argument is not a named expression");

				static_assert(not ::sqlpp::detail::has_duplicates<typename Columns::_name_t...>::value, "at least one duplicate name detected");

				struct _column_type {};


				template<typename Db>
				using _result_row_t = typename std::conditional<_is_dynamic::value,
							dynamic_result_row_t<Db, make_field_t<Columns>...>,
							result_row_t<Db, make_field_t<Columns>...>>::type;

				using _dynamic_names_t = typename dynamic_select_column_list<Database>::_names_t;

				template <typename Select>
					using _pseudo_table_t = select_pseudo_table_t<Select, Columns...>;

				template <typename Db>
					using _dynamic_t = select_column_list_t<Db, std::tuple<Columns...>>;

				select_column_list_t(std::tuple<Columns...> columns):
					_columns(columns)
				{}

				select_column_list_t(Columns... columns):
					_columns(columns...)
				{}

				select_column_list_t(const select_column_list_t&) = default;
				select_column_list_t(select_column_list_t&&) = default;
				select_column_list_t& operator=(const select_column_list_t&) = default;
				select_column_list_t& operator=(select_column_list_t&&) = default;
				~select_column_list_t() = default;

				static constexpr size_t static_size()
				{
					return sizeof...(Columns);
				}

				template<typename Policies>
					struct _methods_t
					{
						template<typename NamedExpression>
							void add_column_ntc(NamedExpression namedExpression)
							{
								add_column<NamedExpression, std::false_type>(namedExpression);
							}

						template<typename NamedExpression, typename TableCheckRequired = std::true_type>
							void add_column(NamedExpression namedExpression)
							{
								static_assert(_is_dynamic::value, "add_column can only be called for dynamic_column");
								static_assert(is_named_expression_t<NamedExpression>::value, "invalid named expression argument in add_column()");
								static_assert(TableCheckRequired::value or Policies::template _no_unknown_tables<NamedExpression>::value, "named expression uses tables unknown to this statement in add_column()");
								using column_names = ::sqlpp::detail::make_type_set_t<typename Columns::_name_t...>;
								static_assert(not ::sqlpp::detail::is_element_of<typename NamedExpression::_name_t, column_names>::value, "a column of this name is present in the select already");

								using ok = ::sqlpp::detail::all_t<
											_is_dynamic::value, 
											is_named_expression_t<NamedExpression>::value
												>;

								_add_column_impl(namedExpression, ok()); // dispatch to prevent compile messages after the static_assert
							}

					private:
						template<typename NamedExpression>
							void _add_column_impl(NamedExpression namedExpression, const std::true_type&)
							{
								return static_cast<typename Policies::_statement_t*>(this)->_column_list._dynamic_columns.emplace_back(namedExpression);
							}

						template<typename NamedExpression>
							void _add_column_impl(NamedExpression namedExpression, const std::false_type&);
					};


				const select_column_list_t& _column_list() const { return *this; }
				std::tuple<Columns...> _columns;
				dynamic_select_column_list<Database> _dynamic_columns;
			};
	}

	namespace detail
	{
		template<typename Database, typename... Columns>
			using make_select_column_list_t = 
			copy_tuple_args_t<vendor::select_column_list_t, Database, 
			decltype(std::tuple_cat(as_tuple<Columns>::_(std::declval<Columns>())...))>;
	}

	namespace vendor
	{
		struct no_select_column_list_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::noop>;
			using _recursive_traits = make_recursive_traits<>;

			template<typename Db>
				using _result_row_t = ::sqlpp::result_row_t<Db>;
			using _dynamic_names_t = typename dynamic_select_column_list<void>::_names_t;
			struct _name_t {};

			template<typename T>
				struct _pseudo_table_t
				{
					static_assert(wrong_t<T>::value, "Cannot use a select as a table when no columns have been selected yet");
				};

			template<typename Policies>
				struct _methods_t
				{
					using _database_t = typename Policies::_database_t;
					template<typename T>
					using _new_statement_t = typename Policies::template _new_statement_t<no_select_column_list_t, T>;

					template<typename... Args>
						auto columns(Args... args)
						-> _new_statement_t<::sqlpp::detail::make_select_column_list_t<void, Args...>>
						{
							return { *static_cast<typename Policies::_statement_t*>(this), ::sqlpp::detail::make_select_column_list_t<void, Args...>{std::tuple_cat(::sqlpp::detail::as_tuple<Args>::_(args)...)} };
						}

					template<typename... Args>
						auto dynamic_columns(Args... args)
						-> _new_statement_t<::sqlpp::detail::make_select_column_list_t<_database_t, Args...>>
						{
							static_assert(not std::is_same<_database_t, void>::value, "dynamic_columns must not be called in a static statement");
							return { *static_cast<typename Policies::_statement_t*>(this), ::sqlpp::detail::make_select_column_list_t<_database_t, Args...>{std::tuple_cat(::sqlpp::detail::as_tuple<Args>::_(args)...)} };
						}
				};
		};

		// Interpreters
		template<typename Context, typename Database, typename... Columns>
			struct serializer_t<Context, select_column_list_t<Database, Columns...>>
			{
				using T = select_column_list_t<Database, Columns...>;

				static Context& _(const T& t, Context& context)
				{
					// check for at least one expression
					static_assert(T::_is_dynamic::value or sizeof...(Columns), "at least one select expression required");

					interpret_tuple(t._columns, ',', context);
					if (sizeof...(Columns) and not t._dynamic_columns.empty())
						context << ',';
					serialize(t._dynamic_columns, context);
					return context;
				}
			};

		template<typename Context>
			struct serializer_t<Context, no_select_column_list_t>
			{
				using T = no_select_column_list_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};
	}
}

#endif

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

#ifndef SQLPP_SELECT_COLUMN_LIST_H
#define SQLPP_SELECT_COLUMN_LIST_H

#include <tuple>
#include <sqlpp11/result_row.h>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/table.h>
#include <sqlpp11/no_value.h>
#include <sqlpp11/vendor/field.h>
#include <sqlpp11/vendor/expression_fwd.h>
#include <sqlpp11/vendor/select_pseudo_table.h>
#include <sqlpp11/vendor/named_interpretable.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename... Rest>
			struct get_first_argument_if_unique
			{
				using _value_type = no_value_t;
				struct _name_t {};
			};

		template<typename T>
			struct get_first_argument_if_unique<T>
			{
				using _value_type = typename T::_value_type;
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
					void push_back(Expr expr)
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

				template<typename T>
					void push_back(const T&) {}

				static constexpr bool empty()
				{
					return true;
				}
			};

		template<typename Context, typename Db>
			struct interpreter_t<Context, dynamic_select_column_list<Db>>
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
						interpret(column, context);
					}
					return context;
				}
			};

		template<typename Context>
			struct interpreter_t<Context, dynamic_select_column_list<void>>
			{
				using T = dynamic_select_column_list<void>;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};


		template<typename Database, typename T>
			struct select_column_list_t
			{
				static_assert(::sqlpp::vendor::wrong_t<Database, T>::value, "invalid template argument for select_column_list");
			};

		template<typename Database, typename... NamedExpr>
			struct select_column_list_t<Database, std::tuple<NamedExpr...>>
			{
				using _is_select_column_list = std::true_type;
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
				using _parameter_tuple_t = std::tuple<NamedExpr...>;
				using size = std::tuple_size<_parameter_tuple_t>;

				// check for duplicate select expressions
				static_assert(not ::sqlpp::detail::has_duplicates<NamedExpr...>::value, "at least one duplicate argument detected");

				// check for invalid select expressions
				template<typename T>
					using is_valid_expression_t = std::integral_constant<bool, is_named_expression_t<T>::value or is_multi_column_t<T>::value>;
				static_assert(::sqlpp::detail::and_t<is_valid_expression_t, NamedExpr...>::value, "at least one argument is not a named expression");

				// check for duplicate select expression names
				static_assert(not ::sqlpp::detail::has_duplicates<typename NamedExpr::_name_t...>::value, "at least one duplicate name detected");

				// provide type information for sub-selects that are used as expressions
				struct _column_type {};
				struct _value_type: ::sqlpp::detail::get_first_argument_if_unique<NamedExpr...>::_value_type
				{
					using _is_expression = typename std::conditional<sizeof...(NamedExpr) == 1, std::true_type, std::false_type>::type;
					using _is_named_expression = typename std::conditional<sizeof...(NamedExpr) == 1, std::true_type, std::false_type>::type;
					using _is_alias = std::false_type;
				};
				using _name_t = typename ::sqlpp::detail::get_first_argument_if_unique<NamedExpr...>::_name_t;

				using _result_row_t = typename std::conditional<_is_dynamic::value,
							dynamic_result_row_t<make_field_t<NamedExpr>...>,
							result_row_t<make_field_t<NamedExpr>...>>::type;

				using _dynamic_names_t = typename dynamic_select_column_list<Database>::_names_t;

				template <typename Select>
					using _pseudo_table_t = select_pseudo_table_t<Select, NamedExpr...>;

				template <typename Db>
					using _dynamic_t = select_column_list_t<Db, std::tuple<NamedExpr...>>;

				template<typename Expr>
					void add(Expr namedExpr)
					{
						static_assert(is_named_expression_t<Expr>::value, "select() arguments require to be named expressions");
						static_assert(_is_dynamic::value, "cannot add columns to a non-dynamic column list");
						_dynamic_columns.push_back(namedExpr);
					}

				_parameter_tuple_t _columns;
				dynamic_select_column_list<Database> _dynamic_columns;
			};

		template<typename Context, typename Database, typename Tuple>
			struct interpreter_t<Context, select_column_list_t<Database, Tuple>>
			{
				using T = select_column_list_t<Database, Tuple>;

				static Context& _(const T& t, Context& context)
				{
					// check for at least one expression
					static_assert(T::_is_dynamic::value or T::size::value, "at least one select expression required");

					interpret_tuple(t._columns, ',', context);
					if (T::size::value and not t._dynamic_columns.empty())
						context << ',';
					interpret(t._dynamic_columns, context);
					return context;
				}
			};
	}
}

#endif

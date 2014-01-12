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

#ifndef SQLPP_SELECT_EXPRESSION_LIST_H
#define SQLPP_SELECT_EXPRESSION_LIST_H

#include <tuple>
#include <ostream>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/no_value.h>
#include <sqlpp11/field.h>
#include <sqlpp11/result_row.h>
#include <sqlpp11/table_base.h>
#include <sqlpp11/select_pseudo_table.h>
#include <sqlpp11/detail/named_serializable.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/detail/set.h>

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

	template<typename Db>
		struct dynamic_select_expression_list
		{
			using _names_t = std::vector<std::string>;
			std::vector<detail::named_serializable_t<Db>> _dynamic_expressions;
			_names_t _dynamic_expression_names;

			template<typename Expr>
				void push_back(Expr&& expr)
				{
					_dynamic_expression_names.push_back(std::decay<Expr>::type::_name_t::_get_name());
					_dynamic_expressions.emplace_back(std::forward<Expr>(expr));
				}

			bool empty() const
			{
				return _dynamic_expressions.empty();
			}
		};

		template<>
			struct dynamic_select_expression_list<void>
			{
				struct _names_t {};
				_names_t _dynamic_expression_names;

				template<typename T>
					void push_back(const T&) {}

				static constexpr bool empty()
				{
					return true;
				}
			};

	template<typename Context, typename Db>
		struct interpreter_t<Context, dynamic_select_expression_list<Db>>
		{
			using T = dynamic_select_expression_list<Db>;

			static Context& _(const T& t, Context& context)
			{
				for (const auto column : t._dynamic_expressions)
				{
					interpret(column, context);
				}
				return context;
			}
		};

	template<typename Context>
		struct interpreter_t<Context, dynamic_select_expression_list<void>>
		{
			using T = dynamic_select_expression_list<void>;

			static Context& _(const T& t, Context& context)
			{
				return context;
			}
		};


	template<typename Database, typename... NamedExpr>
		struct select_expression_list_t<Database, std::tuple<NamedExpr...>>
		{
			using _is_select_expression_list = std::true_type;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
			using _parameter_tuple_t = std::tuple<NamedExpr...>;

			// check for at least one expression
			static_assert(_is_dynamic::value or sizeof...(NamedExpr), "at least one select expression required");

			// check for duplicate select expressions
			static_assert(not detail::has_duplicates<NamedExpr...>::value, "at least one duplicate argument detected");

			// check for invalid select expressions
			template<typename T>
				struct is_valid_expression_t: public std::integral_constant<bool, is_named_expression_t<T>::value or is_multi_column_t<T>::value> {};
			using _valid_expressions = typename detail::make_set_if<is_valid_expression_t, NamedExpr...>::type;
			static_assert(_valid_expressions::size::value == sizeof...(NamedExpr), "at least one argument is not a named expression");

			// check for duplicate select expression names
			static_assert(not detail::has_duplicates<typename NamedExpr::_name_t...>::value, "at least one duplicate name detected");
			
			// provide type information for sub-selects that are used as expressions
			struct _column_type {};
			struct _value_type: detail::get_first_argument_if_unique<NamedExpr...>::_value_type
			{
				using _is_expression = typename std::conditional<sizeof...(NamedExpr) == 1, std::true_type, std::false_type>::type;
				using _is_named_expression = typename std::conditional<sizeof...(NamedExpr) == 1, std::true_type, std::false_type>::type;
				using _is_alias = std::false_type;
			};
			using _name_t = typename detail::get_first_argument_if_unique<NamedExpr...>::_name_t;

			using _result_row_t = typename std::conditional<_is_dynamic::value,
				dynamic_result_row_t<make_field_t<NamedExpr>...>,
				result_row_t<make_field_t<NamedExpr>...>>::type;

			using _dynamic_names_t = typename dynamic_select_expression_list<Database>::_names_t;

			template <typename Select>
				using _pseudo_table_t = select_pseudo_table_t<Select, NamedExpr...>;

			template <typename Db>
				using _dynamic_t = select_expression_list_t<Db, std::tuple<NamedExpr...>>;

			template<typename Expr>
				void add(Expr&& namedExpr)
				{
					static_assert(is_named_expression_t<typename std::decay<Expr>::type>::value, "select() arguments require to be named expressions");
					static_assert(_is_dynamic::value, "cannot add columns to a non-dynamic column list");
					_dynamic_expressions.push_back(std::forward<Expr>(namedExpr));
				}

			_parameter_tuple_t _expressions;
			dynamic_select_expression_list<Database> _dynamic_expressions;
		};

	template<typename Context, typename Database, typename... NamedExpr>
		struct interpreter_t<Context, select_expression_list_t<Database, NamedExpr...>>
		{
			using T = select_expression_list_t<Database, NamedExpr...>;

			static Context& _(const T& t, Context& context)
			{
				interpret_tuple(t._expressions, ',', context);
				if (not t._dynamic_expressions.empty())
					context << ',';
				interpret(t._dynamic_expressions, context);
				return context;
			}
		};

}

#endif

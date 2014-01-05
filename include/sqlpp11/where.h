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

#ifndef SQLPP_WHERE_H
#define SQLPP_WHERE_H

#include <ostream>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/expression.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/set.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/detail/serializable_list.h>
#include <sqlpp11/parameter_list.h>

namespace sqlpp
{
	template<typename Database, typename... Expr>
		struct where_t
		{
			using _is_where = std::true_type;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
			using _parameter_tuple_t = std::tuple<Expr...>;

			static_assert(_is_dynamic::value or sizeof...(Expr), "at least one expression argument required in where()");
			using _valid_expressions = typename detail::make_set_if<is_expression_t, Expr...>::type;
			static_assert(_valid_expressions::size::value == sizeof...(Expr), "at least one argument is not an expression in where()");

			using _parameter_list_t = typename make_parameter_list_t<_parameter_tuple_t>::type;
			static_assert(not _parameter_list_t::_contains_trivial_value_is_null_t::value, "must not use trivial_value_is_null in parameters of where expression, use where_parameter() instead of parameter() to turn off automatic conversion");

			template<typename E>
				void add(E&& expr)
				{
					static_assert(is_expression_t<typename std::decay<E>::type>::value, "invalid expression argument in add_where()");
					_dynamic_expressions.emplace_back(std::forward<E>(expr));
				}

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					if (sizeof...(Expr) == 0 and _dynamic_expressions.empty())
						return;
					os << " WHERE ";
					detail::serialize_tuple(os, db, _expressions, " AND ");
					_dynamic_expressions.serialize(os, db, " AND ", sizeof...(Expr) == 0);
				}

			size_t _set_parameter_index(size_t index)
			{
				return set_parameter_index(_expressions, index);
			}

			_parameter_tuple_t _expressions;
			detail::serializable_list<Database> _dynamic_expressions;
		};
}

#endif

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

#ifndef SQLPP_ORDER_BY_H
#define SQLPP_ORDER_BY_H

#include <tuple>
#include <ostream>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/detail/serializable.h>

namespace sqlpp
{
	template<typename Database,typename... Expr>
		struct order_by_t
		{
			using _is_order_by = std::true_type;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

			// check for at least one order expression
			static_assert(_is_dynamic::value or sizeof...(Expr), "at least one sort-order expression required in order_by()");

			// check for duplicate order expressions
			static_assert(not detail::has_duplicates<Expr...>::value, "at least one duplicate argument detected in order_by()");

			// check for invalid order expressions
			using _valid_expressions = typename detail::make_set_if<is_sort_order_t, Expr...>::type;
			static_assert(_valid_expressions::size::value == sizeof...(Expr), "at least one argument is not a sort order expression in order_by()");

			template<typename E>
				void add(E&& expr)
				{
					static_assert(is_sort_order_t<typename std::decay<E>::type>::value, "order_by arguments require to be sort-order expressions");
					_dynamic_expressions.push_back(std::forward<E>(expr));
				}

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					static_assert(Db::_supports_order_by, "order by not supported by current database");
					if (sizeof...(Expr) == 0 and _dynamic_expressions.empty())
						return;

					os << " ORDER BY ";
					detail::serialize_tuple(os, db, _expressions, ',');
					_dynamic_expressions.serialize(os, db, sizeof...(Expr) == 0);
				}

			std::tuple<Expr...> _expressions;
			detail::serializable_list<Database> _dynamic_expressions;
		};

}

#endif

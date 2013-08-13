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
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
	template<typename... OrderExpr>
		struct order_by_t
		{
			// check for at least one order expression
			static_assert(sizeof...(OrderExpr), "at least one select expression required in order_by()");

			// check for duplicate order expressions
			static_assert(not detail::has_duplicates<OrderExpr...>::value, "at least one duplicate argument detected in order_by()");

			// check for invalid order expressions
			using _valid_expressions = typename detail::make_set_if<is_sort_order_t, OrderExpr...>::type;
			static_assert(_valid_expressions::size::value == sizeof...(OrderExpr), "at least one argument is not a sort order expression in order_by()");

			using _is_order_by = tag_yes;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << " ORDER BY ";
					detail::serialize_tuple(os, db, _orderExpressions, ',');
				}

			std::tuple<OrderExpr...> _orderExpressions;
		};
}

#endif

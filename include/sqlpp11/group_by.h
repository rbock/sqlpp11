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

#ifndef SQLPP_GROUP_BY_H
#define SQLPP_GROUP_BY_H

#include <ostream>
#include <vector>
#include <tuple>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/expression.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/set.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/detail/serializable.h>

namespace sqlpp
{
	template<typename... Expr>
		struct group_by_t
		{
			// ensure one argument at least
			static_assert(sizeof...(Expr), "at least one expression (e.g. a column) required in group_by()");

			// check for duplicate expressions
			static_assert(not detail::has_duplicates<Expr...>::value, "at least one duplicate argument detected in group_by()");

			// check for invalid expressions
			using _valid_expressions = typename detail::make_set_if<is_expression_t, Expr...>::type;
			static_assert(_valid_expressions::size::value == sizeof...(Expr), "at least one argument is not an expression in group_by()");

			using _is_group_by = std::true_type;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << " GROUP BY ";
					detail::serialize_tuple(os, db, _expressions, ',');
				}

			std::tuple<Expr...> _expressions;

		};

	template<typename Db>
		struct dynamic_group_by_t
		{
			using _is_group_by = std::true_type;
			using _is_dynamic = std::true_type;

			template<typename Expr>
				void add(Expr&& expr)
				{
					static_assert(is_expression_t<typename std::decay<Expr>::type>::value, "group_by arguments require to be expressions");
					_expressions.push_back(std::forward<Expr>(expr));
				}

			void serialize(std::ostream& os, Db& db) const
			{
				if (_expressions.empty())
					return;
				os << " GROUP BY ";
				bool first = true;
				for (const auto& expr : _expressions)
				{
					if (not first)
						os << ',';
					expr.serialize(os, db);
					first = false;
				}
			}

			std::vector<detail::serializable_t<Db>> _expressions;
		};

}

#endif

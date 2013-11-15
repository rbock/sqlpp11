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
#include <sqlpp11/detail/serializable_list.h>

namespace sqlpp
{
	template<typename Expr>
		struct where_t
		{
			static_assert(is_expression_t<Expr>::value, "invalid expression argument in where()");

			using _is_where = std::true_type;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << " WHERE ";
					_expr.serialize(os, db);
				}

			Expr _expr;
		};

	template<typename Db>
	struct dynamic_where_t
	{

		using _is_where = std::true_type;
		using _is_dynamic = std::true_type;

		template<typename Expr>
			void add(Expr&& expr)
			{
				static_assert(is_expression_t<typename std::decay<Expr>::type>::value, "invalid expression argument in where()");
				_conditions.emplace_back(std::forward<Expr>(expr));
			}

		void serialize(std::ostream& os, Db& db) const
		{
			if (_conditions.empty())
				return;

			os << " WHERE ";
			bool first = true;
			_conditions.serialize(os, db, " AND ", true);
		}

		detail::serializable_list<Db> _conditions;
	};

}

#endif

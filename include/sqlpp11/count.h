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

#ifndef SQLPP_COUNT_H
#define SQLPP_COUNT_H

#include <sstream>
#include <sqlpp11/numeric.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename Expr>
		struct count_t: public numeric::template operators<count_t<Expr>>
		{
			static_assert(is_value_t<Expr>::value, "count() requires a sql value as argument");

			struct _value_type: public numeric
			{
				using _is_named_expression = tag_yes;
			};

			struct _name_t
			{
				static constexpr const char* _get_name() { return "COUNT"; }
				template<typename T>
					struct _member_t
					{
						T count;
					};
			};

			count_t(Expr&& expr):
				_expr(std::move(expr))
			{}

			count_t(const Expr& expr):
				_expr(expr)
			{}

			count_t(const count_t&) = default;
			count_t(count_t&&) = default;
			count_t& operator=(const count_t&) = default;
			count_t& operator=(count_t&&) = default;
			~count_t() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << "COUNT(";
					_expr.serialize(os, db);
					os << ")";
				}

		private:
			Expr _expr;
		};
	}

	template<typename T>
	auto count(T&& t) -> typename detail::count_t<typename operand_t<T, is_value_t>::type>
	{
		return { std::forward<T>(t) };
	}

}

#endif

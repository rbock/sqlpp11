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

#ifndef SQLPP_DETAIL_MAKE_EXPRESSION_TUPLE_H
#define SQLPP_DETAIL_MAKE_EXPRESSION_TUPLE_H

namespace sqlpp
{
	namespace detail
	{
		template<typename Expr>
			auto make_single_expression_tuple(Expr expr)
			-> typename std::enable_if<is_named_expression_t<Expr>::value, decltype(std::make_tuple(expr))>::type
			{
				return std::make_tuple(expr);
			};

		template<typename Expr>
			auto make_single_expression_tuple(Expr expr)
			-> typename std::enable_if<is_select_flag_t<Expr>::value, std::tuple<>>::type
			{
				return {};
			};

		template<typename... Expr>
			auto make_single_expression_tuple(std::tuple<Expr...> t)
			-> std::tuple<Expr...>
			{
				return t;
			};

		template<typename... Expr>
			auto make_expression_tuple(Expr... expr)
			-> decltype(std::tuple_cat(make_single_expression_tuple(expr)...))
			{
				return std::tuple_cat(make_single_expression_tuple(expr)...);
			};

	}
}
#endif

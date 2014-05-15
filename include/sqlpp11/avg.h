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

#ifndef SQLPP_AVG_H
#define SQLPP_AVG_H

#include <sqlpp11/type_traits.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename Flag, typename Expr>
		struct avg_t: public floating_point::template expression_operators<avg_t<Flag, Expr>>
		{
			using _traits = make_traits_t<value_type_of<Expr>, tag::expression, tag::named_expression>;
			using _recursive_traits = make_recursive_traits_t<Select>;

			static_assert(is_noop<Flag>::value or std::is_same<sqlpp::distinct_t, Flag>::value, "avg() used with flag other than 'distinct'");
			static_assert(is_numeric_t<Expr>::value, "avg() requires a value expression as argument");

			struct _name_t
			{
				static constexpr const char* _get_name() { return "AVG"; }
				template<typename T>
					struct _member_t
					{
						T avg;
						T& operator()() { return avg; }
						const T& operator()() const { return avg; }
					};
			};

			avg_t(Expr expr):
				_expr(expr)
			{}

			avg_t(const avg_t&) = default;
			avg_t(avg_t&&) = default;
			avg_t& operator=(const avg_t&) = default;
			avg_t& operator=(avg_t&&) = default;
			~avg_t() = default;

			Expr _expr;
		};
	}

	namespace vendor
	{
		template<typename Context, typename Flag, typename Expr>
			struct serializer_t<Context, vendor::avg_t<Flag, Expr>>
			{
				using T = vendor::avg_t<Flag, Expr>;

				static Context& _(const T& t, Context& context)
				{
					context << "AVG(";
					if (std::is_same<sqlpp::distinct_t, Flag>::value)
					{
						serialize(Flag(), context);
						context << ' ';
					}
					serialize(t._expr, context);
					context << ")";
					return context;
				}
			};
	}

	template<typename T>
		auto avg(T t) -> typename vendor::avg_t<vendor::noop, vendor::wrap_operand_t<T>>
		{
			static_assert(is_numeric_t<vendor::wrap_operand_t<T>>::value, "avg() requires a value expression as argument");
			return { t };
		}

	template<typename T>
		auto avg(const sqlpp::distinct_t&, T t) -> typename vendor::avg_t<sqlpp::distinct_t, vendor::wrap_operand_t<T>>
		{
			static_assert(is_numeric_t<vendor::wrap_operand_t<T>>::value, "avg() requires a value expression as argument");
			return { t };
		}

}

#endif

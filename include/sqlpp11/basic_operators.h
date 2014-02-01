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

#ifndef SQLPP_DETAIL_BASIC_OPERATORS_H
#define SQLPP_DETAIL_BASIC_OPERATORS_H

#include <sqlpp11/alias.h>
#include <sqlpp11/sort_order.h>
#include <sqlpp11/vendor/expression_fwd.h>
#include <sqlpp11/vendor/in_fwd.h>
#include <sqlpp11/vendor/is_null_fwd.h>

namespace sqlpp
{

	namespace detail
	{
		struct boolean;
	}

	// basic operators
	template<typename Base, template<typename> class Constraint>
		struct basic_operators
		{
			template<typename T>
				vendor::equal_t<Base, typename Constraint<T>::type> operator==(T t) const
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
					return { *static_cast<const Base*>(this), {t}	};
				}

			template<typename T>
				vendor::not_equal_t<Base, typename Constraint<T>::type> operator!=(T t) const
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
					return { *static_cast<const Base*>(this), {t} };
				}
			template<typename T>
				vendor::less_than_t<Base, typename Constraint<T>::type> operator<(T t) const
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
					return { *static_cast<const Base*>(this), {t} };
				}

			template<typename T>
				vendor::less_equal_t<Base, typename Constraint<T>::type> operator<=(T t) const
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
					return { *static_cast<const Base*>(this), {t} };
				}

			template<typename T>
				vendor::greater_than_t<Base, typename Constraint<T>::type> operator>(T t) const
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
					return { *static_cast<const Base*>(this), {t} };
				}

			template<typename T>
				vendor::greater_equal_t<Base, typename Constraint<T>::type> operator>=(T t) const
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
					return { *static_cast<const Base*>(this), {t} };
				}

			vendor::is_null_t<true, Base> is_null() const
			{
				static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used with is_null()");
				return { *static_cast<const Base*>(this) };
			}

			vendor::is_null_t<false, Base> is_not_null() const
			{
				static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used with is_not_null()");
				return { *static_cast<const Base*>(this) };
			}

			sort_order_t<Base, sort_type::asc> asc()
			{ 
				static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used for sorting");
				return { *static_cast<const Base*>(this) };
			}

			sort_order_t<Base, sort_type::desc> desc()
			{ 
				static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used for sorting");
				return { *static_cast<const Base*>(this) };
			}

			// Hint: use value_list wrapper for containers...
			template<typename... T>
				vendor::in_t<true, Base, typename Constraint<T>::type...> in(T... t) const
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used with in()");
					return { *static_cast<const Base*>(this), {t}... };
				}

			template<typename... T>
				vendor::in_t<false, Base, typename Constraint<T>::type...> not_in(T... t) const
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot with be used with not_in()");
					return { *static_cast<const Base*>(this), {t}... };
				}

			template<typename alias_provider>
				expression_alias_t<Base, alias_provider> as(const alias_provider&)
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot have a name");
					return { *static_cast<const Base*>(this) };
				}
		};

}
#endif

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

#include <sqlpp11/expression.h>
#include <sqlpp11/alias.h>
#include <sqlpp11/sort_order.h>
#include <sqlpp11/in.h>
#include <sqlpp11/is_null.h>

namespace sqlpp
{

	namespace detail
	{
		struct boolean;

		// operators
		struct lt_
		{
			using _value_type = boolean;
			static constexpr const char* _name = "<";
		};

		struct le_
		{
			using _value_type = boolean;
			static constexpr const char* _name = "<=";
		};

		struct ge_
		{
			using _value_type = boolean;
			static constexpr const char* _name = ">=";
		};

		struct gt_
		{
			using _value_type = boolean;
			static constexpr const char* _name = ">";
		};

		struct is_not_null_
		{
			using _value_type = boolean;
			static constexpr const char* _name = "IS NOT NULL";
		};


		// basic operators
		template<typename Base, template<typename> class Constraint>
			struct basic_operators
		{
			template<typename T>
				equal_t<Base, typename Constraint<T>::type> operator==(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}

			template<typename T>
				not_equal_t<Base, typename Constraint<T>::type> operator!=(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}
			template<typename T>
				binary_expression_t<Base, lt_, typename Constraint<T>::type> operator<(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}

			template<typename T>
				binary_expression_t<Base, le_, typename Constraint<T>::type> operator<=(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}

			template<typename T>
				binary_expression_t<Base, ge_, typename Constraint<T>::type> operator>=(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}

			template<typename T>
				binary_expression_t<Base, gt_, typename Constraint<T>::type> operator>(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}

			is_null_t<true, boolean, Base> is_null() const
			{
				return { *static_cast<const Base*>(this) };
			}

			is_null_t<false, boolean, Base> is_not_null() const
			{
				return { *static_cast<const Base*>(this) };
			}

			sort_order_t<Base, sort_type::asc> asc()
			{ 
				return { *static_cast<const Base*>(this) };
			}

			sort_order_t<Base, sort_type::desc> desc()
			{ 
				return { *static_cast<const Base*>(this) };
			}

			// Hint: use value_list wrapper for containers...
			template<typename... T>
				in_t<true, boolean, Base, typename Constraint<T>::type...> in(T&&... t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t)... };
				}

			template<typename... T>
				in_t<false, boolean, Base, typename Constraint<T>::type...> not_in(T&&... t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t)... };
				}

			template<typename alias_provider>
				expression_alias_t<Base, typename std::decay<alias_provider>::type> as(alias_provider&&)
				{
					static_assert(not is_nameless_expression_t<Base>::value, "expression cannot have a name, e.g. like any()");
					return { *static_cast<const Base*>(this) };
				}

			constexpr bool _is_trivial() const { return false; }
		};
	}

}
#endif

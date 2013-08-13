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

#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/alias.h>
#include <sqlpp11/sort_order.h>

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

		struct is_null_
		{
			using _value_type = boolean;
			static constexpr const char* _name = "IS NULL";
		};

		struct is_not_null_
		{
			using _value_type = boolean;
			static constexpr const char* _name = "IS NOT NULL";
		};

		struct in_
		{
			using _value_type = boolean;
			static constexpr const char* _name = "IN";
		};

		struct not_in_
		{
			using _value_type = boolean;
			static constexpr const char* _name = "NOT IN";
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
				nary_expression_t<Base, lt_, typename Constraint<T>::type> operator<(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}

			template<typename T>
				nary_expression_t<Base, le_, typename Constraint<T>::type> operator<=(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}

			template<typename T>
				nary_expression_t<Base, ge_, typename Constraint<T>::type> operator>=(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}

			template<typename T>
				nary_expression_t<Base, gt_, typename Constraint<T>::type> operator>(T&& t) const
				{
					return { *static_cast<const Base*>(this), std::forward<T>(t) };
				}

			nary_expression_t<Base, is_null_> is_null() const
			{
				return { *static_cast<const Base*>(this) };
			}

			nary_expression_t<Base, is_not_null_> is_not_null() const
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

			// Hint: use wrappers for containers...
			template<typename... T>
				nary_expression_t<Base, in_, typename Constraint<T>::type...> in(T&&... t) const
				{
					static_assert(sizeof...(T) > 0, "in() requires at least one argument");
					return { *static_cast<const Base*>(this), std::forward<T>(t)... };
				}

			template<typename... T>
				nary_expression_t<Base, not_in_, typename Constraint<T>::type...> not_in(T&&... t) const
				{
					static_assert(sizeof...(T) > 0, "not_in() requires at least one argument");
					return { *static_cast<const Base*>(this), std::forward<T>(t)... };
				}

			template<typename alias_provider>
				expression_alias_t<Base, typename std::decay<alias_provider>::type> as(alias_provider&&)
				{
					return { *static_cast<const Base*>(this) };
				}

			constexpr bool _is_trivial() const { return false; }
		};
	}

}
#endif

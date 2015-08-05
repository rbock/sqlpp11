/*
 * Copyright (c) 2013-2015, Roland Bock
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

#ifndef SQLPP_DETAIL_BASIC_EXPRESSION_OPERATORS_H
#define SQLPP_DETAIL_BASIC_EXPRESSION_OPERATORS_H

#include <sqlpp11/value_type_fwd.h>
#include <sqlpp11/alias.h>
#include <sqlpp11/sort_order.h>
#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/in_fwd.h>
#include <sqlpp11/is_null_fwd.h>
#include <sqlpp11/wrap_operand_fwd.h>
#include <sqlpp11/logic.h>

namespace sqlpp
{
	// basic operators
	template<typename Expr, typename ValueType>
		struct basic_expression_operators
		{
			template<typename T>
				struct _is_valid_comparison_operand
				{
					static constexpr bool value =
						(is_expression_t<T>::value // expressions are OK
						 or is_multi_expression_t<T>::value) // multi-expressions like ANY are OK for comparisons, too
						and ValueType::template _is_valid_operand<T>::value // the correct value type is required, of course
						;
				};

			template<typename T>
				equal_to_t<Expr, wrap_operand_t<T>> operator==(T t) const
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_comparison_operand<rhs>::value, "invalid rhs operand in comparison");

					return { *static_cast<const Expr*>(this), {rhs{t}} };
				}

			template<typename T>
				not_equal_to_t<Expr, wrap_operand_t<T>> operator!=(T t) const
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_comparison_operand<rhs>::value, "invalid rhs operand in comparison");

					return { *static_cast<const Expr*>(this), {rhs{t}} };
				}

			template<typename T>
				less_than_t<Expr, wrap_operand_t<T>> operator<(T t) const
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_comparison_operand<rhs>::value, "invalid rhs operand in comparison");

					return { *static_cast<const Expr*>(this), rhs{t} };
				}

			template<typename T>
				less_equal_t<Expr, wrap_operand_t<T>> operator<=(T t) const
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_comparison_operand<rhs>::value, "invalid rhs operand in comparison");

					return { *static_cast<const Expr*>(this), rhs{t} };
				}

			template<typename T>
				greater_than_t<Expr, wrap_operand_t<T>> operator>(T t) const
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_comparison_operand<rhs>::value, "invalid rhs operand in comparison");

					return { *static_cast<const Expr*>(this), rhs{t} };
				}

			template<typename T>
				greater_equal_t<Expr, wrap_operand_t<T>> operator>=(T t) const
				{
					using rhs = wrap_operand_t<T>;
					static_assert(_is_valid_comparison_operand<rhs>::value, "invalid rhs operand in comparison");

					return { *static_cast<const Expr*>(this), rhs{t} };
				}

			is_null_t<Expr> is_null() const
			{
				return { *static_cast<const Expr*>(this) };
			}

			is_not_null_t<Expr> is_not_null() const
			{
				return { *static_cast<const Expr*>(this) };
			}

			sort_order_t<Expr, sort_type::asc> asc() const
			{
				return { *static_cast<const Expr*>(this) };
			}

			sort_order_t<Expr, sort_type::desc> desc() const
			{
				return { *static_cast<const Expr*>(this) };
			}

			// Hint: use value_list wrapper for containers...
			template<typename... T>
				in_t<Expr, wrap_operand_t<T>...> in(T... t) const
				{
					static_assert(logic::all_t<_is_valid_comparison_operand<wrap_operand_t<T>>::value...>::value, "at least one operand of in() is not valid");
					return { *static_cast<const Expr*>(this), wrap_operand_t<T>{t}... };
				}

			template<typename... T>
				not_in_t<Expr, wrap_operand_t<T>...> not_in(T... t) const
				{
					static_assert(logic::all_t<_is_valid_comparison_operand<wrap_operand_t<T>>::value...>::value, "at least one operand of in() is not valid");
					return { *static_cast<const Expr*>(this), wrap_operand_t<T>{t}... };
				}
		};

	template<typename Expr>
		struct alias_operators
		{
			template<typename alias_provider>
				expression_alias_t<Expr, alias_provider> as(const alias_provider&) const
				{
					return { *static_cast<const Expr*>(this) };
				}
		};

}
#endif

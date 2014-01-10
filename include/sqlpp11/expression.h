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

#ifndef SQLPP_EXPRESSION_H
#define SQLPP_EXPRESSION_H

#include <sqlpp11/alias.h>
#include <sqlpp11/noop.h>
#include <sqlpp11/interpreter.h>
#include <sqlpp11/detail/wrap_operand.h>
#include <sqlpp11/detail/serialize_tuple.h>

namespace sqlpp
{
	template<typename Lhs, typename Rhs>
		struct assignment_t
		{
			using _is_assignment = std::true_type;
			using column_type = Lhs;
			using value_type = Rhs;

			Lhs _lhs;
			Rhs _rhs;
		};

	template<typename Db, typename Lhs, typename Rhs>
		struct interpreter_t<Db, assignment_t<Lhs, Rhs>>
		{
			using T = assignment_t<Lhs, Rhs>;
			template<typename Context>
				static void _(const T& t, Context& context)
				{
					interpret(t._lhs, context);
					if (trivial_value_is_null_t<Lhs>::value and t._rhs._is_trivial())
					{
						context << "=NULL";
					}
					else
					{
						context << "=";
						interpret(t._rhs, context);
					}
				}
		};


	template<typename Lhs, typename Rhs, typename ValueType = detail::boolean>
		struct equal_t: public ValueType::template operators<equal_t<Lhs, Rhs>>
		{
			using _value_type = ValueType; // FIXME: Can we use boolean directly here?

			template<typename L, typename R>
			equal_t(L&& l, R&& r):
				_lhs(std::forward<L>(l)), 
				_rhs(std::forward<R>(r))
			{}

			equal_t(const equal_t&) = default;
			equal_t(equal_t&&) = default;
			equal_t& operator=(const equal_t&) = default;
			equal_t& operator=(equal_t&&) = default;
			~equal_t() = default;

		private:
			Lhs _lhs;
			Rhs _rhs;
		};

	template<typename Db, typename... Args>
		struct interpreter_t<Db, equal_t<Args...>>
		{
			using T = equal_t<Args...>;
			template<typename Context>
				static void interpret(const T& t, Context& context)
				{
					context << "(";
					interpret(t._lhs, context);
					if (trivial_value_is_null_t<typename T::Lhs>::value and t._rhs._is_trivial())
					{
						context << "IS NULL";
					}
					else
					{
						context << "=";
						interpret(t._rhs, context);
					}
					context << ")";
				}
		};

	template<typename Lhs, typename Rhs, typename ValueType = detail::boolean>
		struct not_equal_t: public ValueType::template operators<not_equal_t<Lhs, Rhs>>
		{
			using _value_type = ValueType;

			template<typename L, typename R>
			not_equal_t(L&& l, R&& r):
				_lhs(std::forward<L>(l)), 
				_rhs(std::forward<R>(r))
			{}

			not_equal_t(const not_equal_t&) = default;
			not_equal_t(not_equal_t&&) = default;
			not_equal_t& operator=(const not_equal_t&) = default;
			not_equal_t& operator=(not_equal_t&&) = default;
			~not_equal_t() = default;

			Lhs _lhs;
			Rhs _rhs;
		};

	template<typename Db, typename... Args>
		struct interpreter_t<Db, not_equal_t<Args...>>
		{
			using T = not_equal_t<Args...>;
			template<typename Context>
				static void interpret(const T& t, Context& context)
				{
					context << "(";
					interpret(t._lhs, context);
					if (trivial_value_is_null_t<typename T::Lhs>::value and t._rhs._is_trivial())
					{
						context << "IS NOT NULL";
					}
					else
					{
						context << "!=";
						interpret(t._rhs, context);
					}
					context << ")";
				}
		};

	template<typename Lhs, typename ValueType = detail::boolean>
		struct not_t: public ValueType::template operators<not_t<Lhs>>
		{
			using _value_type = ValueType;

			not_t(Lhs l):
				_lhs(l)
			{}

			not_t(const not_t&) = default;
			not_t(not_t&&) = default;
			not_t& operator=(const not_t&) = default;
			not_t& operator=(not_t&&) = default;
			~not_t() = default;

			Lhs _lhs;
		};

	template<typename Db, typename... Args>
		struct interpreter_t<Db, not_t<Args...>>
		{
			using T = not_t<Args...>;
			template<typename Context>
				static void interpret(const T& t, Context& context)
				{
					context << "(";
					if (trivial_value_is_null_t<typename T::Lhs>::value and t._lhs._is_trivial())
					{
						interpret(t._lhs, context);
						context << "IS NULL";
					}
					else
					{
						context << "NOT ";
						interpret(t._lhs, context);
					}
					context << ")";
				}
		};

	template<typename Lhs, typename O, typename Rhs>
		struct binary_expression_t: public O::_value_type::template operators<binary_expression_t<Lhs, O, Rhs>>
		{
			using _value_type = typename O::_value_type;

			binary_expression_t(Lhs lhs, Rhs rhs):
				_lhs(lhs), 
				_rhs(rhs)
			{}

			binary_expression_t(const binary_expression_t&) = default;
			binary_expression_t(binary_expression_t&&) = default;
			binary_expression_t& operator=(const binary_expression_t&) = default;
			binary_expression_t& operator=(binary_expression_t&&) = default;
			~binary_expression_t() = default;

			Lhs _lhs;
			Rhs _rhs;
		};

	template<typename Db, typename... Args>
		struct interpreter_t<Db, binary_expression_t<Args...>>
		{
			using T = binary_expression_t<Args...>;
			template<typename Context>
				static void interpret(const T& t, Context& context)
				{
					context << "(";
					interpret(t._lhs, context);
					context << T::O::_name;
					interpret(t._rhs, context);
					context << ")";
				}
		};

}

#endif

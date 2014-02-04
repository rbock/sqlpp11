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
#include <sqlpp11/boolean.h>
#include <sqlpp11/tvin.h>
#include <sqlpp11/vendor/noop.h>
#include <sqlpp11/vendor/expression_fwd.h>
#include <sqlpp11/vendor/interpreter.h>
#include <sqlpp11/vendor/wrap_operand.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename Lhs, typename Rhs>
			struct equal_t: public ::sqlpp::detail::boolean::template operators<equal_t<Lhs, Rhs>>
		{
			using _value_type = ::sqlpp::detail::boolean;
			using _parameter_tuple_t = std::tuple<Lhs, Rhs>;

			equal_t(Lhs lhs, Rhs rhs):
				_lhs(lhs), 
				_rhs(rhs)
			{}

			equal_t(const equal_t&) = default;
			equal_t(equal_t&&) = default;
			equal_t& operator=(const equal_t&) = default;
			equal_t& operator=(equal_t&&) = default;
			~equal_t() = default;

			Lhs _lhs;
			tvin_wrap_t<Rhs> _rhs;
		};

		template<typename Context, typename Lhs, typename Rhs>
			struct interpreter_t<Context, equal_t<Lhs, Rhs>>
			{
				using T = equal_t<Lhs, Rhs>;

				static Context& _(const T& t, Context& context)
				{
					context << "(";
					interpret(t._lhs, context);
					if (t._rhs._is_trivial())
					{
						context << " IS NULL";
					}
					else
					{
						context << "=";
						interpret(t._rhs, context);
					}
					context << ")";
					return context;
				}
			};

		template<typename Lhs, typename Rhs>
			struct not_equal_t: public ::sqlpp::detail::boolean::template operators<not_equal_t<Lhs, Rhs>>
		{
			using _value_type = ::sqlpp::detail::boolean;
			using _parameter_tuple_t = std::tuple<Lhs, Rhs>;

			not_equal_t(Lhs lhs, Rhs rhs):
				_lhs(lhs), 
				_rhs(rhs)
			{}

			not_equal_t(const not_equal_t&) = default;
			not_equal_t(not_equal_t&&) = default;
			not_equal_t& operator=(const not_equal_t&) = default;
			not_equal_t& operator=(not_equal_t&&) = default;
			~not_equal_t() = default;

			Lhs _lhs;
			tvin_wrap_t<Rhs> _rhs;
		};

		template<typename Context, typename Lhs, typename Rhs>
			struct interpreter_t<Context, not_equal_t<Lhs, Rhs>>
			{
				using T = not_equal_t<Lhs, Rhs>;

				static Context& _(const T& t, Context& context)
				{
					context << "(";
					interpret(t._lhs, context);
					if (t._rhs._is_trivial())
					{
						context << " IS NOT NULL";
					}
					else
					{
						context << "!=";
						interpret(t._rhs, context);
					}
					context << ")";
					return context;
				}
			};

		template<typename Lhs>
			struct logical_not_t: public ::sqlpp::detail::boolean::template operators<logical_not_t<Lhs>>
		{
			using _value_type = ::sqlpp::detail::boolean;
			using _parameter_tuple_t = std::tuple<Lhs>;

			logical_not_t(Lhs l):
				_lhs(l)
			{}

			logical_not_t(const logical_not_t&) = default;
			logical_not_t(logical_not_t&&) = default;
			logical_not_t& operator=(const logical_not_t&) = default;
			logical_not_t& operator=(logical_not_t&&) = default;
			~logical_not_t() = default;

			Lhs _lhs;
		};

		template<typename Context, typename Lhs>
			struct interpreter_t<Context, logical_not_t<Lhs>>
			{
				using T = logical_not_t<Lhs>;

				static Context& _(const T& t, Context& context)
				{
					context << "(";
					context << "NOT ";
					interpret(t._lhs, context);
					context << ")";
					return context;
				}
			};

		template<typename Lhs, typename O, typename Rhs>
			struct binary_expression_t: public O::_value_type::template operators<binary_expression_t<Lhs, O, Rhs>>
		{
			using _lhs_t = Lhs;
			using _rhs_t = Rhs;
			using _value_type = typename O::_value_type;
			using _parameter_tuple_t = std::tuple<_lhs_t, _rhs_t>;

			binary_expression_t(_lhs_t lhs, _rhs_t rhs):
				_lhs(lhs), 
				_rhs(rhs)
			{}

			binary_expression_t(const binary_expression_t&) = default;
			binary_expression_t(binary_expression_t&&) = default;
			binary_expression_t& operator=(const binary_expression_t&) = default;
			binary_expression_t& operator=(binary_expression_t&&) = default;
			~binary_expression_t() = default;

			_lhs_t _lhs;
			_rhs_t _rhs;
		};

		template<typename Context, typename Lhs, typename O, typename Rhs>
			struct interpreter_t<Context, binary_expression_t<Lhs, O, Rhs>>
			{
				using T = binary_expression_t<Lhs, O, Rhs>;

				static Context& _(const T& t, Context& context)
				{
					context << "(";
					interpret(t._lhs, context);
					context << O::_name;
					interpret(t._rhs, context);
					context << ")";
					return context;
				}
			};

		template<typename O, typename Rhs>
			struct unary_expression_t: public O::_value_type::template operators<unary_expression_t<O, Rhs>>
		{
			using _value_type = typename O::_value_type;
			using _parameter_tuple_t = std::tuple<Rhs>;

			unary_expression_t(Rhs rhs):
				_rhs(rhs)
			{}

			unary_expression_t(const unary_expression_t&) = default;
			unary_expression_t(unary_expression_t&&) = default;
			unary_expression_t& operator=(const unary_expression_t&) = default;
			unary_expression_t& operator=(unary_expression_t&&) = default;
			~unary_expression_t() = default;

			Rhs _rhs;
		};

		template<typename Context, typename O, typename Rhs>
			struct interpreter_t<Context, unary_expression_t<O, Rhs>>
			{
				using T = unary_expression_t<O, Rhs>;

				static Context& _(const T& t, Context& context)
				{
					context << "(";
					context << O::_name;
					interpret(t._rhs, context);
					context << ")";
					return context;
				}
			};
	}
}

#endif

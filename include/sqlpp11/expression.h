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

#ifndef SQLPP_EXPRESSION_H
#define SQLPP_EXPRESSION_H

#include <sqlpp11/alias.h>
#include <sqlpp11/boolean.h>
#include <sqlpp11/tvin.h>
#include <sqlpp11/rhs_is_null.h>
#include <sqlpp11/rhs_is_trivial.h>
#include <sqlpp11/noop.h>
#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/wrap_operand.h>

namespace sqlpp
{
	template<typename Lhs, typename Rhs>
		struct binary_expression_t<Lhs, op::equal_to, Rhs>: public ::sqlpp::detail::boolean::template expression_operators<binary_expression_t<Lhs, op::equal_to, Rhs>>,
		public alias_operators<binary_expression_t<Lhs, op::equal_to, Rhs>>
		{
			using _traits = make_traits<boolean, sqlpp::tag::is_expression>;
			using _recursive_traits = make_recursive_traits<Lhs, Rhs>;
			using _lhs_t = Lhs;
			using _rhs_t = allow_tvin_t<Rhs>;

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

	template<typename Context, typename Lhs, typename Rhs>
		struct serializer_t<Context, equal_to_t<Lhs, Rhs>>
		{
			using T = equal_to_t<Lhs, Rhs>;

			static Context& _(const T& t, Context& context)
			{
				context << "(";
				serialize(t._lhs, context);
				if ((trivial_value_is_null_t<typename T::_lhs_t>::value and rhs_is_trivial(t))
						or rhs_is_null(t))
				{
					context << " IS NULL";
				}
				else
				{
					context << "=";
					serialize(t._rhs, context);
				}
				context << ")";
				return context;
			}
		};

	template<typename Lhs, typename Rhs>
		struct binary_expression_t<Lhs, op::not_equal_to, Rhs>: public ::sqlpp::detail::boolean::template expression_operators<binary_expression_t<Lhs, op::not_equal_to, Rhs>>,
		public alias_operators<binary_expression_t<Lhs, op::not_equal_to, Rhs>>
		{
			using _traits = make_traits<boolean, sqlpp::tag::is_expression>;
			using _recursive_traits = make_recursive_traits<Lhs, Rhs>;
			using _lhs_t = Lhs;
			using _rhs_t = allow_tvin_t<Rhs>;

			binary_expression_t(Lhs lhs, _rhs_t rhs):
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

	template<typename Context, typename Lhs, typename Rhs>
		struct serializer_t<Context, not_equal_to_t<Lhs, Rhs>>
		{
			using T = not_equal_to_t<Lhs, Rhs>;

			static Context& _(const T& t, Context& context)
			{
				context << "(";
				serialize(t._lhs, context);
				if ((trivial_value_is_null_t<typename T::_lhs_t>::value and rhs_is_trivial(t))
						or rhs_is_null(t))
				{
					context << " IS NOT NULL";
				}
				else
				{
					context << "!=";
					serialize(t._rhs, context);
				}
				context << ")";
				return context;
			}
		};

	template<typename Rhs>
		struct unary_expression_t<op::logical_not, Rhs>: public ::sqlpp::detail::boolean::template expression_operators<unary_expression_t<op::logical_not, Rhs>>,
		public alias_operators<unary_expression_t<op::logical_not, Rhs>>
		{
			using _traits = make_traits<boolean, sqlpp::tag::is_expression>;
			using _recursive_traits = make_recursive_traits<Rhs>;

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

	template<typename Context, typename Rhs>
		struct serializer_t<Context, logical_not_t<Rhs>>
		{
			using T = logical_not_t<Rhs>;

			static Context& _(const T& t, Context& context)
			{
				context << "(";
				if (trivial_value_is_null_t<Rhs>::value)
				{
					serialize(t._lhs, context);
					context << " IS NULL ";
				}
				else
				{
					context << "NOT ";
					serialize(t._rhs, context);
				}
				context << ")";
			}
		};

	template<typename Lhs, typename O, typename Rhs>
		struct binary_expression_t: public value_type_of<O>::template expression_operators<binary_expression_t<Lhs, O, Rhs>>,
		public alias_operators<binary_expression_t<Lhs, O, Rhs>>
	{
		using _traits = make_traits<value_type_of<O>, sqlpp::tag::is_expression>;
		using _recursive_traits = make_recursive_traits<Lhs, Rhs>;

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

	template<typename Context, typename Lhs, typename O, typename Rhs>
		struct serializer_t<Context, binary_expression_t<Lhs, O, Rhs>>
		{
			using T = binary_expression_t<Lhs, O, Rhs>;

			static Context& _(const T& t, Context& context)
			{
				context << "(";
				serialize(t._lhs, context);
				context << O::_name;
				serialize(t._rhs, context);
				context << ")";
				return context;
			}
		};

	template<typename O, typename Rhs>
		struct unary_expression_t: public value_type_of<O>::template expression_operators<unary_expression_t<O, Rhs>>,
		public alias_operators<unary_expression_t<O, Rhs>>
	{
		using _traits = make_traits<value_type_of<O>, sqlpp::tag::is_expression>;
		using _recursive_traits = make_recursive_traits<Rhs>;

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
		struct serializer_t<Context, unary_expression_t<O, Rhs>>
		{
			using T = unary_expression_t<O, Rhs>;

			static Context& _(const T& t, Context& context)
			{
				context << "(";
				context << O::_name;
				serialize(t._rhs, context);
				context << ")";
				return context;
			}
		};
}

#endif

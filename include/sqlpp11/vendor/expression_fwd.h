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

#ifndef SQLPP_EXPRESSION_FWD_H
#define SQLPP_EXPRESSION_FWD_H

namespace sqlpp
{
	namespace detail
	{
		struct boolean;
		struct integral;
		struct floating_point;
	}

	namespace vendor
	{
		namespace op
		{
			struct less
			{
				using _traits = make_traits<::sqlpp::detail::boolean>;
				static constexpr const char* _name = "<";
			};

			struct less_equal
			{
				using _traits = make_traits<::sqlpp::detail::boolean>;
				static constexpr const char* _name = "<=";
			};

			struct equal_to
			{
				using _traits = make_traits<::sqlpp::detail::boolean>;
			};

			struct not_equal_to
			{
				using _traits = make_traits<::sqlpp::detail::boolean>;
			};

			struct greater_equal
			{
				using _traits = make_traits<::sqlpp::detail::boolean>;
				static constexpr const char* _name = ">=";
			};

			struct greater
			{
				using _traits = make_traits<::sqlpp::detail::boolean>;
				static constexpr const char* _name = ">";
			};

			struct logical_or
			{
				using _traits = make_traits<::sqlpp::detail::boolean>;
				static constexpr const char* _name = " OR ";
			};

			struct logical_and
			{
				using _traits = make_traits<::sqlpp::detail::boolean>;
				static constexpr const char* _name = " AND ";
			};

			struct logical_not
			{
				using _traits = make_traits<::sqlpp::detail::boolean>;
			};

			template<typename ValueType>
			struct plus
			{
				using _traits = make_traits<ValueType>;
				static constexpr const char* _name = "+";
			};

			template<typename ValueType>
			struct minus
			{
				using _traits = make_traits<ValueType>;
				static constexpr const char* _name = "-";
			};

			template<typename ValueType>
			struct multiplies
			{
				using _traits = make_traits<ValueType>;
				static constexpr const char* _name = "*";
			};

			struct divides
			{
				using _traits = make_traits<::sqlpp::detail::floating_point>;
				static constexpr const char* _name = "/";
			};

			struct modulus
			{
				using _traits = make_traits<::sqlpp::detail::integral>;
				static constexpr const char* _name = "%";
			};

			template<typename ValueType>
			struct unary_minus
			{
				using _traits = make_traits<ValueType>;
				static constexpr const char* _name = "-";
			};

			template<typename ValueType>
			struct unary_plus
			{
				using _traits = make_traits<ValueType>;
				static constexpr const char* _name = "+";
			};
		}

		template<typename Lhs, typename O, typename Rhs>
			struct binary_expression_t;

		template<typename O, typename Rhs>
			struct unary_expression_t;

		template<typename Lhs, typename Rhs>
			using less_than_t = binary_expression_t<Lhs, op::less, Rhs>;

		template<typename Lhs, typename Rhs>
			using less_equal_t = binary_expression_t<Lhs, op::less_equal, Rhs>;

		template<typename Lhs, typename Rhs>
			using equal_to_t = binary_expression_t<Lhs, op::equal_to, Rhs>;

		template<typename Lhs, typename Rhs>
			using not_equal_to_t = binary_expression_t<Lhs, op::not_equal_to, Rhs>;

		template<typename Lhs, typename Rhs>
			using greater_than_t = binary_expression_t<Lhs, op::greater, Rhs>;

		template<typename Lhs, typename Rhs>
			using greater_equal_t = binary_expression_t<Lhs, op::greater_equal, Rhs>;

		template<typename Lhs, typename Rhs>
			using logical_and_t = binary_expression_t<Lhs, op::logical_and, Rhs>;

		template<typename Lhs, typename Rhs>
			using logical_or_t = binary_expression_t<Lhs, op::logical_or, Rhs>;

		template<typename Lhs, typename ValueType, typename Rhs>
			using plus_t = binary_expression_t<Lhs, op::plus<ValueType>, Rhs>;

		template<typename Lhs, typename ValueType, typename Rhs>
			using minus_t = binary_expression_t<Lhs, op::minus<ValueType>, Rhs>;

		template<typename Lhs, typename ValueType, typename Rhs>
			using multiplies_t = binary_expression_t<Lhs, op::multiplies<ValueType>, Rhs>;

		template<typename Lhs, typename Rhs>
			using divides_t = binary_expression_t<Lhs, op::divides, Rhs>;

		template<typename Lhs, typename Rhs>
			using modulus_t = binary_expression_t<Lhs, op::modulus, Rhs>;

		template<typename Rhs>
			using logical_not_t = unary_expression_t<op::logical_not, Rhs>;

		template<typename ValueType, typename Rhs>
			using unary_plus_t = unary_expression_t<op::unary_plus<ValueType>, Rhs>;

		template<typename ValueType, typename Rhs>
			using unary_minus_t = unary_expression_t<op::unary_minus<ValueType>, Rhs>;

	}
}

#endif

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

#ifndef SQLPP_DETAIL_WRAP_OPERAND_H
#define SQLPP_DETAIL_WRAP_OPERAND_H

#include <string>
#include <sqlpp11/vendor/interpreter.h>

namespace sqlpp
{
	namespace detail
	{
		struct boolean;
		struct integral;
		struct floating_point;
		struct text;
	}

	namespace vendor
	{
		struct boolean_operand
		{
			static constexpr bool _is_expression = true;
			using _value_type = sqlpp::detail::boolean;

			bool _is_trivial() const { return _t == false; }

			bool _t;
		};

		template<typename Context>
			struct interpreter_t<Context, boolean_operand>
			{
				using Operand = boolean_operand;

				static Context& _(const Operand& t, Context& context)
				{
					context << t._t;
					return context;
				}
			};

		template<typename T>
			struct integral_operand
			{
				static constexpr bool _is_expression = true;
				using _value_type = ::sqlpp::detail::integral;

				bool _is_trivial() const { return _t == 0; }

				T _t;
			};

		template<typename Context, typename T>
			struct interpreter_t<Context, integral_operand<T>>
			{
				using Operand = integral_operand<T>;

				static Context& _(const Operand& t, Context& context)
				{
					context << t._t;
					return context;
				}
			};


		template<typename T>
			struct floating_point_operand
			{
				static constexpr bool _is_expression = true;
				using _value_type = ::sqlpp::detail::floating_point;

				bool _is_trivial() const { return _t == 0; }

				T _t;
			};

		template<typename Context, typename T>
			struct interpreter_t<Context, floating_point_operand<T>>
			{
				using Operand = floating_point_operand<T>;

				static Context& _(const Operand& t, Context& context)
				{
					context << t._t;
					return context;
				}
			};

		struct text_operand
		{
			static constexpr bool _is_expression = true;
			using _value_type = ::sqlpp::detail::text;

			bool _is_trivial() const { return _t.empty(); }

			std::string _t;
		};

		template<typename Context>
			struct interpreter_t<Context, text_operand>
			{
				using Operand = text_operand;

				static Context& _(const Operand& t, Context& context)
				{
					context << '\'' << context.escape(t._t) << '\'';
					return context;
				}
			};

		template<typename T, typename Enable = void>
			struct wrap_operand
			{
				using type = T;
			};

		template<>
			struct wrap_operand<bool, void>
			{
				using type = boolean_operand;
			};

		template<typename T>
			struct wrap_operand<T, typename std::enable_if<std::is_integral<T>::value>::type>
			{
				using type = integral_operand<T>;
			};

		template<typename T>
			struct wrap_operand<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
			{
				using type = floating_point_operand<T>;
			};

		template<typename T>
			struct wrap_operand<T, typename std::enable_if<std::is_convertible<T, std::string>::value>::type>
			{
				using type = text_operand;
			};

		// FIXME: Need to allow std::ref arguments
	}
}

#endif

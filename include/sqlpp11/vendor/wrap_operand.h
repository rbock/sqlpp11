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

#ifndef SQLPP_DETAIL_WRAP_OPERAND_H
#define SQLPP_DETAIL_WRAP_OPERAND_H

#include <string>
#include <sqlpp11/vendor/serializer.h>
#include <sqlpp11/type_traits.h>

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
			using _traits = make_traits<::sqlpp::detail::boolean, ::sqlpp::tag::expression>;
			using _recursive_traits = make_recursive_traits<>;

			using _value_t = bool;

			boolean_operand():
				_t{}
			{}

			boolean_operand(_value_t t):
				_t(t)
			{}

			boolean_operand(const boolean_operand&) = default;
			boolean_operand(boolean_operand&&) = default;
			boolean_operand& operator=(const boolean_operand&) = default;
			boolean_operand& operator=(boolean_operand&&) = default;
			~boolean_operand() = default;

			bool _is_trivial() const { return _t == false; }

			_value_t _t;
		};

		template<typename Context>
			struct serializer_t<Context, boolean_operand>
			{
				using Operand = boolean_operand;

				static Context& _(const Operand& t, Context& context)
				{
					context << t._t;
					return context;
				}
			};

		struct integral_operand
		{
			using _traits = make_traits<::sqlpp::detail::integral, ::sqlpp::tag::expression>;
			using _recursive_traits = make_recursive_traits<>;

			using _value_t = int64_t;

			integral_operand():
				_t{}
			{}

			integral_operand(_value_t t):
				_t(t)
			{}

			integral_operand(const integral_operand&) = default;
			integral_operand(integral_operand&&) = default;
			integral_operand& operator=(const integral_operand&) = default;
			integral_operand& operator=(integral_operand&&) = default;
			~integral_operand() = default;

			bool _is_trivial() const { return _t == 0; }

			_value_t _t;
		};

		template<typename Context>
			struct serializer_t<Context, integral_operand>
			{
				using Operand = integral_operand;

				static Context& _(const Operand& t, Context& context)
				{
					context << t._t;
					return context;
				}
			};


		struct floating_point_operand
		{
			using _traits = make_traits<::sqlpp::detail::floating_point, ::sqlpp::tag::expression>;
			using _recursive_traits = make_recursive_traits<>;

			using _value_t = double;

			floating_point_operand():
				_t{}
			{}

			floating_point_operand(_value_t t):
				_t(t)
			{}

			floating_point_operand(const floating_point_operand&) = default;
			floating_point_operand(floating_point_operand&&) = default;
			floating_point_operand& operator=(const floating_point_operand&) = default;
			floating_point_operand& operator=(floating_point_operand&&) = default;
			~floating_point_operand() = default;

			bool _is_trivial() const { return _t == 0; }

			_value_t _t;
		};

		template<typename Context>
			struct serializer_t<Context, floating_point_operand>
			{
				using Operand = floating_point_operand;

				static Context& _(const Operand& t, Context& context)
				{
					context << t._t;
					return context;
				}
			};

		struct text_operand
		{
			using _traits = make_traits<::sqlpp::detail::text, ::sqlpp::tag::expression>;
			using _recursive_traits = make_recursive_traits<>;

			using _value_t = std::string;

			text_operand():
				_t{}
			{}

			text_operand(_value_t t):
				_t(t)
			{}

			text_operand(const text_operand&) = default;
			text_operand(text_operand&&) = default;
			text_operand& operator=(const text_operand&) = default;
			text_operand& operator=(text_operand&&) = default;
			~text_operand() = default;

			bool _is_trivial() const { return _t.empty(); }

			_value_t _t;
		};

		template<typename Context>
			struct serializer_t<Context, text_operand>
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
				using type = integral_operand;
			};

		template<typename T>
			struct wrap_operand<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
			{
				using type = floating_point_operand;
			};

		template<typename T>
			struct wrap_operand<T, typename std::enable_if<std::is_convertible<T, std::string>::value>::type>
			{
				using type = text_operand;
			};

		// FIXME: Need to allow std::ref arguments

		template<typename T>
			using wrap_operand_t = typename wrap_operand<T>::type;

	}
}

#endif

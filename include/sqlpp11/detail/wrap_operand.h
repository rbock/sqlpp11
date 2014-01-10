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

#include <ostream>
#include <sqlpp11/interpreter.h>

// FIXME: must leave detail, since it is interpreted (and might require specializations)

namespace sqlpp
{
	namespace detail
	{
		struct boolean;
		struct integral;
		struct floating_point;
		struct text;
	}

		struct bool_operand
		{
			static constexpr bool _is_expression = true;
			using _value_type = detail::boolean;

			bool_operand(bool t): _t(t) {}
			bool_operand(const bool_operand&) = default;
			bool_operand(bool_operand&&) = default;
			bool_operand& operator=(const bool_operand&) = default;
			bool_operand& operator=(bool_operand&&) = default;
			~bool_operand() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << _t;
				}

			bool _is_trivial() const { return _t == false; }

			bool _t;
		};

		template<typename T>
			struct integral_operand
			{
				static constexpr bool _is_expression = true;
				using _value_type = detail::integral;

				integral_operand(T t): _t(t) {}
				integral_operand(const integral_operand&) = default;
				integral_operand(integral_operand&&) = default;
				integral_operand& operator=(const integral_operand&) = default;
				integral_operand& operator=(integral_operand&&) = default;
				~integral_operand() = default;

				template<typename Db>
					void serialize(std::ostream& os, Db& db) const
					{
						os << _t;
					}

				bool _is_trivial() const { return _t == 0; }

				T _t;
			};

	template<typename Db, typename T>
		struct interpreter_t<Db, integral_operand<T>>
		{
			using Operand = integral_operand<T>;
			template<typename Context>
				static void _(const Operand& t, Context& context)
				{
					context << t._t;
				}
		};


		template<typename T>
			struct floating_point_operand
			{
				static constexpr bool _is_expression = true;
				using _value_type = detail::floating_point;

				floating_point_operand(T t): _t(t) {}
				floating_point_operand(const floating_point_operand&) = default;
				floating_point_operand(floating_point_operand&&) = default;
				floating_point_operand& operator=(const floating_point_operand&) = default;
				floating_point_operand& operator=(floating_point_operand&&) = default;
				~floating_point_operand() = default;

				template<typename Db>
					void serialize(std::ostream& os, Db& db) const
					{
						os << _t;
					}

				bool _is_trivial() const { return _t == 0; }

				T _t;
			};

		template<typename T>
			struct text_operand
			{
				static constexpr bool _is_expression = true;
				using _value_type = detail::text;

				text_operand(const T& t): _t(t) {}
				text_operand(const text_operand&) = default;
				text_operand(text_operand&&) = default;
				text_operand& operator=(const text_operand&) = default;
				text_operand& operator=(text_operand&&) = default;
				~text_operand() = default;

				template<typename Db>
					void serialize(std::ostream& os, Db& db) const
					{
						os << '\'' << db.escape(_t) << '\'';
					}

				bool _is_trivial() const { return _t.empty(); }

				std::string _t;
			};

		template<typename T, typename Enable = void>
			struct wrap_operand
			{
				using type = T;
			};

		template<>
			struct wrap_operand<bool, void>
			{
				using type = bool_operand;
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
				using type = text_operand<T>;
			};
}

#endif

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

#ifndef SQLPP_TVIN_H
#define SQLPP_TVIN_H

// TVIN: Trivial value is NULL

#include <sqlpp11/type_traits.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/wrap_operand.h>

namespace sqlpp
{
	template<typename Operand>
		struct tvin_t
		{
			using _traits = make_traits<value_type_of<Operand>, tag::expression>;
			using _recursive_traits = make_recursive_traits<Operand>;

			using _operand_t = Operand;

			tvin_t(Operand operand): 
				_value(operand)
			{}
			tvin_t(const tvin_t&) = default;
			tvin_t(tvin_t&&) = default;
			tvin_t& operator=(const tvin_t&) = default;
			tvin_t& operator=(tvin_t&&) = default;
			~tvin_t() = default;

			bool _is_trivial() const
			{
				return _value._is_trivial();
			}

			Operand _value;
		};

	template<typename Context, typename Operand>
		struct serializer_t<Context, tvin_t<Operand>>
		{
			using T = tvin_t<Operand>;

			static void _(const T& t, Context& context)
			{
				static_assert(wrong_t<T>::value, "tvin() must not be used with anything but =, ==, != and !");
			}
		};

	template<typename Operand>
		struct maybe_tvin_t
		{
			using _traits = make_traits<value_type_of<Operand>, tag::expression>;
			using _recursive_traits = make_recursive_traits<Operand>;

			static constexpr bool _is_trivial()
			{
				return false;
			}

			maybe_tvin_t(Operand operand): 
				_value(operand)
			{}
			maybe_tvin_t(const maybe_tvin_t&) = default;
			maybe_tvin_t(maybe_tvin_t&&) = default;
			maybe_tvin_t& operator=(const maybe_tvin_t&) = default;
			maybe_tvin_t& operator=(maybe_tvin_t&&) = default;
			~maybe_tvin_t() = default;

			Operand _value;
		};

	template<typename Operand>
		struct maybe_tvin_t<tvin_t<Operand>>
		{
			using _traits = make_traits<value_type_of<Operand>, tag::expression>;
			using _recursive_traits = make_recursive_traits<Operand>;

			bool _is_trivial() const
			{
				return _value._is_trivial();
			};

			maybe_tvin_t(tvin_t<Operand> operand): 
				_value(operand._value)
			{}
			maybe_tvin_t(const maybe_tvin_t&) = default;
			maybe_tvin_t(maybe_tvin_t&&) = default;
			maybe_tvin_t& operator=(const maybe_tvin_t&) = default;
			maybe_tvin_t& operator=(maybe_tvin_t&&) = default;
			~maybe_tvin_t() = default;

			typename tvin_t<Operand>::_operand_t _value;
		};

	template<typename Context, typename Operand>
		struct serializer_t<Context, maybe_tvin_t<Operand>>
		{
			using T = maybe_tvin_t<Operand>;

			static Context& _(const T& t, Context& context)
			{
				if (t._is_trivial())
				{
					context << "NULL";
				}
				else
				{
					serialize(t._value, context);
				}
				return context;
			}
		};

	template<typename Operand>
		auto tvin(Operand operand) -> tvin_t<typename wrap_operand<Operand>::type>
		{
			using _operand_t = typename wrap_operand<Operand>::type;
			static_assert(std::is_same<_operand_t, text_operand>::value
					or not std::is_same<_operand_t, Operand>::value, "tvin() used with invalid type (only string and primitive types allowed)");
			return {{operand}};
		}

}

#endif

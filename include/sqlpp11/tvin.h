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

#ifndef SQLPP_TVIN_H
#define SQLPP_TVIN_H

// TVIN: Trivial value is NULL

#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/interpreter.h>

namespace sqlpp
{
	template<typename Operand>
		struct tvin_t
		{
			using _operand_t = Operand;
			using _value_type = typename _operand_t::_value_type;

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

			_operand_t _value;
		};

	namespace vendor
	{
		template<typename Context, typename Operand>
			struct interpreter_t<Context, tvin_t<Operand>>
			{
				using T = tvin_t<Operand>;

				static void _(const T& t, Context& context)
				{
					static_assert(vendor::wrong_t<T>::value, "tvin() must not be used with anything but =, ==, != and !");
				}
			};
	}

	template<typename T>
		struct tvin_wrap_t
		{
			static constexpr bool _is_trivial()
			{
				return false;
			}

			tvin_wrap_t(T t): 
				_value(t)
			{}
			tvin_wrap_t(const tvin_wrap_t&) = default;
			tvin_wrap_t(tvin_wrap_t&&) = default;
			tvin_wrap_t& operator=(const tvin_wrap_t&) = default;
			tvin_wrap_t& operator=(tvin_wrap_t&&) = default;
			~tvin_wrap_t() = default;

			T _value;
		};

	template<typename T>
		struct tvin_wrap_t<tvin_t<T>>
		{
			bool _is_trivial() const
			{
				return _value._is_trivial();
			};

			tvin_wrap_t(tvin_t<T> t): 
				_value(t._value)
			{}
			tvin_wrap_t(const tvin_wrap_t&) = default;
			tvin_wrap_t(tvin_wrap_t&&) = default;
			tvin_wrap_t& operator=(const tvin_wrap_t&) = default;
			tvin_wrap_t& operator=(tvin_wrap_t&&) = default;
			~tvin_wrap_t() = default;

			typename tvin_t<T>::_operand_t _value;
		};

	namespace vendor
	{
		template<typename Context, typename Operand>
			struct interpreter_t<Context, tvin_wrap_t<Operand>>
			{
				using T = tvin_wrap_t<Operand>;

				static Context& _(const T& t, Context& context)
				{
					if (t._is_trivial())
					{
						context << "NULL";
					}
					else
					{
						interpret(t._value, context);
					}
					return context;
				}
			};
	}

	template<typename T>
		auto tvin(T t) -> tvin_t<typename vendor::wrap_operand<T>::type>
		{
			using _operand_t = typename vendor::wrap_operand<T>::type;
			static_assert(std::is_same<_operand_t, vendor::text_operand>::value
				 	or not std::is_same<_operand_t, T>::value, "tvin() used with invalid type (only string and primitive types allowed)");
			return {{t}};
		}

}

#endif

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

#ifndef SQLPP_RHS_IS_NULL_H
#define SQLPP_RHS_IS_NULL_H

#include <sqlpp11/tvin.h>

namespace sqlpp
{
	template<typename T, typename Enable = void>
		struct is_trivial_t
		{
			static constexpr bool _(const T&)
			{
				return false;
			}
		};

	template<typename T>
		struct is_trivial_t<T, typename std::enable_if<std::is_member_function_pointer<decltype(&T::_is_trivial)>::value, void>::type>
		{
			static bool _(const T& t)
			{
				return t._is_trivial();
			}
		};

	template<typename T>
		bool is_trivial(const T& t)
		{
			return is_trivial_t<T>::_(t);
		}

	template<typename Expression>
		constexpr bool rhs_is_null(const Expression& e)
		{
			return (((trivial_value_is_null_t<typename Expression::_column_t>::value or is_tvin_t<typename Expression::_value_t>::value)
							and is_trivial(e._rhs))
						or (std::is_same<typename Expression::_value_t, null_t>::value));
		}
}

#endif

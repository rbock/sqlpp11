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

#ifndef SQLPP_VALUE_TYPE_FWD_H
#define SQLPP_VALUE_TYPE_FWD_H

#include <sqlpp11/wrong.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
	template<typename ValueType>
		struct parameter_value_t
		{
			static_assert(wrong_t<parameter_value_t>::value, "Missing parameter value type for ValueType");
		};

	template<typename Column, typename ValueType>
		struct column_operators
		{
			static_assert(wrong_t<column_operators>::value, "Missing column operators for ValueType");
		};

	template<typename Expr, typename ValueType>
		struct expression_operators
		{
			static_assert(wrong_t<expression_operators>::value, "Missing expression operators for ValueType");
		};

	template<typename ValueType, typename T>
		struct is_valid_operand
		{
			static constexpr bool value = 
				is_expression_t<T>::value // expressions are OK
				and ValueType::template _is_valid_operand<T>::value // the correct value type is required, of course
				;
		};
}

#endif



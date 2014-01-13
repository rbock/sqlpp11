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

#ifndef SQLPP_PARAMETER_H
#define SQLPP_PARAMETER_H

#include <ostream>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
	template<typename ValueType, typename NameType, typename TrivialValueIsNull>
	struct parameter_t: public ValueType::template operators<parameter_t<ValueType, NameType, TrivialValueIsNull>>
	{
		using _value_type = ValueType;
		using _is_parameter = std::true_type;
		using _is_expression_t = std::true_type;
		using _instance_t = typename NameType::_name_t::template _member_t<typename ValueType::_parameter_t>;
		using _trivial_value_is_null_t = TrivialValueIsNull;

		static_assert(std::is_same<_trivial_value_is_null_t, std::true_type>::value or std::is_same<_trivial_value_is_null_t, std::false_type>::value, "Invalid template parameter TrivialValueIsNull");

		parameter_t()
		{}

		parameter_t(const parameter_t&) = default;
		parameter_t(parameter_t&&) = default;
		parameter_t& operator=(const parameter_t&) = default;
		parameter_t& operator=(parameter_t&&) = default;
		~parameter_t() = default;

		constexpr bool _is_trivial() const
		{
			return false;
		}
	};

	template<typename Context, typename ValueType, typename NameType, typename TrivialValueIsNull>
		struct interpreter_t<Context, parameter_t<ValueType, NameType, TrivialValueIsNull>>
		{
			using T = parameter_t<ValueType, NameType, TrivialValueIsNull>;

			static Context& _(const T& t, Context& context)
			{
				context << "?";
				return context;
			}
		};

	template<typename NamedExpr, typename TrivialValueIsNull = trivial_value_is_null_t<typename std::decay<NamedExpr>::type>>
		auto parameter(NamedExpr&& namedExpr)
		-> parameter_t<typename std::decay<NamedExpr>::type::_value_type, typename std::decay<NamedExpr>::type, TrivialValueIsNull>
		{
			return {};
		}

	template<typename NamedExpr>
		auto where_parameter(NamedExpr&& namedExpr)
		-> parameter_t<typename std::decay<NamedExpr>::type::_value_type, typename std::decay<NamedExpr>::type, std::false_type>
		{
			return {};
		}

}

#endif

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
	template<typename ValueType, typename NameType, bool TrivialValueIsNull>
	struct parameter_t
	{
		using _value_type = ValueType;
		using _parameter_type = typename ValueType::template _parameter_t<TrivialValueIsNull>;
		using _is_parameter = std::true_type;
		using _is_expression_t = std::true_type;

		template<typename Db>
			void serialize(std::ostream& os, Db& db) const
			{
				static_assert(Db::_supports_parameter, "parameter not supported by current database");
				os << " ? "; // FIXME: Need to support positional placeholders and also type indicators for postgres for instance
			}

		using _member_t = typename NameType::_name_t::template _member_t<_parameter_type>;
	};

	template<typename NamedExpr, bool TrivialValueIsNull = trivial_value_is_null_t<typename std::decay<NamedExpr>::type>::value>
		auto parameter(NamedExpr&& namedExpr)
		-> parameter_t<typename std::decay<NamedExpr>::type::_value_type, typename std::decay<NamedExpr>::type, TrivialValueIsNull>
		{
			return {};
		}

}

#endif

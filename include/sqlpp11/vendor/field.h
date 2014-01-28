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

#ifndef SQLPP_FIELD_H
#define SQLPP_FIELD_H

#include <sqlpp11/multi_column.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename NameType, typename ValueType>
			struct field_t
			{ 
				using _name_t = NameType;
				using _value_type = ValueType;
			};

		template<typename AliasProvider, typename FieldTuple>
			struct multi_field_t
			{
			};

		namespace detail
		{
			template<typename NamedExpr>
				struct make_field_t_impl
				{
					using type = field_t<typename NamedExpr::_name_t, typename NamedExpr::_value_type::_base_value_type>;
				};

			template<typename AliasProvider, typename... NamedExpr>
				struct make_field_t_impl<multi_column_t<AliasProvider, std::tuple<NamedExpr...>>>
				{
					using type = multi_field_t<AliasProvider, std::tuple<typename make_field_t_impl<NamedExpr>::type...>>;
				};
		}

		template<typename NamedExpr>
			using make_field_t = typename detail::make_field_t_impl<NamedExpr>::type;
	}

}

#endif

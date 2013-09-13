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

#ifndef SQLPP_RESULT_ROW_H
#define SQLPP_RESULT_ROW_H

#include <sqlpp11/raw_result_row.h>
#include <sqlpp11/field.h>
#include <iostream>
namespace sqlpp
{
	namespace detail
	{
		template<size_t level, size_t index, typename... NamedExpr>
			struct result_row_impl;

		template<size_t level, size_t index, typename NamedExpr, typename... Rest>
			struct result_row_impl<level, index, NamedExpr, Rest...>: 
			public NamedExpr::_name_t::template _member_t<typename NamedExpr::_value_type::template _result_entry_t<index>>,
			public result_row_impl<level, index + 1, Rest...>
			{
				using _field = typename NamedExpr::_name_t::template _member_t<typename NamedExpr::_value_type::template _result_entry_t<index>>;
				using _rest = result_row_impl<level, index + 1, Rest...>;

				result_row_impl(const raw_result_row_t& raw_result_row):
					_field({raw_result_row}),
					_rest(raw_result_row)
				{}

				result_row_impl& operator=(const raw_result_row_t& raw_result_row)
				{
					_field::operator=({raw_result_row});
					_rest::operator=(raw_result_row);
					return *this;
				}
			};

		template<size_t level, size_t index, typename AliasProvider, typename... Col, typename... Rest>
			struct result_row_impl<level, index, multi_field_t<AliasProvider, std::tuple<Col...>>, Rest...>: 
			public AliasProvider::_name_t::template _member_t<result_row_impl<level + 1, index, Col...>>, // level prevents identical closures to be present twice in the inheritance tree
			public result_row_impl<level, index + sizeof...(Col), Rest...>
			{
				using _multi_field = typename AliasProvider::_name_t::template _member_t<result_row_impl<level + 1, index, Col...>>;
				using _rest = result_row_impl<level, index + sizeof...(Col), Rest...>;

				result_row_impl(const raw_result_row_t& raw_result_row):
					_multi_field({raw_result_row}),
					_rest(raw_result_row)
				{}

				result_row_impl& operator=(const raw_result_row_t& raw_result_row)
				{
					_multi_field::operator=({raw_result_row});
					_rest::operator=(raw_result_row);
					return *this;
				}
			};

		template<size_t level, size_t index>
			struct result_row_impl<level, index>
			{
				result_row_impl(const raw_result_row_t& raw_result_row)
				{}

				result_row_impl& operator=(const raw_result_row_t& raw_result_row)
				{
					return *this;
				}
			};
	}

	template<typename... NamedExpr>
	struct result_row_t: public detail::result_row_impl<0, 0, NamedExpr...>
	{
		bool _is_row;

		result_row_t(const raw_result_row_t& raw_result_row): 
			detail::result_row_impl<0, 0, NamedExpr...>(raw_result_row),
			_is_row(raw_result_row.data != nullptr)
		{}

		result_row_t& operator=(const raw_result_row_t& raw_result_row)
		{
			detail::result_row_impl<0, 0, NamedExpr...>::operator=(raw_result_row);
			_is_row = raw_result_row.data != nullptr;
			return *this;
		}

		explicit operator bool() const
		{
			return _is_row;
		}
	};
}

#endif

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
#include <sqlpp11/text.h>
#include <iostream>
#include <map>

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
				static constexpr size_t _last_index = _rest::_last_index;

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
				static constexpr size_t _last_index = _rest::_last_index;

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
				static constexpr size_t _last_index = index;
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
		using _impl = detail::result_row_impl<0, 0, NamedExpr...>;
		bool _is_row;
		raw_result_row_t _raw_result_row;

		result_row_t():
			_impl({}),
			_raw_result_row({}),
			_is_row(false)
		{
		}

		template<typename T>
		result_row_t(const raw_result_row_t& raw_result_row, const T&):
			_impl(raw_result_row),
			_raw_result_row(raw_result_row),
			_is_row(_raw_result_row.data != nullptr)
		{
		}

		result_row_t(const result_row_t&) = delete;
		result_row_t(result_row_t&&) = default;
		result_row_t& operator=(const result_row_t&) = delete;
		result_row_t& operator=(result_row_t&&) = default;

		result_row_t& operator=(const raw_result_row_t& raw_result_row)
		{
			_impl::operator=(raw_result_row);
			_raw_result_row = raw_result_row;
			_is_row = _raw_result_row.data != nullptr;
			return *this;
		}

		bool operator==(const result_row_t& rhs) const
		{
			return _raw_result_row == rhs._raw_result_row;
		}

		explicit operator bool() const
		{
			return _is_row;
		}

		static constexpr size_t static_size()
		{
			return sizeof...(NamedExpr);
		}
	};

	template<typename... NamedExpr>
	struct dynamic_result_row_t: public detail::result_row_impl<0, 0, NamedExpr...>
	{
		using _impl = detail::result_row_impl<0, 0, NamedExpr...>;
		using _field_type = detail::text::_result_entry_t<0>;
		static constexpr size_t _last_static_index = _impl::_last_index;

		raw_result_row_t _raw_result_row;
		bool _is_row;
		std::vector<std::string> _dynamic_columns;
		std::map<std::string, _field_type> _dynamic_fields;

		dynamic_result_row_t(): 
			_impl({}),
			_raw_result_row({}),
			_is_row(false)
		{
		}

		dynamic_result_row_t(const raw_result_row_t& raw_result_row, std::vector<std::string> dynamic_columns): 
			_impl(raw_result_row),
			_raw_result_row(raw_result_row),
			_is_row(raw_result_row.data != nullptr),
			_dynamic_columns(dynamic_columns)
		{
			raw_result_row_t dynamic_row = raw_result_row;
			if (_is_row)
			{
				dynamic_row.data += _last_static_index;
				dynamic_row.len += _last_static_index;
				for (const auto& column : _dynamic_columns)
				{
					_dynamic_fields.insert(std::make_pair(column, _field_type(dynamic_row)));
					++dynamic_row.data;
					++dynamic_row.len;
				}
			}
			else
			{
				for (const auto& column : _dynamic_columns)
				{
					_dynamic_fields.insert(std::make_pair(column, _field_type(dynamic_row)));
				}
			}

		}

		dynamic_result_row_t(const dynamic_result_row_t&) = delete;
		dynamic_result_row_t(dynamic_result_row_t&&) = default;
		dynamic_result_row_t& operator=(const dynamic_result_row_t&) = delete;
		dynamic_result_row_t& operator=(dynamic_result_row_t&&) = default;

		dynamic_result_row_t& operator=(const raw_result_row_t& raw_result_row)
		{
			_impl::operator=(raw_result_row);
			_raw_result_row = raw_result_row;
			_is_row = raw_result_row.data != nullptr;

			raw_result_row_t dynamic_row = raw_result_row;
			if (_is_row)
			{
				dynamic_row.data += _last_static_index;
				dynamic_row.len += _last_static_index;
				for (const auto& column : _dynamic_columns)
				{
					_dynamic_fields.at(column) = dynamic_row;
					++dynamic_row.data;
					++dynamic_row.len;
				}
			}
			else
			{
				for (const auto& column : _dynamic_columns)
				{
					_dynamic_fields.at(column) = dynamic_row;
				}
			}

			return *this;
		}

		bool operator==(const dynamic_result_row_t& rhs) const
		{
			return _raw_result_row == rhs._raw_result_row;
		}

		const _field_type& at(const std::string& field_name) const
		{
			return _dynamic_fields.at(field_name);
		}

		explicit operator bool() const
		{
			return _is_row;
		}

	};
}

#endif

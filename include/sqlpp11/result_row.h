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

#include <sqlpp11/vendor/char_result_row.h>
#include <sqlpp11/vendor/field.h>
#include <sqlpp11/text.h>
#include <map>

namespace sqlpp
{
	namespace detail
	{
		template<size_t> struct index_t {}; // this is just for overloading

		template<size_t level, size_t index, typename... NamedExpr>
			struct result_row_impl;

		template<size_t level, size_t index, typename NamedExpr, typename... Rest>
			struct result_row_impl<level, index, NamedExpr, Rest...>: 
			public NamedExpr::_name_t::template _member_t<typename NamedExpr::_value_type::_result_entry_t>,
			public result_row_impl<level, index + 1, Rest...>
			{
				using _field = typename NamedExpr::_name_t::template _member_t<typename NamedExpr::_value_type::_result_entry_t>;
				using _rest = result_row_impl<level, index + 1, Rest...>;
				static constexpr size_t _last_index = _rest::_last_index;

				result_row_impl() = default;
				result_row_impl(const char_result_row_t& char_result_row_t):
					_field({{char_result_row_t.data[index], char_result_row_t.len[index]}}),
					_rest(char_result_row_t)
				{
				}

				result_row_impl& operator=(const char_result_row_t& char_result_row_t)
				{
					_field::operator()().assign(char_result_row_t.data[index], char_result_row_t.len[index]);
					_rest::operator=(char_result_row_t);
					return *this;
				}

				void validate()
				{
					_field::operator()().validate();
					_rest::validate();
				}

				void invalidate()
				{
					_field::operator()().invalidate();
					_rest::invalidate();
				}

				template<typename Target>
				void _bind(Target& target)
				{
					_field::operator()()._bind(target, index);
					_rest::_bind(target);
				}
			};

		template<size_t level, size_t index, typename AliasProvider, typename... Col, typename... Rest>
			struct result_row_impl<level, index, vendor::multi_field_t<AliasProvider, std::tuple<Col...>>, Rest...>: 
			public AliasProvider::_name_t::template _member_t<result_row_impl<level, index, Col...>>, // level prevents identical closures to be present twice in the inheritance tree
			public result_row_impl<level, index + sizeof...(Col), Rest...>
			{
				using _multi_field = typename AliasProvider::_name_t::template _member_t<result_row_impl<level, index, Col...>>;
				using _rest = result_row_impl<level, index + sizeof...(Col), Rest...>;
				static constexpr size_t _last_index = _rest::_last_index;

				result_row_impl() = default;
				result_row_impl(const char_result_row_t& char_result_row_t):
					_multi_field({char_result_row_t}),
					_rest(char_result_row_t)
				{}

				result_row_impl& operator=(const char_result_row_t& char_result_row_t)
				{
					_multi_field::operator()() = char_result_row_t;
					_rest::operator=(char_result_row_t);
					return *this;
				}

				void validate()
				{
					_multi_field::operator()().validate();
					_rest::validate();
				}

				void invalidate()
				{
					_multi_field::operator()().invalidate();
					_rest::invalidate();
				}

				template<typename Target>
				void _bind(Target& target)
				{
					_multi_field::operator()()._bind(target);
					_rest::_bind(target);
				}
			};

		template<size_t level, size_t index>
			struct result_row_impl<level, index>
			{
				static constexpr size_t _last_index = index;
				result_row_impl() = default;
				result_row_impl(const char_result_row_t& char_result_row_t)
				{
				}

				result_row_impl& operator=(const char_result_row_t& char_result_row_t)
				{
					return *this;
				}

				void validate()
				{
				}

				void invalidate()
				{
				}

				template<typename Target>
				void _bind(Target& target)
				{
				}
			};
	}

	template<typename... NamedExpr>
	struct result_row_t: public detail::result_row_impl<0, 0, NamedExpr...>
	{
		using _impl = detail::result_row_impl<0, 0, NamedExpr...>;
		bool _is_valid;
		static constexpr size_t _last_static_index = _impl::_last_index;

		result_row_t():
			_impl(),
			_is_valid(false)
		{
		}

		template<typename DynamicNames>
		result_row_t(const DynamicNames&):
			_impl(),
			_is_valid(false)
		{
		}

		result_row_t(const result_row_t&) = delete;
		result_row_t(result_row_t&&) = default;
		result_row_t& operator=(const result_row_t&) = delete;
		result_row_t& operator=(result_row_t&&) = default;

		result_row_t& operator=(const char_result_row_t& char_result_row_t)
		{
			_impl::operator=(char_result_row_t);
			_is_valid = true;
			return *this;
		}

		void validate()
		{
			_impl::validate();
			_is_valid = true;
		}

		void invalidate()
		{
			_impl::invalidate();
			_is_valid = false;
		}

		bool operator==(const result_row_t& rhs) const
		{
			return _is_valid == rhs._is_valid;
		}

		explicit operator bool() const
		{
			return _is_valid;
		}

		static constexpr size_t static_size()
		{
			return _last_static_index;
		}

		template<typename Target>
			void bind_result(Target& target)
			{
				_impl::_bind(target);
			}
	};

	template<typename... NamedExpr>
	struct dynamic_result_row_t: public detail::result_row_impl<0, 0, NamedExpr...>
	{
		using _impl = detail::result_row_impl<0, 0, NamedExpr...>;
		using _field_type = detail::text::_result_entry_t;
		static constexpr size_t _last_static_index = _impl::_last_index;

		bool _is_valid;
		std::vector<std::string> _dynamic_columns;
		std::map<std::string, _field_type> _dynamic_fields;

		dynamic_result_row_t(): 
			_impl(),
			_is_valid(false)
		{
		}

		dynamic_result_row_t(const std::vector<std::string>& dynamic_columns): 
			_impl(),
			_is_valid(false),
			_dynamic_columns(dynamic_columns)
		{
		}

		dynamic_result_row_t(const dynamic_result_row_t&) = delete;
		dynamic_result_row_t(dynamic_result_row_t&&) = default;
		dynamic_result_row_t& operator=(const dynamic_result_row_t&) = delete;
		dynamic_result_row_t& operator=(dynamic_result_row_t&&) = default;

		dynamic_result_row_t& operator=(const char_result_row_t& char_result_row)
		{
			_impl::operator=(char_result_row);
			_is_valid = true;

			char_result_row_t dynamic_row = char_result_row;

			dynamic_row.data += _last_static_index;
			dynamic_row.len += _last_static_index;
			for (const auto& column : _dynamic_columns)
			{
				_dynamic_fields[column].assign(dynamic_row.data[0], dynamic_row.len[0]);
				++dynamic_row.data;
				++dynamic_row.len;
			}
			return *this;
		}

		void invalidate()
		{
			_impl::invalidate();
			_is_valid = false;
			for (auto& field : _dynamic_fields)
			{
				field.second.invalidate();
			}
		}

		bool operator==(const dynamic_result_row_t& rhs) const
		{
			return _is_valid == rhs._is_valid;
		}

		const _field_type& at(const std::string& field_name) const
		{
			return _dynamic_fields.at(field_name);
		}

		explicit operator bool() const
		{
			return _is_valid;
		}

	};
}

#endif

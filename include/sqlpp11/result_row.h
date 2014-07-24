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

#ifndef SQLPP_RESULT_ROW_H
#define SQLPP_RESULT_ROW_H

#include <map>
#include <sqlpp11/field.h>
#include <sqlpp11/text.h>
#include <sqlpp11/detail/column_index_sequence.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename Db, typename IndexSequence, typename... FieldSpecs>
			struct result_row_impl;

		template<typename Db, std::size_t index, typename FieldSpec>
			struct result_field:
				public FieldSpec::_name_t::template _member_t<typename value_type_of<FieldSpec>::template _result_entry_t<Db, FieldSpec>>
		{
			using _field = typename FieldSpec::_name_t::template _member_t<typename value_type_of<FieldSpec>::template _result_entry_t<Db, FieldSpec>>;

			result_field() = default;

			void _validate()
			{
				_field::operator()()._validate();
			}

			void _invalidate()
			{
				_field::operator()()._invalidate();
			}

			template<typename Target>
				void _bind(Target& target)
				{
					_field::operator()()._bind(target, index);
				}
		};

		template<std::size_t index, typename AliasProvider, typename Db, typename... FieldSpecs>
			struct result_field<Db, index, multi_field_t<AliasProvider, std::tuple<FieldSpecs...>>>: 
			public AliasProvider::_name_t::template _member_t<result_row_impl<Db, detail::make_column_index_sequence<index, FieldSpecs...>, FieldSpecs...>>
			{
				using _multi_field = typename AliasProvider::_name_t::template _member_t<result_row_impl<Db, detail::make_column_index_sequence<index, FieldSpecs...>, FieldSpecs...>>;

				result_field() = default;

				void _validate()
				{
					_multi_field::operator()()._validate();
				}

				void _invalidate()
				{
					_multi_field::operator()()._invalidate();
				}

				template<typename Target>
					void _bind(Target& target)
					{
						_multi_field::operator()()._bind(target);
					}
			};

		template<typename Db, std::size_t LastIndex, std::size_t... Is, typename... FieldSpecs>
			struct result_row_impl<Db, detail::column_index_sequence<LastIndex, Is...>, FieldSpecs...>: 
			public result_field<Db, Is, FieldSpecs>...
			{
				static constexpr std::size_t _last_index = LastIndex;

				result_row_impl() = default;

				void _validate()
				{
					using swallow = int[];
					(void) swallow{(result_field<Db, Is, FieldSpecs>::_validate(), 0)...};
				}

				void _invalidate()
				{
					using swallow = int[];
					(void) swallow{(result_field<Db, Is, FieldSpecs>::_invalidate(), 0)...};
				}

				template<typename Target>
					void _bind(Target& target)
					{
						using swallow = int[];
						(void) swallow{(result_field<Db, Is, FieldSpecs>::_bind(target), 0)...};
					}
			};

	}

	template<typename Db, typename... FieldSpecs>
		struct result_row_t: public detail::result_row_impl<Db, detail::make_column_index_sequence<0, FieldSpecs...>, FieldSpecs...>
	{
		using _impl = detail::result_row_impl<Db, detail::make_column_index_sequence<0, FieldSpecs...>, FieldSpecs...>;
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

		void _validate()
		{
			_impl::_validate();
			_is_valid = true;
		}

		void _invalidate()
		{
			_impl::_invalidate();
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
			void _bind(Target& target)
			{
				_impl::_bind(target);
			}
	};

	template<typename Db, typename... FieldSpecs>
		struct dynamic_result_row_t: public detail::result_row_impl<Db, detail::make_column_index_sequence<0, FieldSpecs...>, FieldSpecs...>
	{
		using _impl = detail::result_row_impl<Db, detail::make_column_index_sequence<0, FieldSpecs...>, FieldSpecs...>;
		struct _field_spec_t
		{
			using _traits = make_traits<detail::text, tag::noop, tag::can_be_null, tag::null_is_trivial_value>;
			using _recursive_traits = make_recursive_traits<>;

			struct _name_t {};
		};
		using _field_type = detail::text::_result_entry_t<Db, _field_spec_t>;
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
			for (auto name : _dynamic_columns)
			{
				_dynamic_fields.insert({name, _field_type{}});
			}
		}

		dynamic_result_row_t(const dynamic_result_row_t&) = delete;
		dynamic_result_row_t(dynamic_result_row_t&&) = default;
		dynamic_result_row_t& operator=(const dynamic_result_row_t&) = delete;
		dynamic_result_row_t& operator=(dynamic_result_row_t&&) = default;

		void _validate()
		{

			_impl::_validate();
			_is_valid = true;
			for (auto& field : _dynamic_fields)
			{
				field.second._validate();
			}
		}

		void _invalidate()
		{
			_impl::_invalidate();
			_is_valid = false;
			for (auto& field : _dynamic_fields)
			{
				field.second._invalidate();
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

		template<typename Target>
			void _bind(Target& target)
			{
				_impl::_bind(target);

				std::size_t index = _last_static_index;
				for (const auto& name  : _dynamic_columns)
				{
					_dynamic_fields.at(name)._bind(target, ++index);
				}
			}
	};
}

#endif

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

#ifndef SQLPP_TEXT_H
#define SQLPP_TEXT_H

#include <sqlpp11/basic_operators.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/vendor/concat.h>
#include <sqlpp11/vendor/like.h>

namespace sqlpp
{
	namespace detail
	{
		// text value type
		struct text
		{
			using _base_value_type = text;
			using _is_text = std::true_type;
			using _is_value = std::true_type;
			using _is_expression = std::true_type;
			using _cpp_value_type = std::string;

			struct _parameter_t
			{
				using _value_type = integral;

				_parameter_t():
					_value(""),
					_is_null(true)
					{}

				_parameter_t(const _cpp_value_type& value):
					_value(value),
					_is_null(false)
					{}

				_parameter_t& operator=(const _cpp_value_type& value)
				{
					_value = value;
					_is_null = false;
					return *this;
				}

				_parameter_t& operator=(const std::nullptr_t&)
				{
					_value = "";
					_is_null = true;
					return *this;
				}

				bool is_null() const
			 	{ 
					return _is_null; 
				}

				_cpp_value_type value() const
				{
					return _value;
				}

				operator _cpp_value_type() const { return value(); }

				template<typename Target>
					void _bind(Target& target, size_t index) const
					{
						target._bind_text_parameter(index, &_value, _is_null);
					}

			private:
				_cpp_value_type _value;
				bool _is_null;
			};

			struct _result_entry_t
			{
				_result_entry_t():
					_is_valid(false),
					_value_ptr(nullptr),
					_len(0)
					{}

				_result_entry_t(char* data, size_t len):
					_is_valid(true),
					_value_ptr(data),
					_len(_value_ptr ? 0 : len)
					{}

				void assign(const char* data, size_t len)
				{
					_is_valid = true;
					_value_ptr = data;
					_len = _value_ptr ? len: 0;
				}

				void validate()
				{
					_is_valid = true;
				}

				void invalidate()
				{
					_is_valid = false;
					_value_ptr = nullptr;
					_len = 0;
				}

				bool operator==(const _cpp_value_type& rhs) const { return value() == rhs; }
				bool operator!=(const _cpp_value_type& rhs) const { return not operator==(rhs); }

				bool is_null() const
			 	{ 
					if (not _is_valid)
						throw exception("accessing is_null in non-existing row");
					return _value_ptr == nullptr; 
				}

				_cpp_value_type value() const
				{
					if (not _is_valid)
						throw exception("accessing value in non-existing row");
					if (_value_ptr)
						return std::string(_value_ptr, _value_ptr + _len);
					else
						return "";
				}

				operator _cpp_value_type() const { return value(); }

				template<typename Target>
					void _bind(Target& target, size_t i)
					{
						target._bind_text_result(i, &_value_ptr, &_len);
					}

			private:
				bool _is_valid;
				const char* _value_ptr;
				size_t _len;
			};

			template<typename T>
				using _operand_t = operand_t<T, is_text_t>;
			template<typename T>
				using _constraint = is_text_t<T>;

			template<typename Base>
				struct operators: public basic_operators<Base, _operand_t>
			{
				template<typename T>
					vendor::concat_t<Base, typename _operand_t<T>::type> operator+(T t) const
					{
						static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
						return { *static_cast<const Base*>(this), {t} };
					}

				template<typename T>
					vendor::like_t<Base, typename _operand_t<T>::type> like(T t) const
					{
						static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
						return { *static_cast<const Base*>(this), {t} };
					}

			};
		};

		inline std::ostream& operator<<(std::ostream& os, const text::_result_entry_t& e)
		{
			return os << e.value();
		}
	}

	using text = detail::text;
	using varchar = detail::text;
	using char_ = detail::text;

}
#endif

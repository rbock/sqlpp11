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

#ifndef SQLPP_BOOLEAN_H
#define SQLPP_BOOLEAN_H

#include <cstdlib>
#include <ostream>
#include <sqlpp11/basic_operators.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/exception.h>

namespace sqlpp
{
	// boolean operators
	namespace detail
	{
		// boolean value type
		struct boolean
		{
			using _base_value_type = boolean;
			using _is_boolean = std::true_type;
			using _is_value = std::true_type;
			using _is_expression = std::true_type;
			using _cpp_value_type = bool;

			struct _parameter_t
			{
				using _value_type = boolean;

				_parameter_t():
					_value(false),
					_is_null(true)
					{}

				_parameter_t(const _cpp_value_type& value):
					_value(value),
					_is_null(false)
					{}

				_parameter_t& operator=(const _cpp_value_type& value)
				{
					_value = value;
					_is_null = (false);
					return *this;
				}

				_parameter_t& operator=(const std::nullptr_t&)
				{
					_value = false;
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
						target._bind_boolean_parameter(index, &_value, _is_null);
					}

			private:
				signed char _value;
				bool _is_null;
			};

			struct _result_entry_t
			{
				_result_entry_t():
					_is_valid(false),
					_is_null(true),
					_value(false)
					{}

				_result_entry_t(const char* data, size_t):
					_is_valid(true),
					_is_null(data == nullptr),
					_value(_is_null ? false : (data[0] == 't' or data[0] == '1'))
					{}

				void assign(const char* data, size_t)
				{
					_is_valid = true;
					_is_null = data == nullptr;
					_value = _is_null ? false : (data[0] == 't' or data[0] == '1');
				}

				void validate()
				{
					_is_valid = true;
				}

				void invalidate()
				{
					_is_valid = false;
					_is_null = true;
					_value = 0;
				}

				bool is_null() const
			 	{ 
					if (not _is_valid)
						throw exception("accessing is_null in non-existing row");
					return _is_null; 
				}

				_cpp_value_type value() const
				{
					if (not _is_valid)
						throw exception("accessing value in non-existing row");
					return _value;
				}

				operator _cpp_value_type() const { return value(); }

				template<typename Target>
					void _bind(Target& target, size_t i)
					{
						target._bind_boolean_result(i, &_value, &_is_null);
					}

			private:
				bool _is_valid;
				bool _is_null;
				signed char _value;
			};

			template<typename T>
				using _operand_t = operand_t<T, is_boolean_t>;
			template<typename T>
				using _constraint = is_boolean_t<T>;

			template<typename Base>
				struct operators: public basic_operators<Base, _operand_t>
			{
				template<typename T>
					vendor::logical_and_t<Base, typename _operand_t<T>::type> operator and(T t) const
					{
						static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
						return { *static_cast<const Base*>(this), {t} };
					}

				template<typename T>
					vendor::logical_or_t<Base, typename _operand_t<T>::type> operator or(T t) const
					{
						static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
						return { *static_cast<const Base*>(this), {t} };
					}

				vendor::logical_not_t<Base> operator not() const
				{
					static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be as operand for operator not");
					return { *static_cast<const Base*>(this) };
				}
			};
		};

		inline std::ostream& operator<<(std::ostream& os, const boolean::_result_entry_t& e)
		{
			return os << e.value();
		}
	}

	using boolean = detail::boolean;

}
#endif

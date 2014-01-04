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

#ifndef SQLPP_FLOATING_POINT_H
#define SQLPP_FLOATING_POINT_H

#include <cstdlib>
#include <sqlpp11/detail/basic_operators.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/char_result_row.h> // FIXME: Need to update floating_point
#include <sqlpp11/exception.h>

namespace sqlpp
{
	namespace detail
	{

		// floating_point value type
		struct floating_point
		{
			using _base_value_type = floating_point;
			using _is_numeric = std::true_type;
			using _is_floating_point = std::true_type;
			using _is_value = std::true_type;
			using _is_expression = std::true_type;
			using _cpp_value_type = double;
			
			struct _parameter_t
			{
				using _value_type = integral;

				_parameter_t(const std::true_type&):
					_trivial_value_is_null(true),
					_value(0),
					_is_null(_trivial_value_is_null	and _is_trivial())
					{}

				_parameter_t(const std::false_type&):
					_trivial_value_is_null(false),
					_value(0),
					_is_null(_trivial_value_is_null	and _is_trivial())
					{}

				_parameter_t(const _cpp_value_type& value):
					_value(value),
					_is_null(_trivial_value_is_null and _is_trivial())
					{}

				_parameter_t& operator=(const _cpp_value_type& value)
				{
					_value = value;
					_is_null = (_trivial_value_is_null and _is_trivial());
					return *this;
				}

				_parameter_t& operator=(const std::nullptr_t&)
				{
					_value = 0;
					_is_null = true;
					return *this;
				}

				template<typename Db>
					void serialize(std::ostream& os, Db& db) const
					{
						os << value();
					}

				bool _is_trivial() const { return value() == 0; }

				bool is_null() const
			 	{ 
					return _is_null; 
				}

				const _cpp_value_type& value() const
				{
					return _value;
				}

				operator _cpp_value_type() const { return _value; }

				template<typename Target>
					void bind(Target& target, size_t index) const
					{
						target.bind_floating_point_parameter(index, &_value, _is_null);
					}

			private:
				bool _trivial_value_is_null;
				_cpp_value_type _value;
				bool _is_null;
			};

			struct _result_entry_t
			{
				using _value_type = integral;

				_result_entry_t():
					_is_valid(false),
					_is_null(true),
					_value(0)
					{}

				_result_entry_t(const char* data, size_t):
					_is_valid(true),
					_is_null(data == nullptr),
					_value(_is_null ? 0 : std::strtoll(data, nullptr, 10))
					{}

				void assign(const char* data, size_t)
				{
					_is_valid = true;
					_is_null = data == nullptr;
					_value = _is_null ? 0 : std::strtoll(data, nullptr, 10);
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

				template<typename Db>
					void serialize(std::ostream& os, Db& db) const
					{
						os << value();
					}

				bool _is_trivial() const { return value() == 0; }

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
					void bind(Target& target, size_t i)
					{
						target.bind_floating_point_result(i, &_value, &_is_null);
					}

			private:
				bool _is_valid;
				bool _is_null;
				_cpp_value_type _value;
			};

			struct plus_
			{
				using _value_type = floating_point;
				static constexpr const char* _name = "+";
			};

			struct minus_
			{
				using _value_type = floating_point;
				static constexpr const char* _name = "-";
			};

			struct multiplies_
			{
				using _value_type = floating_point;
				static constexpr const char* _name = "*";
			};

			struct divides_
			{
				using _value_type = floating_point;
				static constexpr const char* _name = "/";
			};

			template<typename T>
				using _constraint = operand_t<T, is_numeric_t>;

			template<typename Base>
				struct operators: public basic_operators<Base, _constraint>
			{
				template<typename T>
					binary_expression_t<Base, plus_, typename _constraint<T>::type> operator +(T&& t) const
					{
						static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
						return { *static_cast<const Base*>(this), std::forward<T>(t) };
					}

				template<typename T>
					binary_expression_t<Base, minus_, typename _constraint<T>::type> operator -(T&& t) const
					{
						static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
						return { *static_cast<const Base*>(this), std::forward<T>(t) };
					}

				template<typename T>
					binary_expression_t<Base, multiplies_, typename _constraint<T>::type> operator *(T&& t) const
					{
						static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
						return { *static_cast<const Base*>(this), std::forward<T>(t) };
					}

				template<typename T>
					binary_expression_t<Base, divides_, typename _constraint<T>::type> operator /(T&& t) const
					{
						static_assert(not is_multi_expression_t<Base>::value, "multi-expression cannot be used as left hand side operand");
						return { *static_cast<const Base*>(this), std::forward<T>(t) };
					}

				template<typename T>
					auto operator +=(T&& t) const -> decltype(std::declval<Base>() = std::declval<Base>() + std::forward<T>(t))
					{
						return *static_cast<const Base*>(this) = operator +(std::forward<T>(t));
					}

				template<typename T>
					auto operator -=(T&& t) const -> decltype(std::declval<Base>() = std::declval<Base>() - std::forward<T>(t))
					{
						return *static_cast<const Base*>(this) = operator -(std::forward<T>(t));
					}

				template<typename T>
					auto operator /=(T&& t) const -> decltype(std::declval<Base>() = std::declval<Base>() / std::forward<T>(t))
					{
						return *static_cast<const Base*>(this) = operator /(std::forward<T>(t));
					}

				template<typename T>
					auto operator *=(T&& t) const -> decltype(std::declval<Base>() = std::declval<Base>() * std::forward<T>(t))
					{
						return *static_cast<const Base*>(this) = operator *(std::forward<T>(t));
					}


			};
		};

		inline std::ostream& operator<<(std::ostream& os, const floating_point::_result_entry_t& e)
		{
			return os << e.value();
		}
	}

	using floating_point = detail::floating_point;

}
#endif

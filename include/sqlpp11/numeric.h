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

#ifndef SQLPP_NUMERIC_H
#define SQLPP_NUMERIC_H

#include <cstdlib>
#include <sqlpp11/detail/basic_operators.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/raw_result_row.h>
#include <sqlpp11/exception.h>

namespace sqlpp
{
	namespace detail
	{

		// numeric value type
		struct numeric
		{
			using _base_value_type = numeric;
			using _is_numeric = tag_yes;
			using _is_value = tag_yes;
			using _is_expression = tag_yes;
			
			template<size_t index>
			struct _result_entry_t
			{
				using _value_type = numeric;
				_result_entry_t(const raw_result_row_t& row):
					_is_valid(row.data != nullptr),
					_is_null(row.data == nullptr or row.data[index] == nullptr),
					_value(_is_null ? 0 : std::strtoll(row.data[index], nullptr, 10))
					{}

				_result_entry_t& operator=(const raw_result_row_t& row)
				{
					_is_valid = (row.data != nullptr);
					_is_null = row.data == nullptr or row.data[index] == nullptr;
					_value = _is_null ? 0 : std::strtoll(row.data[index], nullptr, 10);
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
					if (not _is_valid)
						throw exception("accessing is_null in non-existing row");
					return _is_null; 
				}

				int64_t value() const
				{
					if (not _is_valid)
						throw exception("accessing value in non-existing row");
					return _value;
				}

				operator int64_t() const { return value(); }

			private:
				bool _is_valid;
				bool _is_null;
				int64_t _value;
			};

			struct plus_
			{
				using _value_type = numeric;
				static constexpr const char* _name = "+";
			};

			struct minus_
			{
				using _value_type = numeric;
				static constexpr const char* _name = "-";
			};

			struct multiplies_
			{
				using _value_type = numeric;
				static constexpr const char* _name = "*";
			};

			struct divides_
			{
				using _value_type = numeric;
				static constexpr const char* _name = "/";
			};

			template<typename T>
				using _constraint = operand_t<T, is_numeric_t>;

			template<typename Base>
				struct operators: public basic_operators<Base, _constraint>
			{
				template<typename T>
					nary_expression_t<Base, plus_, typename _constraint<T>::type> operator +(T&& t) const
					{
						return { *static_cast<const Base*>(this), std::forward<T>(t) };
					}

				template<typename T>
					nary_expression_t<Base, minus_, typename _constraint<T>::type> operator -(T&& t) const
					{
						return { *static_cast<const Base*>(this), std::forward<T>(t) };
					}

				template<typename T>
					nary_expression_t<Base, multiplies_, typename _constraint<T>::type> operator *(T&& t) const
					{
						return { *static_cast<const Base*>(this), std::forward<T>(t) };
					}

				template<typename T>
					nary_expression_t<Base, divides_, typename _constraint<T>::type> operator /(T&& t) const
					{
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

		template<size_t index>
		std::ostream& operator<<(std::ostream& os, const numeric::_result_entry_t<index>& e)
		{
			return os << e.value();
		}
	}

	using tinyint = detail::numeric;
	using smallint = detail::numeric;
	using integer = detail::numeric;
	using bigint = detail::numeric;

}
#endif

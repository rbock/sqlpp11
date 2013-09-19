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

#include <cstdlib>
#include <sqlpp11/detail/basic_operators.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/raw_result_row.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/concat.h>

namespace sqlpp
{
	namespace detail
	{
		struct text;

		struct like_
		{
			using _value_type = boolean;
			static constexpr const char* _name = "LIKE";
		};

		// text value type
		struct text
		{
			using _base_value_type = text;
			using _is_text = tag_yes;
			using _is_value = tag_yes;
			using _is_expression = tag_yes;

			template<size_t index>
			struct _result_entry_t
			{
				_result_entry_t(const raw_result_row_t& row):
					_is_valid(row.data != nullptr),
					_is_null(row.data == nullptr or row.data[index] == nullptr),
					_value(_is_null ? "" : std::string(row.data[index], row.data[index] + row.len[index]))
					{}

				_result_entry_t& operator=(const raw_result_row_t& row)
				{
					_is_valid = (row.data != nullptr);
					_is_null = row.data == nullptr or row.data[index] == nullptr;
					_value = _is_null ? "" : std::string(row.data[index], row.data[index] + row.len[index]);
					return *this;
				}

				template<typename Db>
					void serialize(std::ostream& os, Db& db) const
					{
						os << value();
					}

				bool _is_trivial() const { return value().empty(); }

				bool operator==(const std::string& rhs) const { return value() == rhs; }
				bool operator!=(const std::string& rhs) const { return not operator==(rhs); }

				bool is_null() const
			 	{ 
					if (not _is_valid)
						throw exception("accessing is_null in non-existing row");
					return _is_null; 
				}

				std::string value() const
				{
					if (not _is_valid)
						throw exception("accessing value in non-existing row");
					return _value;
				}

				operator std::string() const { return value(); }

			private:
				bool _is_valid;
				bool _is_null;
				std::string _value;
			};

			template<typename T>
				using _constraint = operand_t<T, is_text_t>;

			template<typename Base>
				struct operators: public basic_operators<Base, _constraint>
			{
				template<typename T>
					detail::concat_t<text, Base, typename _constraint<T>::type> operator+(T&& t) const
					{
						return { *static_cast<const Base*>(this), std::forward<T>(t) };
					}

				template<typename T>
					nary_member_function_t<Base, like_, typename _constraint<T>::type> like(T&& t) const
					{
						return { *static_cast<const Base*>(this), std::forward<T>(t) };
					}

			};
		};

		template<size_t index>
		std::ostream& operator<<(std::ostream& os, const text::_result_entry_t<index>& e)
		{
			return os << e.value();
		}
	}

	using text = detail::text;
	using varchar = detail::text;
	using char_ = detail::text;

}
#endif

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

#ifndef SQLPP_IS_NULL_H
#define SQLPP_IS_NULL_H

#include <sstream>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/set.h>

namespace sqlpp
{
	namespace detail
	{
		// The ValueType should be boolean, this is a hack because boolean is not fully defined when the compiler first gets here...
		template<bool NotInverted, typename ValueType, typename Operand>
		struct is_null_t: public ValueType::_base_value_type::template operators<is_null_t<NotInverted, ValueType, Operand>>
		{
			static constexpr bool _inverted = not NotInverted;

			struct _value_type: public ValueType::_base_value_type // we requite fully defined boolean here
			{
				using _is_named_expression = std::true_type;
			};

			struct _name_t
			{
				static constexpr const char* _get_name() { return _inverted ? "IS NOT NULL" : "IS NULL"; }
				template<typename T>
					struct _member_t
					{
						T in;
					};
			};

			is_null_t(const Operand& operand):
				_operand(operand)
			{}

			is_null_t(Operand&& operand):
				_operand(std::move(operand))
			{}

			is_null_t(const is_null_t&) = default;
			is_null_t(is_null_t&&) = default;
			is_null_t& operator=(const is_null_t&) = default;
			is_null_t& operator=(is_null_t&&) = default;
			~is_null_t() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					static_assert(NotInverted and Db::_supports_is_null
							or _inverted and Db::_supports_is_not_null, "is_null() and/or is_not_null() not supported by current database");
					_operand.serialize(os, db);
					os << (_inverted ? " IS NOT NULL" : " IS NULL");
				}

		private:
			Operand _operand;
		};
	}
}

#endif

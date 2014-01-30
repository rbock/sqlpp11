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

#ifndef SQLPP_SOME_H
#define SQLPP_SOME_H

#include <sstream>
#include <sqlpp11/boolean.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename Select>
		struct some_t: public boolean::template operators<some_t<Select>>
		{
			static_assert(is_select_t<Select>::value, "some() requires a single column select expression as argument");
			static_assert(is_value_t<Select>::value, "some() requires a single column select expression as argument");

			struct _value_type: public Select::_value_type::_base_value_type
			{
				using _is_multi_expression = std::true_type; // must not be named
			};

			struct _name_t
			{
				static constexpr const char* _get_name() { return "SOME"; }
				template<typename T>
					struct _member_t
					{
						T some;
						T& operator()() { return some; }
						const T& operator()() const { return some; }
					};
			};

			some_t(Select select):
				_select(select)
			{}

			some_t(const some_t&) = default;
			some_t(some_t&&) = default;
			some_t& operator=(const some_t&) = default;
			some_t& operator=(some_t&&) = default;
			~some_t() = default;

			Select _select;
		};
	}

	namespace vendor
	{
		template<typename Context, typename Select>
			struct interpreter_t<Context, vendor::some_t<Select>>
			{
				using T = vendor::some_t<Select>;

				static Context& _(const T& t, Context& context)
				{
					context << "SOME(";
					interpret(t._select, context);
					context << ")";
					return context;
				}
			};
	}

	template<typename T>
		auto some(T t) -> typename vendor::some_t<typename operand_t<T, is_select_t>::type>
		{
			return { t };
		}

}

#endif

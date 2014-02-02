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

#ifndef SQLPP_CONCAT_H
#define SQLPP_CONCAT_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/detail/logic.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename First, typename... Args>
			struct concat_t: public First::_value_type::template operators<concat_t<First, Args...>>
		{
			static_assert(sizeof...(Args) > 0, "concat requires two arguments at least");
			static_assert(sqlpp::detail::and_t<is_text_t, First, Args...>::value, "at least one non-text argument detected in concat()");

			struct _value_type: public First::_value_type::_base_value_type
			{
				using _is_named_expression = std::true_type;
			};

			struct _name_t
			{
				static constexpr const char* _get_name() { return "CONCAT"; }
				template<typename T>
					struct _member_t
					{
						T concat;
					};
			};

			concat_t(First first, Args... args):
				_args(first, args...)
			{}

			concat_t(const concat_t&) = default;
			concat_t(concat_t&&) = default;
			concat_t& operator=(const concat_t&) = default;
			concat_t& operator=(concat_t&&) = default;
			~concat_t() = default;

			std::tuple<First, Args...> _args;
		};

		template<typename Context, typename First, typename... Args>
			struct interpreter_t<Context, concat_t<First, Args...>>
			{
				using T = concat_t<First, Args...>;

				static Context& _(const T& t, Context& context)
				{
					context << "(";
					interpret_tuple(t._args, "||", context);
					context << ")";
					return context;
				}
			};
	}
}

#endif

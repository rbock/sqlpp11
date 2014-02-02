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

#ifndef SQLPP_LIKE_H
#define SQLPP_LIKE_H

#include <sqlpp11/boolean.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename Operand, typename Pattern>
			struct like_t: public boolean::template operators<like_t<Operand, Pattern>>
		{
			static_assert(is_text_t<Operand>::value, "Operand for like() has to be a text");
			static_assert(is_text_t<Pattern>::value, "Pattern for like() has to be a text");
			using _parameter_tuple_t = std::tuple<Operand, Pattern>;

			struct _value_type: public boolean
			{
				using _is_named_expression = std::true_type;
			};

			struct _name_t
			{
				static constexpr const char* _get_name() { return "LIKE"; }
				template<typename T>
					struct _member_t
					{
						T like;
					};
			};

			like_t(Operand operand, Pattern pattern):
				_operand(operand),
				_pattern(pattern)
			{}

			like_t(const like_t&) = default;
			like_t(like_t&&) = default;
			like_t& operator=(const like_t&) = default;
			like_t& operator=(like_t&&) = default;
			~like_t() = default;

			Operand _operand;
			Pattern _pattern;
		};

		template<typename Context, typename Operand, typename Pattern>
			struct interpreter_t<Context, like_t<Operand, Pattern>>
			{
				using T = like_t<Operand, Pattern>;

				static Context& _(const T& t, Context& context)
				{
					interpret(t._operand, context);
					context << " LIKE(";
					interpret(t._pattern, context);
					context << ")";
					return context;
				}
			};
	}
}

#endif

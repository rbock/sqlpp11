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

#ifndef SQLPP_IN_H
#define SQLPP_IN_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/boolean.h>
#include <sqlpp11/vendor/in_fwd.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
	namespace vendor
	{
		template<bool NotInverted, typename Operand, typename... Args>
			struct in_t: public boolean::template operators<in_t<NotInverted, Operand, Args...>>
		{
			static constexpr bool _inverted = not NotInverted;
			static_assert(sizeof...(Args) > 0, "in() requires at least one argument");

			struct _value_type: public boolean
			{
				using _is_named_expression = std::true_type;
			};

			struct _name_t
			{
				static constexpr const char* _get_name() { return _inverted ? "NOT IN" : "IN"; }
				template<typename T>
					struct _member_t
					{
						T in;
					};
			};

			in_t(Operand operand, Args... args):
				_operand(operand),
				_args(args...)
			{}

			in_t(const in_t&) = default;
			in_t(in_t&&) = default;
			in_t& operator=(const in_t&) = default;
			in_t& operator=(in_t&&) = default;
			~in_t() = default;

			Operand _operand;
			std::tuple<Args...> _args;
		};

		template<typename Context, bool NotInverted, typename Operand, typename... Args>
			struct interpreter_t<Context, vendor::in_t<NotInverted, Operand, Args...>>
			{
				using T = vendor::in_t<NotInverted, Operand, Args...>;

				static Context& _(const T& t, Context& context)
				{
					interpret(t._operand, context);
					context << (t._inverted ? " NOT IN(" : " IN(");
					interpret_tuple(t._args, ',', context);
					context << ')';
					return context;
				}
			};
	}

}

#endif

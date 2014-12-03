/*
 * Copyright (c) 2013-2014, Roland Bock
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

#ifndef SQLPP_NOT_IN_H
#define SQLPP_NOT_IN_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/boolean.h>
#include <sqlpp11/in_fwd.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
	template<typename Operand, typename... Args>
		struct not_in_t:
			public expression_operators<not_in_t<Operand, Args...>, boolean>,
			public alias_operators<not_in_t<Operand, Args...>>
	{
		using _traits = make_traits<boolean, tag::is_expression, tag::is_selectable>;
		using _recursive_traits = make_recursive_traits<Operand, Args...>;

		static_assert(sizeof...(Args) > 0, "not_in() requires at least one argument");

		struct _alias_t
		{
			static constexpr const char _literal[] =  "not_in_";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template<typename T>
				struct _member_t
				{
					T not_in;
				};
		};

		not_in_t(Operand operand, Args... args):
			_operand(operand),
			_args(args...)
		{}

		not_in_t(const not_in_t&) = default;
		not_in_t(not_in_t&&) = default;
		not_in_t& operator=(const not_in_t&) = default;
		not_in_t& operator=(not_in_t&&) = default;
		~not_in_t() = default;

		Operand _operand;
		std::tuple<Args...> _args;
	};

	template<typename Context, typename Operand, typename... Args>
		struct serializer_t<Context, not_in_t<Operand, Args...>>
		{
			using _serialize_check = serialize_check_of<Context, Args...>;
			using T = not_in_t<Operand, Args...>;

			static Context& _(const T& t, Context& context)
			{
				serialize_operand(t._operand, context);
				context << " NOT IN(";
				if (sizeof...(Args) == 1)
					serialize(std::get<0>(t._args), context);
				else
					interpret_tuple(t._args, ',', context);
				context << ')';
				return context;
			}
		};

}

#endif

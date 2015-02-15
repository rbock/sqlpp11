/*
 * Copyright (c) 2013-2015, Roland Bock
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
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/logic.h>

namespace sqlpp
{
	// FIXME: Remove First, inherit from text_t
	template<typename First, typename... Args>
		struct concat_t: 
			public expression_operators<concat_t<First, Args...>, value_type_of<First>>,
			public alias_operators<concat_t<First, Args...>>
	{
		using _traits = make_traits<value_type_of<First>, tag::is_expression, tag::is_selectable>;
		using _recursive_traits = make_recursive_traits<First, Args...>;

		static_assert(sizeof...(Args) > 0, "concat requires two arguments at least");
		static_assert(logic::all_t<is_text_t<First>::value, is_text_t<Args>::value...>::value, "at least one non-text argument detected in concat()");
		struct _alias_t
		{
			static constexpr const char _literal[] =  "concat_";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
		struct serializer_t<Context, concat_t<First, Args...>>
		{
			using _serialize_check = serialize_check_of<Context, First, Args...>;
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

#endif

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

#ifndef SQLPP_INTERPRET_TUPLE_H
#define SQLPP_INTERPRET_TUPLE_H

#include <tuple>
#include <ostream>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
	template<typename Context, typename Tuple>
		struct tuple_interpreter_t
		{
			template<typename Separator>
				static void _(const Tuple& t, const Separator& separator, Context& context)
				{
					_impl(t, separator, context, type<0>());
				};

		private:
			template<size_t> struct type {};

			template<typename Separator, size_t index>
				static void _impl(const Tuple& t, const Separator& separator, Context& context, const type<index>&)
				{
					if (index)
						context << separator;
					const auto& entry = std::get<index>(t);
					using entry_type = typename std::tuple_element<index, Tuple>::type;
					if (requires_braces_t<entry_type>::value)
						context << "(";
					interpret(entry, context);
					if (requires_braces_t<entry_type>::value)
						context << ")";
					_impl(t, separator, context, type<index + 1>());
				}

			template<typename Separator>
				static void _impl(const Tuple& t, const Separator& separator, Context& context, const type<std::tuple_size<Tuple>::value>&)
				{
				}
		};

	template<typename Tuple, typename Separator, typename Context>
		auto interpret_tuple(const Tuple& t, const Separator& separator, Context& context)
		-> decltype(tuple_interpreter_t<Context, Tuple>::_(t, separator, context))
		{
			return tuple_interpreter_t<Context, Tuple>::_(t, separator, context);
		}
}

#endif

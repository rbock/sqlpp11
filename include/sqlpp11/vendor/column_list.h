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

#ifndef SQLPP_COLUMN_LIST_H
#define SQLPP_COLUMN_LIST_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/logic.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <sqlpp11/vendor/simple_column.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename... Columns>
			struct column_list_t
			{
				using _is_column_list = std::true_type;
				using _parameter_tuple_t = std::tuple<Columns...>;

				// check for at least one order column
				static_assert(sizeof...(Columns), "at least one column required in columns()");

				// check for duplicate columns
				static_assert(not ::sqlpp::detail::has_duplicates<Columns...>::value, "at least one duplicate argument detected in columns()");

				// check for invalid columns
				static_assert(::sqlpp::detail::and_t<is_column_t, Columns...>::value, "at least one argument is not a column in columns()");

				// check for prohibited columns
				static_assert(not ::sqlpp::detail::or_t<must_not_insert_t, Columns...>::value, "at least one column argument has a must_not_insert flag in its definition");

				std::tuple<simple_column_t<Columns>...> _columns;
			};

		template<typename Context, typename... Columns>
			struct interpreter_t<Context, column_list_t<Columns...>>
			{
				using T = column_list_t<Columns...>;

				static Context& _(const T& t, Context& context)
				{
					context << " (";
					interpret_tuple(t._columns, ",", context);
					context << ")";

					return context;
				}
			};
	}
}

#endif

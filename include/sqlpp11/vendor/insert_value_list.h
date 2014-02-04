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

#ifndef SQLPP_INSERT_VALUE_LIST_H
#define SQLPP_INSERT_VALUE_LIST_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/logic.h>
#include <sqlpp11/vendor/insert_value.h>
#include <sqlpp11/vendor/interpret_tuple.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename... InsertValues>
			struct insert_value_list_t
			{
				using _is_insert_value_list = std::true_type;

				static_assert(sizeof...(InsertValues), "at least one insert value required");

				// check for invalid arguments
				static_assert(::sqlpp::detail::and_t<is_insert_value_t, InsertValues...>::value, "at least one argument is not an insert value");

				using _value_tuple_t = std::tuple<InsertValues...>;

				void add(_value_tuple_t value_tuple)
				{
					_insert_values.emplace_back(value_tuple);
				}

				bool empty() const
				{
					return _insert_values.empty();
				}

				std::vector<_value_tuple_t> _insert_values;
			};

		template<>
			struct insert_value_list_t<noop>
			{
				using _is_insert_value_list = std::true_type;

				using _value_tuple_t = std::tuple<>;

				void add(_value_tuple_t value_tuple)
				{
				}

				static constexpr bool empty()
				{
					return true;
				}
			};

		template<typename Context, typename... InsertValues>
			struct interpreter_t<Context, insert_value_list_t<InsertValues...>>
			{
				using T = insert_value_list_t<InsertValues...>;

				static Context& _(const T& t, Context& context)
				{
					context << " VALUES ";
					bool first = true;
					for (const auto& row : t._insert_values)
					{
						if (not first)
							context << ',';
						else
							first = false;
						context << '(';
						interpret_tuple(row, ",", context);
						context << ')';
					}
					return context;
				}
			};

		template<typename Context>
			struct interpreter_t<Context, insert_value_list_t<noop>>
			{
				using T = insert_value_list_t<noop>;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};
	}
}

#endif

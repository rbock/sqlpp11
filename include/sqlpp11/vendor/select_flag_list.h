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

#ifndef SQLPP_VENDOR_SELECT_FLAG_LIST_H
#define SQLPP_VENDOR_SELECT_FLAG_LIST_H

#include <sqlpp11/select_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <tuple>

namespace sqlpp
{
	namespace vendor
	{
		template<typename Database, typename T>
			struct select_flag_list_t
			{
				static_assert(::sqlpp::vendor::wrong_t<T>::value, "invalid argument for select_flag_list");
			};

		// select_flag_list_t
		template<typename Database, typename... Flag>
			struct select_flag_list_t<Database, std::tuple<Flag...>>
			{
				using _is_select_flag_list = std::true_type; 
				using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
				using _parameter_tuple_t = std::tuple<Flag...>;
				using size = std::tuple_size<_parameter_tuple_t>;

				// check for duplicate order expressions
				static_assert(not ::sqlpp::detail::has_duplicates<Flag...>::value, "at least one duplicate argument detected in select flag list");

				// check for invalid order expressions
				static_assert(::sqlpp::detail::and_t<is_select_flag_t, Flag...>::value, "at least one argument is not a select flag in select flag list");

				template<typename E>
					void add(E expr)
					{
						static_assert(is_select_flag_t<E>::value, "flag arguments require to be select flags");
						_dynamic_flags.emplace_back(expr);
					}

				_parameter_tuple_t _flags;
				vendor::interpretable_list_t<Database> _dynamic_flags;
			};

		template<typename Context, typename Database, typename... Flag>
			struct interpreter_t<Context, select_flag_list_t<Database, std::tuple<Flag...>>>
			{
				using T = select_flag_list_t<Database, std::tuple<Flag...>>;

				static Context& _(const T& t, Context& context)
				{
					interpret_tuple(t._flags, ' ', context);
					if (sizeof...(Flag))
						context << ' ';
					interpret_list(t._dynamic_flags, ',', context);
					if (not t._dynamic_flags.empty())
						context << ' ';
					return context;
				}
			};
	}

}

#endif

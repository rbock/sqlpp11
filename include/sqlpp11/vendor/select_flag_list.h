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

#ifndef SQLPP_SELECT_FLAGS_H
#define SQLPP_SELECT_FLAGS_H

#include <sqlpp11/select_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/set.h>
#include <sqlpp11/vendor/interpret_tuple.h>
#include <tuple>

namespace sqlpp
{
	// standard select flags
	struct all_t
	{
		struct _value_type 
		{ 
			using _is_select_flag = std::true_type; 
		};
	};
	static constexpr all_t all = {};

	template<typename Context>
		struct vendor::interpreter_t<Context, all_t>
		{
			static Context& _(const all_t&, Context& context)
			{
				context << "ALL";
				return context;
			}
		};

	struct distinct_t
	{
		struct _value_type 
		{ 
			using _is_select_flag = std::true_type; 
		};
	};
	static constexpr distinct_t distinct = {};

	template<typename Context>
		struct vendor::interpreter_t<Context, distinct_t>
		{
			static Context& _(const distinct_t&, Context& context)
			{
				context << "DISTINCT";
				return context;
			}
		};

	struct straight_join_t
	{
		struct _value_type 
		{ 
			using _is_select_flag = std::true_type; 
		};
	};
	static constexpr straight_join_t straight_join = {};

	template<typename Context>
		struct vendor::interpreter_t<Context, straight_join_t>
		{
			static Context& _(const straight_join_t&, Context& context)
			{
				context << "STRAIGHT_JOIN";
				return context;
			}
		};

	namespace vendor
	{
		template<typename T>
			struct select_flag_list_t
			{
				static_assert(detail::wrong<T>::value, "invalid argument for select_flag_list");
			};

		// select_flag_list_t
		template<typename... Flag>
			struct select_flag_list_t<std::tuple<Flag...>>
			{
				// check for duplicate order expressions
				static_assert(not detail::has_duplicates<Flag...>::value, "at least one duplicate argument detected in select flag list");

				// check for invalid order expressions
				using _valid_flags = typename detail::make_set_if<is_select_flag_t, Flag...>::type;
				static_assert(_valid_flags::size::value == sizeof...(Flag), "at least one argument is not a select flag in select flag list");

				using _is_select_flag_list = std::true_type; 

				std::tuple<Flag...> _flags;
			};

		template<typename Context, typename... Flag>
			struct interpreter_t<Context, select_flag_list_t<std::tuple<Flag...>>>
			{
				using T = select_flag_list_t<std::tuple<Flag...>>;

				static Context& _(const T& t, Context& context)
				{
					interpret_tuple(t._flags, ' ', context);
					if (sizeof...(Flag))
						context << ' ';
					return context;
				}
			};
	}

}

#endif

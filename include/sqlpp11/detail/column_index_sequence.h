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

#ifndef SQLPP_DETAIL_COLUMN_INDEX_SEQUENCE_H
#define SQLPP_DETAIL_COLUMN_INDEX_SEQUENCE_H

#include <type_traits>

namespace sqlpp
{
	namespace detail
	{
		template<std::size_t LastIndex, std::size_t... Ints>
			struct column_index_sequence
			{
				static constexpr std::size_t last_index = LastIndex;
			};

		template<typename T, typename... Fields>
			struct make_column_index_sequence_impl;

		template<std::size_t LastIndex, std::size_t... Ints, typename Column, typename... Rest>
			struct make_column_index_sequence_impl<column_index_sequence<LastIndex, Ints...>, Column, Rest...>
			{
				using type = typename make_column_index_sequence_impl<column_index_sequence<LastIndex + 1, Ints..., LastIndex + 1>, Rest...>::type;
			};

		template<std::size_t LastIndex, std::size_t... Ints, typename AliasProvider, typename... Fields, typename... Rest>
			struct make_column_index_sequence_impl<column_index_sequence<LastIndex, Ints...>, multi_field_t<AliasProvider, Fields...>, Rest...>
			{
				using type = typename make_column_index_sequence_impl<column_index_sequence<LastIndex + sizeof...(Fields), Ints..., LastIndex + sizeof...(Fields)>, Rest...>::type;
			};

		template<std::size_t LastIndex, std::size_t... Ints>
			struct make_column_index_sequence_impl<column_index_sequence<LastIndex, Ints...>>
			{
				using type = column_index_sequence<LastIndex, Ints...>;
			};

		template<std::size_t StartIndex, typename... Fields>
			using make_column_index_sequence = typename make_column_index_sequence_impl<column_index_sequence<StartIndex - 1>, Fields...>::type;

	}
}


#endif

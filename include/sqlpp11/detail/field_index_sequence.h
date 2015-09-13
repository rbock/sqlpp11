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

#ifndef SQLPP_DETAIL_FIELD_INDEX_SEQUENCE_H
#define SQLPP_DETAIL_FIELD_INDEX_SEQUENCE_H

#include <type_traits>
#include <sqlpp11/field_spec.h>

namespace sqlpp
{
  namespace detail
  {
    template <std::size_t NextIndex, std::size_t... Ints>
    struct field_index_sequence
    {
      static constexpr std::size_t _next_index = NextIndex;
    };

    template <typename T, typename... Fields>
    struct make_field_index_sequence_impl
    {
      static_assert(wrong_t<make_field_index_sequence_impl>::value, "invalid field index sequence arguments");
    };

    template <std::size_t NextIndex,
              std::size_t... Ints,
              typename NameType,
              typename ValueType,
              bool CanBeNull,
              bool NullIsTrivialValue,
              typename... Rest>
    struct make_field_index_sequence_impl<field_index_sequence<NextIndex, Ints...>,
                                          field_spec_t<NameType, ValueType, CanBeNull, NullIsTrivialValue>,
                                          Rest...>
    {
      using type = typename make_field_index_sequence_impl<field_index_sequence<NextIndex + 1, Ints..., NextIndex>,
                                                           Rest...>::type;
    };

    template <std::size_t NextIndex, std::size_t... Ints, typename AliasProvider, typename FieldTuple, typename... Rest>
    struct make_field_index_sequence_impl<field_index_sequence<NextIndex, Ints...>,
                                          multi_field_spec_t<AliasProvider, FieldTuple>,
                                          Rest...>
    {
      using type = typename make_field_index_sequence_impl<
          field_index_sequence<NextIndex + std::tuple_size<FieldTuple>::value, Ints..., NextIndex>,
          Rest...>::type;
    };

    template <std::size_t NextIndex, std::size_t... Ints>
    struct make_field_index_sequence_impl<field_index_sequence<NextIndex, Ints...>>
    {
      using type = field_index_sequence<NextIndex, Ints...>;
    };

    template <std::size_t StartIndex, typename... Fields>
    using make_field_index_sequence =
        typename make_field_index_sequence_impl<field_index_sequence<StartIndex>, Fields...>::type;
  }
}

#endif

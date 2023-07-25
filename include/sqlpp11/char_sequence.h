#pragma once

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

#include <type_traits>
#include <sqlpp11/detail/index_sequence.h>

namespace sqlpp
{
  template <typename Context>
  std::integral_constant<char, '"'> get_quote_left(const Context&);

  template <typename Context>
  std::integral_constant<char, '"'> get_quote_right(const Context&);

  template <char... Cs>
  struct char_sequence
  {
    template <typename Context>
    static const char* char_ptr()
    {
      static char s[] = {Cs..., '\0'};
      return s;
    }
  };

  template <char... Cs>
  struct char_sequence<'!', Cs...>
  {
    template <typename Context>
    static const char* char_ptr()
    {
      static char s[] = {decltype(get_quote_left(std::declval<Context>()))::value, Cs...,
                         decltype(get_quote_right(std::declval<Context>()))::value, '\0'};
      return s;
    }
  };

  template <std::size_t N, const char* s, typename T>
  struct make_char_sequence_impl;

  template <std::size_t N, const char* s, std::size_t... i>
  struct make_char_sequence_impl<N, s, sqlpp::detail::index_sequence<i...>>
  {
    using type = char_sequence<s[i]...>;
  };

  template <std::size_t N, const char* Input>
  using make_char_sequence =
      typename make_char_sequence_impl<N, Input, sqlpp::detail::make_index_sequence<N - 1>>::type;

}  // namespace sqlpp

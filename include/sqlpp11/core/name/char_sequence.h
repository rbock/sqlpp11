#pragma once

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

#include <type_traits>
#include <sqlpp11/core/compat/utility.h>

namespace sqlpp
{
  template <char... Cs>
  struct char_sequence
  {
  };

  template <std::size_t N, const char* s, typename T>
  struct make_char_sequence_impl;

  template <std::size_t N, const char* s, std::size_t... i>
  struct make_char_sequence_impl<N, s, ::sqlpp::index_sequence<i...>>
  {
    using type = char_sequence<s[i]...>;
  };

  template <typename T>
  struct make_char_sequence
      : make_char_sequence_impl<sizeof(sqlpp::name_tag_of_t<T>::name), sqlpp::name_tag_of_t<T>::name, ::sqlpp::make_index_sequence<sizeof(sqlpp::name_tag_of_t<T>::name) - 1>>
  {
  };

  template <typename T>
  using make_char_sequence_t = typename make_char_sequence<T>::type;

}  // namespace sqlpp

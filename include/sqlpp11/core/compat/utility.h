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

#include <utility>

#ifdef _MSVC_LANG
#define CXX_STD_VER _MSVC_LANG
#else
#define CXX_STD_VER __cplusplus
#endif

#if CXX_STD_VER >= 201402L
#include <string_view>
namespace sqlpp
{
  template <std::size_t... Ints>
  using index_sequence = std::index_sequence<Ints...>;
  template <std::size_t N>
  using make_index_sequence = std::make_index_sequence<N>;

}  // namespace sqlpp

#else // incomplete backport of utility.h

namespace sqlpp
{
  // index_sequence & make_index_sequence
  template <std::size_t... Ints>
  class index_sequence
  {
  };

  template <typename T, std::size_t N>
  struct make_index_sequence_impl;

  template <std::size_t N, std::size_t... Ints>
  struct make_index_sequence_impl<index_sequence<Ints...>, N>
  {
    using type = typename make_index_sequence_impl<index_sequence<Ints..., sizeof...(Ints)>, N - 1>::type;
  };

  template <std::size_t... Ints>
  struct make_index_sequence_impl<index_sequence<Ints...>, 0>
  {
    using type = index_sequence<Ints...>;
  };

  template <std::size_t N>
  using make_index_sequence = typename make_index_sequence_impl<index_sequence<>, N>::type;
}  // namespace sqlpp

#endif

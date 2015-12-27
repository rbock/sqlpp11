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

#ifndef SQLPP_DETAIL_LOGIC_H
#define SQLPP_DETAIL_LOGIC_H

#include <ciso646>  // Required for some compilers to use aliases for boolean operators
#include <type_traits>

namespace sqlpp
{
  namespace logic
  {
    template <bool... B>
    struct logic_helper;

    // see http://lists.boost.org/Archives/boost/2014/05/212946.php :-)

    // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2086629
    template <bool... B>
    struct all
    {
      using type = std::is_same<logic_helper<B...>, logic_helper<(B or true)...>>;
    };

    template <bool... B>
    using all_t = std::is_same<logic_helper<B...>, logic_helper<(B or true)...>>;

    // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2086629
    template <bool... B>
    struct any
    {
      using type =
          std::integral_constant<bool, not std::is_same<logic_helper<B...>, logic_helper<(B and false)...>>::value>;
    };

    template <bool... B>
    using any_t =
        std::integral_constant<bool, not std::is_same<logic_helper<B...>, logic_helper<(B and false)...>>::value>;

    template <bool... B>
    using none_t = std::is_same<logic_helper<B...>, logic_helper<(B and false)...>>;

    template <bool>
    struct not_impl;

    template <>
    struct not_impl<true>
    {
      using type = std::false_type;
    };

    template <>
    struct not_impl<false>
    {
      using type = std::true_type;
    };

    template <template <typename> class Predicate, typename... T>
    using not_t = typename not_impl<Predicate<T>::value...>::type;

    template <typename T>
    using identity_t = T;
  }
}

#endif

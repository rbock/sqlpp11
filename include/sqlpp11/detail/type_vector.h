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

#ifndef SQLPP_DETAIL_TYPE_VECTOR_H
#define SQLPP_DETAIL_TYPE_VECTOR_H

#include <sqlpp11/wrong.h>

namespace sqlpp
{
  namespace detail
  {
    template <typename... T>
    struct type_vector
    {
    };

    template <typename... T>
    struct type_vector_cat_impl
    {
      static_assert(wrong_t<type_vector_cat_impl>::value, "type_vector_cat must be called with type_vector arguments");
    };

    template <>
    struct type_vector_cat_impl<>
    {
      using type = type_vector<>;
    };

    template <typename... T>
    struct type_vector_cat_impl<type_vector<T...>>
    {
      using type = type_vector<T...>;
    };

    template <typename... L, typename... R>
    struct type_vector_cat_impl<type_vector<L...>, type_vector<R...>>
    {
      using type = type_vector<L..., R...>;
    };

    template <typename... L, typename... Rest>
    struct type_vector_cat_impl<type_vector<L...>, Rest...>
    {
      using type = typename type_vector_cat_impl<type_vector<L...>, typename type_vector_cat_impl<Rest...>::type>::type;
    };

    template <typename... T>
    using type_vector_cat_t = typename type_vector_cat_impl<T...>::type;

    template <typename T>
    struct type_vector_size
    {
      static_assert(wrong_t<type_vector_size>::value,
                    "type_vector_size needs to be called with a type_vector argument");
    };

    template <typename... T>
    struct type_vector_size<type_vector<T...>>
    {
      static constexpr std::size_t value = sizeof...(T);
    };
  }
}

#endif

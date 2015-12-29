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

#ifndef SQLPP_DETAIL_COPY_TUPLE_ARGS_H
#define SQLPP_DETAIL_COPY_TUPLE_ARGS_H

#include <tuple>
#include <sqlpp11/auto_alias.h>

namespace sqlpp
{
  template <typename Table>
  struct all_of_t;

  namespace detail
  {
    template <typename T>
    struct as_column_tuple
    {
      static std::tuple<auto_alias_t<T>> _(T t)
      {
        return std::tuple<auto_alias_t<T>>(auto_alias_t<T>{t});
      }
    };

    template <typename T>
    struct as_column_tuple<all_of_t<T>>
    {
      static typename all_of_t<T>::_column_tuple_t _(all_of_t<T>)
      {
        return {};
      }
    };

    template <typename... Args>
    struct as_column_tuple<std::tuple<Args...>>
    {
      static std::tuple<auto_alias_t<Args>...> _(std::tuple<Args...> t)
      {
        return t;
      }
    };

    template <template <typename, typename...> class Target, typename First, typename T>
    struct copy_tuple_args_impl
    {
      static_assert(wrong_t<copy_tuple_args_impl>::value, "copy_tuple_args must be called with a tuple");
    };

    template <template <typename First, typename...> class Target, typename First, typename... Args>
    struct copy_tuple_args_impl<Target, First, std::tuple<Args...>>
    {
      using type = Target<First, Args...>;
    };

    template <template <typename First, typename...> class Target, typename First, typename T>
    using copy_tuple_args_t = typename copy_tuple_args_impl<Target, First, T>::type;

    template <typename... Columns>
    auto column_tuple_merge(Columns... columns) -> decltype(std::tuple_cat(as_column_tuple<Columns>::_(columns)...))
    {
      return std::tuple_cat(as_column_tuple<Columns>::_(columns)...);
    }
  }
}

#endif

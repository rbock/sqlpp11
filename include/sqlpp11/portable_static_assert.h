/*
 * Copyright (c) 2015-2016, Roland Bock
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

#ifndef SQLPP_PORTABLE_STATIC_ASSERT_H
#define SQLPP_PORTABLE_STATIC_ASSERT_H

#include <sqlpp11/consistent.h>
#include <sqlpp11/inconsistent.h>

namespace sqlpp
{
#define SQLPP_PORTABLE_STATIC_ASSERT(name, message) \
  struct name : std::false_type                     \
  {                                                 \
    template <typename... T>                        \
    name(T&&...)                                    \
    {                                               \
      static_assert(wrong_t<T...>::value, message); \
    }                                               \
    auto begin() const -> void;                     \
    auto end() const -> void;                       \
  }

  namespace detail
  {
    template <bool Consistent, typename Assert>
    struct static_check_impl
    {
      using type = Assert;
    };

    template <typename Assert>
    struct static_check_impl<true, Assert>
    {
      using type = consistent_t;
    };
  }

  template <bool Consistent, typename Assert>
  using static_check_t = typename detail::static_check_impl<Consistent, Assert>::type;

  namespace detail
  {
    template <typename... StaticChecks>
    struct static_combined_check_impl;

    template <typename StaticCheck, typename... Rest>
    struct static_combined_check_impl<StaticCheck, Rest...>
    {
      using type = StaticCheck;
    };

    template <typename... Rest>
    struct static_combined_check_impl<consistent_t, Rest...>
    {
      using type = typename static_combined_check_impl<Rest...>::type;
    };

    template <>
    struct static_combined_check_impl<>
    {
      using type = consistent_t;
    };
  }

  template <typename... StaticChecks>
  using static_combined_check_t = typename detail::static_combined_check_impl<StaticChecks...>::type;
}

#endif

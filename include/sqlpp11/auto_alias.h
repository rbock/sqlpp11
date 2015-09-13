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

#ifndef SQLPP_AUTO_ALIAS_H
#define SQLPP_AUTO_ALIAS_H

#include <sqlpp11/alias.h>

namespace sqlpp
{
  template <typename T, typename Enable = void>
  struct has_auto_alias_t
  {
    static constexpr bool value = false;
  };

  template <typename T>
  struct has_auto_alias_t<T, typename std::enable_if<not wrong_t<typename T::_auto_alias_t>::value>::type>
  {
    static constexpr bool value = true;
  };

  namespace detail
  {
    template <typename T, typename Enable = void>
    struct auto_alias_impl
    {
      using type = T;
    };

    template <typename T>
    struct auto_alias_impl<T, typename std::enable_if<has_auto_alias_t<T>::value>::type>
    {
      using type = expression_alias_t<T, typename T::_auto_alias_t>;
    };
  }

  template <typename T>
  using auto_alias_t = typename detail::auto_alias_impl<T>::type;
}

#endif

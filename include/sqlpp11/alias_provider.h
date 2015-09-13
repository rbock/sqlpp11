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

#ifndef SQLPP_ALIAS_PROVIDER_H
#define SQLPP_ALIAS_PROVIDER_H

#include <type_traits>
#include <sqlpp11/char_sequence.h>

#define SQLPP_ALIAS_PROVIDER(name)                                           \
  struct name##_t                                                            \
  {                                                                          \
    struct _alias_t                                                          \
    {                                                                        \
      static constexpr const char _literal[] = #name;                        \
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>; \
      template <typename T>                                                  \
      struct _member_t                                                       \
      {                                                                      \
        T name;                                                              \
        T& operator()()                                                      \
        {                                                                    \
          return name;                                                       \
        }                                                                    \
        const T& operator()() const                                          \
        {                                                                    \
          return name;                                                       \
        }                                                                    \
      };                                                                     \
    };                                                                       \
  };                                                                         \
  constexpr name##_t name = {};

namespace sqlpp
{
  template <typename T, typename Enable = void>
  struct is_alias_provider_t
  {
    static constexpr bool value = false;
  };

  template <typename T>
  struct is_alias_provider_t<
      T,
      typename std::enable_if<std::is_class<typename T::_alias_t::template _member_t<int>>::value, void>::type>
  {
    static constexpr bool value = true;
  };

  namespace alias
  {
    SQLPP_ALIAS_PROVIDER(a)
    SQLPP_ALIAS_PROVIDER(b)
    SQLPP_ALIAS_PROVIDER(c)
    SQLPP_ALIAS_PROVIDER(d)
    SQLPP_ALIAS_PROVIDER(e)
    SQLPP_ALIAS_PROVIDER(f)
    SQLPP_ALIAS_PROVIDER(g)
    SQLPP_ALIAS_PROVIDER(h)
    SQLPP_ALIAS_PROVIDER(i)
    SQLPP_ALIAS_PROVIDER(j)
    SQLPP_ALIAS_PROVIDER(k)
    SQLPP_ALIAS_PROVIDER(l)
    SQLPP_ALIAS_PROVIDER(m)
    SQLPP_ALIAS_PROVIDER(n)
    SQLPP_ALIAS_PROVIDER(o)
    SQLPP_ALIAS_PROVIDER(p)
    SQLPP_ALIAS_PROVIDER(q)
    SQLPP_ALIAS_PROVIDER(s)
    SQLPP_ALIAS_PROVIDER(t)
    SQLPP_ALIAS_PROVIDER(u)
    SQLPP_ALIAS_PROVIDER(v)
    SQLPP_ALIAS_PROVIDER(w)
    SQLPP_ALIAS_PROVIDER(x)
    SQLPP_ALIAS_PROVIDER(y)
    SQLPP_ALIAS_PROVIDER(z)
    SQLPP_ALIAS_PROVIDER(left)
    SQLPP_ALIAS_PROVIDER(right)
  }
}

#endif

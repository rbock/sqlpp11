#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#ifdef _MSVC_LANG
#define CXX_STD_VER _MSVC_LANG
#else
#define CXX_STD_VER __cplusplus
#endif

#if CXX_STD_VER >= 201402L
namespace sqlpp
{
  using ::std::enable_if_t;
}  // namespace sqlpp

#else

namespace sqlpp
{
  template <bool Condition, typename Type = void>
  using enable_if_t = typename ::std::enable_if<Condition, Type>::type;
}  // namespace sqlpp

#endif

#if CXX_STD_VER >= 201703L
namespace sqlpp
{
  using std::void_t;
}  // namespace sqlpp

#else

namespace sqlpp
{
  // See https://en.cppreference.com/w/cpp/types/void_t:
  // "Until the resolution of CWG issue 1558 (a C++11 defect), unused parameters in alias templates were not guaranteed
  // to ensure SFINAE and could be ignored [...].
  namespace detail
  {
    template <typename... T>
    struct void_impl
    {
      using type = void;
    };
  }  // namespace detail

  template <typename... T>
  using void_t = typename detail::void_impl<T...>::type;
}  // namespace sqlpp

#endif

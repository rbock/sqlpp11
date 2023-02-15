/*
 * Copyright (c) 2023, Roland Bock
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

#ifndef SQLPP11_DETAIL_REMOVE_CVREF_H
#define SQLPP11_DETAIL_REMOVE_CVREF_H

#include <type_traits>

namespace sqlpp
{
  namespace detail
  {
    template <typename T>
    using remove_reference_t = typename std::remove_reference<T>::type;
    template <typename T>
    using remove_cv_t = typename std::remove_cv<T>::type;
    template <typename T>
    using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;

    static_assert(std::is_same<remove_reference_t<int const&>, int const>{}, "");
    static_assert(std::is_same<remove_reference_t<int&&>, int>{}, "");
    static_assert(std::is_same<remove_reference_t<int&>, int>{}, "");
    static_assert(std::is_same<remove_reference_t<int>, int>{}, "");

    static_assert(std::is_same<remove_cv_t<int const>, int>{}, "");
    static_assert(std::is_same<remove_cv_t<int volatile>, int>{}, "");
    static_assert(std::is_same<remove_cv_t<int* const>, int*>{}, "");
    static_assert(std::is_same<remove_cv_t<int>, int>{}, "");

    static_assert(std::is_same<remove_cvref_t<int const&&>, int>{}, "");
    static_assert(std::is_same<remove_cvref_t<int const&>, int>{}, "");
    static_assert(std::is_same<remove_cvref_t<int volatile&>, int>{}, "");
    static_assert(std::is_same<remove_cvref_t<int&&>, int>{}, "");
    static_assert(std::is_same<remove_cvref_t<int&>, int>{}, "");
    static_assert(std::is_same<remove_cvref_t<int* const&&>, int*>{}, "");
    static_assert(std::is_same<remove_cvref_t<int* const&>, int*>{}, "");
    static_assert(std::is_same<remove_cvref_t<int>, int>{}, "");
  }  // namespace detail
}  // namespace sqlpp

#endif

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

#ifndef SQLPP_IS_REGULAR_H
#define SQLPP_IS_REGULAR_H

#include <type_traits>

namespace sqlpp
{
  template <typename T>
  struct is_regular
  {
#if defined __clang__
#if __has_feature(cxx_thread_local)
#define SQLPP_TEST_NO_THROW_MOVE_CONSTRUCTIBLE  // clang 3.2 has a problem with nothrow_constructibility (it also does
                                                // not have thread_local support)
#endif
#else
#define SQLPP_TEST_NO_THROW_MOVE_CONSTRUCTIBLE
#endif

    static constexpr bool value =
        true
#if !defined _MSC_VER
#if defined SQLPP_TEST_NO_THROW_MOVE_CONSTRUCTIBLE
        and std::is_nothrow_move_constructible<T>::value
#endif
        and std::is_move_assignable<T>::value  // containers and strings are not noexcept_assignable
        and std::is_copy_constructible<T>::value and std::is_copy_assignable<T>::value
// default constructor makes no sense
// (not) equals would be possible
// not sure about less
#endif
        ;
  };
}

#endif

#pragma once

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

#include <limits>
#include <sstream>
#include <type_traits>
#include <utility>

#include <sqlpp11/detail/enable_if.h>

namespace sqlpp
{
  namespace detail
  {
    template <typename T, typename = void>
    struct float_safe_ostringstream_implementation
    {
      template <typename U>
      void operator()(std::ostringstream& os, U&& x) const
      {
        os << std::forward<U>(x);
      }
    };

    template <typename T>
    struct float_safe_ostringstream_implementation<T, enable_if_t<std::is_floating_point<T>::value>>
    {
      template <typename U>
      void operator()(std::ostringstream& os, U&& x) const
      {
        auto const old_precision = os.precision(std::numeric_limits<T>::max_digits10);
        os << std::forward<U>(x);
        os.precision(old_precision);
      }
    };

    struct float_safe_ostringstream : std::ostringstream
    {
      template <typename T>
      friend float_safe_ostringstream& operator<<(float_safe_ostringstream& os, T&& x)
      {
        float_safe_ostringstream_implementation<typename std::decay<T>::type>{}(os, x);
        return os;
      }
    };
  }  // namespace detail
}  // namespace sqlpp

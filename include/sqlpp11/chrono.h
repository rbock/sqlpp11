/*
 * Copyright (c) 2015-2016, Roland Bock, Aaron Bishop
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

#ifndef SQLPP_CHRONO_H
#define SQLPP_CHRONO_H

#include <date.h>

namespace sqlpp
{
  namespace chrono
  {
    using days = std::chrono::duration<int, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>;

    using day_point = std::chrono::time_point<std::chrono::system_clock, days>;
    using microsecond_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;

#if _MSC_FULL_VER >= 190023918
    // MSVC Update 2 provides floor, ceil, round, abs in chrono (which is C++17 only...)
    using ::std::chrono::floor;
#else
    using ::date::floor;
#endif

    template <typename T>
    std::chrono::microseconds time_of_day(T t)
    {
      const auto dp = floor<days>(t);
      return std::chrono::duration_cast<std::chrono::microseconds>(::date::make_time(t - dp).to_duration());
    }
  }
}

#endif

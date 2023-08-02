#pragma once

/**
 * Copyright © 2014-2015, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/chrono.h>
#include <sqlpp11/parameter.h>
#include <sqlpp11/wrap_operand.h>

namespace sqlpp
{
  // Serialize parameters
  template <typename ValueType, typename NameType>
  postgresql::context_t& serialize(const parameter_t<ValueType, NameType>&, postgresql::context_t& context)
  {
      context << "$" << context.count();
      context.pop_count();
      return context;
  }

  inline postgresql::context_t& serialize(const blob_operand& t, postgresql::context_t& context)
  {
    constexpr char hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    context << "'\\x";
    for (const auto c : t._t)
    {
      context << hex_chars[c >> 4] << hex_chars[c & 0x0F];
    }
    context << '\'';

    return context;
  }

  template <typename Period>
  postgresql::context_t& serialize(const time_point_operand<Period>& t, postgresql::context_t& context)
  {
    const auto dp = ::sqlpp::chrono::floor<::date::days>(t._t);
    const auto time = ::date::make_time(t._t - dp);
    const auto ymd = ::date::year_month_day{dp};
    context << "TIMESTAMP WITH TIME ZONE '" << ymd << ' ' << time << "+00'";
    return context;
  }

  template <typename Period>
  postgresql::context_t& serialize(const time_of_day_operand<Period>& t, postgresql::context_t& context)
  {
    context << "TIME WITH TIME ZONE '" << ::date::make_time(t._t) << "+00'";
    return context;
  }
}

#pragma once

/*
 * Copyright (c) 2013-2017, Roland Bock
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

#include <vector>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/alias_operators.h>

namespace sqlpp
{
  struct blob;

  struct blob_operand : public alias_operators<blob_operand>
  {
    using _traits = make_traits<blob, tag::is_expression, tag::is_wrapped_value>;
    using _nodes = detail::type_vector<>;
    using _is_literal_expression = std::true_type;

    using _value_t = std::vector<std::uint8_t>;

    blob_operand() : _t{}
    {
    }

    blob_operand(_value_t t) : _t(t)
    {
    }

    template <std::size_t N>
    blob_operand(const std::array<uint8_t, N>& t) : _t(t.begin(), t.end())
    {
    }

    blob_operand(const blob_operand&) = default;
    blob_operand(blob_operand&&) = default;
    blob_operand& operator=(const blob_operand&) = default;
    blob_operand& operator=(blob_operand&&) = default;
    ~blob_operand() = default;

    _value_t _t;
  };

  template <typename Context>
  Context& serialize(const blob_operand& t, Context& context)
  {
    constexpr char hexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    context << "x'";
    for (const auto c : t._t)
    {
      context << hexChars[c >> 4] << hexChars[c & 0x0F];
    }
    context << '\'';

    return context;
  }
}  // namespace sqlpp

#pragma once

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

#include <string>
#include <utility>
#if __cplusplus >= 201703L
#include <string_view>
#endif
#include <sqlpp11/type_traits.h>
#include <sqlpp11/alias_operators.h>

namespace sqlpp
{
  struct text;

  struct text_operand : public alias_operators<text_operand>
  {
    using _traits = make_traits<text, tag::is_expression, tag::is_wrapped_value>;
    using _nodes = detail::type_vector<>;
    using _is_literal_expression = std::true_type;

    using _value_t = std::string;

    text_operand() = default;

    text_operand(_value_t t) : _t(std::move(t))
    {
    }
#if __cplusplus >= 201703L
    // allow construction from an std::string_view
    text_operand(std::string_view t) : _t(t)
    {
    }
    // additional const char* overload, required to disambiguate
    text_operand(const char* t) : _t(t)
    {
    }
#endif

    text_operand(const text_operand&) = default;
    text_operand(text_operand&&) = default;
    text_operand& operator=(const text_operand&) = default;
    text_operand& operator=(text_operand&&) = default;
    ~text_operand() = default;

    _value_t _t;
  };

  template <typename Context>
  Context& serialize(const text_operand& t, Context& context)
  {
    context << '\'' << context.escape(t._t) << '\'';
    return context;
  }
}  // namespace sqlpp

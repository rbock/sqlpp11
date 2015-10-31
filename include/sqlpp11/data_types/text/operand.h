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

#ifndef SQLPP_TEXT_OPERAND_H
#define SQLPP_TEXT_OPERAND_H

#include <string>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/alias_operators.h>
#include <sqlpp11/serializer.h>

namespace sqlpp
{
  struct text;

  struct text_operand : public alias_operators<text_operand>
  {
    using _traits = make_traits<text, tag::is_expression, tag::is_wrapped_value>;
    using _nodes = detail::type_vector<>;
    using _is_aggregate_expression = std::true_type;

    using _value_t = std::string;

    text_operand() : _t{}
    {
    }

    text_operand(_value_t t) : _t(t)
    {
    }

    text_operand(const text_operand&) = default;
    text_operand(text_operand&&) = default;
    text_operand& operator=(const text_operand&) = default;
    text_operand& operator=(text_operand&&) = default;
    ~text_operand() = default;

    bool _is_trivial() const
    {
      return _t.empty();
    }

    _value_t _t;
  };

  template <typename Context>
  struct serializer_t<Context, text_operand>
  {
    using _serialize_check = consistent_t;
    using Operand = text_operand;

    static Context& _(const Operand& t, Context& context)
    {
      context << '\'' << context.escape(t._t) << '\'';
      return context;
    }
  };
}
#endif

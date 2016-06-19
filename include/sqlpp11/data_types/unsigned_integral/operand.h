/*
 * Copyright (c) 2013-2016, Roland Bock, Aaron Bishop
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

#ifndef SQLPP_UNSIGNED_INTEGRAL_OPERAND_H
#define SQLPP_UNSIGNED_INTEGRAL_OPERAND_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/alias_operators.h>
#include <sqlpp11/serializer.h>

namespace sqlpp
{
  struct unsigned_integral;

  struct unsigned_integral_operand : public alias_operators<unsigned_integral_operand>
  {
    using _traits = make_traits<unsigned_integral, tag::is_expression, tag::is_wrapped_value>;
    using _nodes = detail::type_vector<>;
    using _is_aggregate_expression = std::true_type;

    using _value_t = uint64_t;

    unsigned_integral_operand() : _t{}
    {
    }

    unsigned_integral_operand(_value_t t) : _t(t)
    {
    }

    unsigned_integral_operand(const unsigned_integral_operand&) = default;
    unsigned_integral_operand(unsigned_integral_operand&&) = default;
    unsigned_integral_operand& operator=(const unsigned_integral_operand&) = default;
    unsigned_integral_operand& operator=(unsigned_integral_operand&&) = default;
    ~unsigned_integral_operand() = default;

    bool _is_trivial() const
    {
      return _t == 0;
    }

    _value_t _t;
  };

  template <typename Context>
  struct serializer_t<Context, unsigned_integral_operand>
  {
    using _serialize_check = consistent_t;
    using Operand = unsigned_integral_operand;

    static Context& _(const Operand& t, Context& context)
    {
      context << t._t;
      return context;
    }
  };
}

#endif

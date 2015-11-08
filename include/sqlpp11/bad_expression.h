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

#ifndef SQLPP_BAD_EXPRESSION_H
#define SQLPP_BAD_EXPRESSION_H

#include <sqlpp11/portable_static_assert.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  SQLPP_PORTABLE_STATIC_ASSERT(assert_valid_operands, "Invalid operand(s)");

  template <typename ValueType>
  struct bad_expression
  {
    template <typename... T>
    bad_expression(T&&...)
    {
    }
    using _traits = make_traits<ValueType, tag::is_expression>;
    using _nodes = detail::type_vector<>;
  };

  template <typename Context, typename ValueType>
  struct serializer_t<Context, bad_expression<ValueType>>
  {
    using _serialize_check = assert_valid_operands;
    using T = bad_expression<ValueType>;

    static Context& _(const T&, Context&);
  };
}

#endif

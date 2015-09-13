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

#ifndef SQLPP_BOOLEAN_EXPRESSION_H
#define SQLPP_BOOLEAN_EXPRESSION_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/interpretable.h>

namespace sqlpp
{
  template <typename Database>
  struct boolean_expression_t : public expression_operators<boolean_expression_t<Database>, boolean>
  {
    using _traits = make_traits<boolean, tag::is_expression>;
    using _nodes = detail::type_vector<>;

    template <typename Expr>
    boolean_expression_t(Expr expr)
        : _expr(expr)
    {
      static_assert(is_expression_t<Expr>::value, "boolean_expression requires a boolean expression argument");
      static_assert(is_boolean_t<Expr>::value, "boolean_expression requires a boolean expression argument");
    }

    boolean_expression_t(const boolean_expression_t&) = default;
    boolean_expression_t(boolean_expression_t&&) = default;
    boolean_expression_t& operator=(const boolean_expression_t&) = default;
    boolean_expression_t& operator=(boolean_expression_t&&) = default;
    ~boolean_expression_t() = default;

    interpretable_t<Database> _expr;
  };

  template <typename Database, typename T>
  boolean_expression_t<Database> boolean_expression(T t)
  {
    using Expr = wrap_operand_t<T>;
    return {Expr{t}};
  }

  template <typename Database, typename T>
  boolean_expression_t<Database> boolean_expression(const Database&, T t)
  {
    return boolean_expression<Database>(t);
  }

  template <typename Context, typename Database>
  struct serializer_t<Context, boolean_expression_t<Database>>
  {
    using _serialize_check = consistent_t;
    using T = boolean_expression_t<Database>;

    static Context& _(const T& t, Context& context)
    {
      return serialize(t._expr, context);
    }
  };
}

#endif

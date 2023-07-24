#pragma once

/*
 * Copyright (c) 2021, Roland Bock
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

#include <sqlpp11/value_or_null.h>

namespace sqlpp
{
  template <typename Expr, typename ValueType>
  struct is_equal_to_or_null_t: public expression_operators<is_equal_to_or_null_t<Expr, ValueType>, boolean>,
                     public alias_operators<is_equal_to_or_null_t<Expr, ValueType>>
  {
    using _traits = make_traits<boolean, tag::is_expression>;
    using _nodes = detail::type_vector<Expr, value_or_null_t<ValueType>>;

    is_equal_to_or_null_t(Expr expr, value_or_null_t<ValueType> value) : _expr(expr), _value(value)
    {
    }

    is_equal_to_or_null_t(const is_equal_to_or_null_t&) = default;
    is_equal_to_or_null_t(is_equal_to_or_null_t&&) = default;
    is_equal_to_or_null_t& operator=(const is_equal_to_or_null_t&) = default;
    is_equal_to_or_null_t& operator=(is_equal_to_or_null_t&&) = default;
    ~is_equal_to_or_null_t() = default;


    Expr _expr;
    value_or_null_t<ValueType> _value;
  };

  template <typename Context, typename Expr, typename ValueType>
  Context& serialize(const is_equal_to_or_null_t<Expr, ValueType>& t, Context& context)
  {
    if (t._value._is_null)
      serialize(t._expr.is_null(), context);
    else
      serialize(t._expr == t._value, context);

    return context;
  }

  template <typename Expr, typename ValueType>
  auto is_equal_to_or_null(Expr expr, value_or_null_t<ValueType> value) -> is_equal_to_or_null_t<Expr, ValueType>
  {
    static_assert(is_expression_t<Expr>::value,
                  "is_equal_to_or_null() is to be called an expression (e.g. a column) and a value_or_null expression");
    static_assert(std::is_same<value_type_of<Expr>, ValueType>::value,
                  "is_equal_to_or_null() arguments need to have the same value type");
    return {expr, value};
  }

}  // namespace sqlpp

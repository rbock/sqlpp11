#pragma once

/*
 * Copyright (c) 2013-2020, Roland Bock, MacDue
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

#include <sqlpp11/over.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/aggregate_function_operators.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  template <typename Flag, typename Expr>
  struct count_t
  {
    using _traits = make_traits<integral, tag::is_expression /*, tag::is_selectable*/>;

    using _nodes = detail::type_vector<Expr, aggregate_function>;
    using _can_be_null = std::false_type;
    using _is_aggregate_expression = std::true_type;

    count_t(const Expr expr) : _expr(expr)
    {
    }

    count_t(const count_t&) = default;
    count_t(count_t&&) = default;
    count_t& operator=(const count_t&) = default;
    count_t& operator=(count_t&&) = default;
    ~count_t() = default;

    Expr _expr;
  };

  template <typename Flag, typename Expr>
  struct value_type_of<count_t<Flag, Expr>>
  {
    using type = integral;
  };

  template <typename Context, typename Flag, typename Expr>
  Context& serialize(Context& context, const count_t<Flag, Expr>& t)
  {
    context << "COUNT(";
    if (std::is_same<distinct_t, Flag>::value)
    {
      serialize(context, Flag());
      context << ' ';
    }
    serialize_operand(context, t._expr);
    context << ")";
    return context;
  }

  template <typename T>
  using check_count_arg =
      std::enable_if_t<values_are_comparable<T, T>::value and not contains_aggregate_function_t<T>::value>;

  template <typename T, typename = check_count_arg<T>>
  auto count(T t) -> count_t<noop, T>
  {
    return {std::move(t)};
  }

  template <typename T, typename = check_count_arg<T>>
  auto count(const distinct_t& /*unused*/, T t) -> count_t<distinct_t, T>
  {
    return {std::move(t)};
  }

}  // namespace sqlpp

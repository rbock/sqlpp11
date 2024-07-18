#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/type_traits.h>
#include <sqlpp11/serialize.h>

namespace sqlpp
{
  template <typename Expr>
  struct dynamic_t
  {
    using _traits = make_traits<value_type_of_t<Expr>, tag::is_multi_expression>;
    using _nodes = detail::type_vector<Expr>;

    dynamic_t(bool condition, Expr expr) : _condition(condition), _expr(expr)
    {
    }

    dynamic_t(const dynamic_t&) = default;
    dynamic_t(dynamic_t&&) = default;
    dynamic_t& operator=(const dynamic_t&) = default;
    dynamic_t& operator=(dynamic_t&&) = default;
    ~dynamic_t() = default;

    bool _condition;
    Expr _expr;
  };

  // No value_type_of defined for dynamic_t, because it is to be used in very specific contexts in which _expr may be
  // used depending on the value of _condition.

  template <typename T>
  struct remove_dynamic
  {
    using type = T;
  };

  template <typename Expr>
  struct remove_dynamic<dynamic_t<Expr>>
  {
    using type = Expr;
  };

  template <typename T>
  using remove_dynamic_t = typename remove_dynamic<T>::type;

  template <typename T>
  struct dynamic_to_optional
  {
    using type = T;
  };

  template <typename Expr>
  struct dynamic_to_optional<dynamic_t<Expr>>
  {
    using type = force_optional_t<Expr>;
  };

  template <typename T>
  using dynamic_to_optional_t = typename dynamic_to_optional<T>::type;

  template <typename Context, typename Select>
  Context& serialize(const dynamic_t<Select>& t, Context& context)
  {
    if (t._condition)
    {
    serialize(t._expr, context);
    }
    else
    {
      context << NULL;
    }
    return context;
  }

  template <typename Expr>
  using check_dynamic_args = std::enable_if_t<has_value_type<Expr>::value>;

  template <typename Expr, typename = check_dynamic_args<Expr>>
  auto dynamic(bool condition, Expr t) -> dynamic_t<Expr>
  {
    return {condition, std::move(t)};
  }
}  // namespace sqlpp

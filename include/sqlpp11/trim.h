#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
 * Copyright (c) 2017, Juan Dent
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

#include <sqlpp11/serialize.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  template <typename Expr>
  struct trim_t
  {
    using _traits = make_traits<text, tag::is_expression, tag::is_selectable>;

    using _nodes = detail::type_vector<Expr>;

    trim_t(const Expr expr) : _expr(expr)
    {
    }

    trim_t(const trim_t&) = default;
    trim_t(trim_t&&) = default;
    trim_t& operator=(const trim_t&) = default;
    trim_t& operator=(trim_t&&) = default;
    ~trim_t() = default;

    Expr _expr;
  };

  template <typename Context, typename Expr>
  Context& serialize(Context& context, const trim_t<Expr>& t)
  {
    context << "TRIM(";
    serialize_operand(context, t._expr);
    context << ")";
    return context;
  }

  template<typename T>
    using check_trim_args = std::enable_if_t<is_text<T>::value>;

  template <typename T, typename = check_trim_args<T>>
  auto trim(T t) -> trim_t<T>
  {
    return {std::move(t)};
  }

}  // namespace sqlpp

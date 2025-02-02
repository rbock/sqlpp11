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

#include <sqlpp11/core/to_sql_string.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/operator/enable_as.h>

namespace sqlpp
{
  template <typename Expr>
  struct trim_t : public enable_as<trim_t<Expr>>
  {
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

  template<typename Expr>
    struct value_type_of<trim_t<Expr>> : public value_type_of<Expr> {};

  template<typename Expr>
    struct nodes_of<trim_t<Expr>>
    {
      using type = detail::type_vector<Expr>;
    };

  template <typename Context, typename Expr>
  auto to_sql_string(Context& context, const trim_t<Expr>& t) -> std::string
  {
    return "TRIM(" + to_sql_string(context, t._expr) + ")";
  }

  template<typename T>
    using check_trim_args = ::sqlpp::enable_if_t<is_text<T>::value>;

  template <typename T, typename = check_trim_args<T>>
  auto trim(T t) -> trim_t<T>
  {
    return {std::move(t)};
  }

}  // namespace sqlpp

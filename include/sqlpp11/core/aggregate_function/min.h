#pragma once

/*
 * Copyright (c) 2013, Roland Bock
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

#include <sqlpp11/core/name/alias_provider.h>
#include <sqlpp11/core/operator/enable_as.h>
#include <sqlpp11/core/operator/enable_comparison.h>
#include <sqlpp11/core/aggregate_function/enable_over.h>
#include <sqlpp11/core/clause/select_flags.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  namespace alias
  {
    SQLPP_ALIAS_PROVIDER(min);
  }

  template <typename Flag, typename Expr>
  struct min_t : public enable_as<min_t<Flag, Expr>>,
                 public enable_comparison<min_t<Flag, Expr>>,
                 enable_over<min_t<Flag, Expr>>
  {
    using _traits = make_traits<value_type_of_t<Expr>, tag::is_expression, tag::is_selectable>;
    using _can_be_null = std::true_type;
    using _is_aggregate_expression = std::true_type;

    constexpr min_t(Expr expr) : _expr(std::move(expr))
    {
    }

    min_t(const min_t&) = default;
    min_t(min_t&&) = default;
    min_t& operator=(const min_t&) = default;
    min_t& operator=(min_t&&) = default;
    ~min_t() = default;

    Expr _expr;
  };

  template <typename Flag, typename Expr>
  struct name_tag_of<min_t<Flag, Expr>>: public name_tag_of<alias::min_t>
  {
  };

  template <typename Flag, typename Expr>
  struct nodes_of<min_t<Flag, Expr>>
  {
    using type = sqlpp::detail::type_vector<Expr>;
  };

  template <typename Flag, typename Expr>
  struct value_type_of<min_t<Flag, Expr>>
  {
    using type = sqlpp::force_optional_t<value_type_of_t<Expr>>;
  };

  template <typename Context, typename Flag, typename Expr>
  Context& serialize(Context& context, const min_t<Flag, Expr>& t)
  {
    context << "MIN(";
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
  using check_min_arg =
      ::sqlpp::enable_if_t<values_are_comparable<T, T>::value and not contains_aggregate_function_t<T>::value>;

  template <typename T, typename = check_min_arg<T>>
  auto min(T t) -> min_t<noop, T>
  {
    return {std::move(t)};
  }

  template <typename T, typename = check_min_arg<T>>
  auto min(const distinct_t& /*unused*/, T t) -> min_t<distinct_t, T>
  {
    return {std::move(t)};
  }
}  // namespace sqlpp

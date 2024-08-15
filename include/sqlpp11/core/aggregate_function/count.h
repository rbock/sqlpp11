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
    SQLPP_ALIAS_PROVIDER(count);
  }

  template <typename Flag, typename Expr>
  struct count_t : public enable_as<count_t<Flag, Expr>>,
                   public enable_comparison<count_t<Flag, Expr>>,
                   public enable_over<count_t<Flag, Expr>>
  {
    using _traits = make_traits<integral, tag::is_expression /*, tag::is_selectable*/>;

    using _can_be_null = std::false_type;

    constexpr count_t(Expr expr) : _expr(std::move(expr))
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
  struct is_aggregate_function<count_t<Flag, Expr>> : public std::true_type
  {
  };

  template <typename Flag, typename Expr>
  struct name_tag_of<count_t<Flag, Expr>>: public name_tag_of<alias::count_t>
  {
  };

  template <typename Flag, typename Expr>
  struct nodes_of<count_t<Flag, Expr>>
  {
    using type = sqlpp::detail::type_vector<Expr>;
  };

  template <typename Flag, typename Expr>
  struct value_type_of<count_t<Flag, Expr>>
  {
    using type = integral;
  };

  template <typename Context, typename Flag, typename Expr>
  auto to_sql_string(Context& context, const count_t<Flag, Expr>& t) -> std::string
  {
    return "COUNT(" + to_sql_string(context, Flag()) + to_sql_string(context, t._expr) + ")";
  }

  template <typename T>
  using check_count_arg =
      ::sqlpp::enable_if_t<values_are_comparable<T, T>::value and not contains_aggregate_function<T>::value>;

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

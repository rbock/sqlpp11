#pragma once

/*
Copyright (c) 2024, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <sqlpp11/operator/comparison_expression.h>
#include <sqlpp11/operator/in_expression.h>
#include <sqlpp11/operator/sort_order_expression.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  // To be used as CRTP base for expressions that should offer the comparison member functions.
  template <typename Expr>
  class enable_comparison
  {
    constexpr auto derived() const -> const Expr&
    {
      return static_cast<const Expr&>(*this);
    }

  public:
    template <typename... Args>
    constexpr auto in(std::tuple<Args...> args) const -> in_expression<Expr, operator_in, std::tuple<Args...>>
    {
      return sqlpp::in(this->derived(), std::move(args));
    }

    template <typename... Args>
    constexpr auto in(Args... args) const -> in_expression<Expr, operator_in, std::tuple<Args...>>
    {
      return sqlpp::in(this->derived(), std::move(args)...);
    }

    template <typename Arg>
    constexpr auto in(std::vector<Arg> args) const -> in_expression<Expr, operator_in, std::vector<Arg>>
    {
      return sqlpp::in(this->derived(), std::move(args));
    }

    template <typename... Args>
    constexpr auto not_in(std::tuple<Args...> args) const -> in_expression<Expr, operator_not_in, std::tuple<Args...>>
    {
      return sqlpp::not_in(this->derived(), std::move(args));
    }

    template <typename... Args>
    constexpr auto not_in(Args... args) const -> in_expression<Expr, operator_not_in, std::tuple<Args...>>
    {
      return sqlpp::not_in(this->derived(), std::move(args)...);
    }

    template <typename Arg>
    constexpr auto not_in(std::vector<Arg> args) const -> in_expression<Expr, operator_not_in, std::vector<Arg>>
    {
      return sqlpp::not_in(this->derived(), std::move(args));
    }

    constexpr auto is_null() const -> comparison_expression<Expr, op_is_null, sqlpp::compat::nullopt_t>
    {
      return ::sqlpp::is_null(this->derived());
    }

    constexpr auto is_not_null() const -> comparison_expression<Expr, op_is_not_null, sqlpp::compat::nullopt_t>
    {
      return ::sqlpp::is_not_null(this->derived());
    }

    template <typename R>
    constexpr auto is_distinct_from(R r) const -> comparison_expression<Expr, op_is_distinct_from, R>
    {
      return ::sqlpp::is_distinct_from(this->derived(), std::move(r));
    }

    template <typename R>
    constexpr auto is_not_distinct_from(R r) const -> comparison_expression<Expr, op_is_not_distinct_from, R>
    {
      return ::sqlpp::is_not_distinct_from(this->derived(), std::move(r));
    }

    constexpr auto asc() const -> sort_order_expression<Expr>
    {
      return ::sqlpp::asc(this->derived());
    }

    constexpr auto desc() const -> sort_order_expression<Expr>
    {
      return ::sqlpp::desc(this->derived());
    }

    constexpr auto order(::sqlpp::sort_type t) const -> sort_order_expression<Expr>
    {
      return ::sqlpp::order(this->derived(), t);
    }

    template <typename R>
    constexpr auto like(R r) const -> comparison_expression<Expr, op_like, R>
    {
      return ::sqlpp::like(this->derived(), std::move(r));
    }
  };

}  // namespace sqlpp

#pragma once

/*
Copyright (c) 2018, Roland Bock
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

#include <utility>

#include <sqlpp11/core/noop.h>
#include <sqlpp11/core/enable_as.h>
#include <sqlpp11/core/enable_comparison.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
#warning: mysql does not offer operator||, we need to fail compilation, but maybe offer the concat function in addition
  template <typename L, typename Operator, typename R>
  struct arithmetic_expression : public enable_as<arithmetic_expression<L, Operator, R>>,
#warning: need to test AS and comparison for arithmetic expressions
                                 public enable_comparison<arithmetic_expression<L, Operator, R>>
  {
    arithmetic_expression() = delete;
    constexpr arithmetic_expression(L l, R r) : _l(l), _r(r)
    {
    }
    arithmetic_expression(const arithmetic_expression&) = default;
    arithmetic_expression(arithmetic_expression&&) = default;
    arithmetic_expression& operator=(const arithmetic_expression&) = default;
    arithmetic_expression& operator=(arithmetic_expression&&) = default;
    ~arithmetic_expression() = default;

    L _l;
    R _r;
  };

  template <typename L, typename R>
  using check_arithmetic_args = std::enable_if_t<is_numeric<L>::value and is_numeric<R>::value>;

#warning: need to document that this is on purpose (not integral, or unsigned integral, or floating_point) because it is difficult to know for the library to know what the actual result type will be (it is difficult to guess in C++ already, and it is probably different from DB vendor to vendor).
  template <typename L, typename Operator, typename R>
  struct value_type_of<arithmetic_expression<L, Operator, R>>
      : public std::conditional<sqlpp::is_optional<value_type_of_t<L>>::value or
                                    sqlpp::is_optional<value_type_of_t<R>>::value,
                                sqlpp::compat::optional<numeric>,
                                numeric>
  {
  };

  struct concatenation
  {
    static constexpr auto symbol = " || ";
  };

  template <typename L, typename R>
  struct value_type_of<arithmetic_expression<L, concatenation, R>>
      : public std::conditional<sqlpp::is_optional<value_type_of_t<L>>::value or
                                    sqlpp::is_optional<value_type_of_t<R>>::value,
                                sqlpp::compat::optional<text>,
                                text>
  {
  };

  template <typename L, typename Operator, typename R>
  struct nodes_of<arithmetic_expression<L, Operator, R>>
  {
    using type = detail::type_vector<L, R>;
  };

#if 0

  template <typename L, typename Operator, typename R>
  struct value_type_of_t<arithmetic_expression<L, Operator, R>>
  {
    using type = numeric_t;
  };

  template <typename L, typename Operator, typename R>
  constexpr auto requires_braces_v<arithmetic_expression<L, Operator, R>> = true;

  template <typename Context, typename L, typename Operator, typename R>
  [[nodiscard]] auto to_sql_string(Context& context, const arithmetic_expression<L, Operator, R>& t)
  {
    return to_sql_string(context, embrace(t._l)) + Operator::symbol + to_sql_string(context, embrace(t._r));
  }

  template <typename Context, typename Operator, typename R>
  [[nodiscard]] auto to_sql_string(Context& context, const arithmetic_expression<none_t, Operator, R>& t)
  {
    return Operator::symbol + to_sql_string(context, embrace(t._r));
  }

  template <typename Context, typename L1, typename Operator, typename R1, typename R2>
  [[nodiscard]] auto to_sql_string(Context& context,
                                   const arithmetic_expression<arithmetic_expression<L1, Operator, R1>, Operator, R2>& t)
  {
    return to_sql_string(context, t._l) + Operator::symbol + to_sql_string(context, embrace(t._r));
  }
#endif
  
  template <typename Context, typename L, typename Operator, typename R>
  auto serialize(Context& context, const arithmetic_expression<L, Operator, R>& t) -> Context&
  {
    context << "(";
    serialize_operand(context, t._l);
    context << Operator::symbol;
    serialize_operand(context, t._r);
    context << ")";
    return context;
  }

  struct plus
  {
    static constexpr auto symbol = " + ";
  };

  template <typename L, typename R, typename = check_arithmetic_args<L, R>>
  constexpr auto operator+(L l, R r) -> arithmetic_expression<L, plus, R>
  {
    return {std::move(l), std::move(r)};
  }

  template <typename L, typename R>
  using check_concatenation_args = std::enable_if_t<is_text<L>::value and is_text<R>::value>;

  template <typename L, typename R, typename = check_concatenation_args<L, R>>
  constexpr auto operator+(L l, R r) -> arithmetic_expression<L, concatenation, R>
  {
    return {std::move(l), std::move(r)};
  }

  struct minus
  {
    static constexpr auto symbol = " - ";
  };

  template <typename L, typename R, typename = check_arithmetic_args<L, R>>
  constexpr auto operator-(L l, R r) -> arithmetic_expression<L, minus, R>
  {
    return {std::move(l), std::move(r)};
  }

  struct multiplies
  {
    static constexpr auto symbol = " * ";
  };

  template <typename L, typename R, typename = check_arithmetic_args<L, R>>
  constexpr auto operator*(L l, R r) -> arithmetic_expression<L, multiplies, R>
  {
    return {std::move(l), std::move(r)};
  }

  struct divides
  {
    static constexpr auto symbol = " / ";
  };

  template <typename L, typename R, typename = check_arithmetic_args<L, R>>
  constexpr auto operator/(L l, R r) -> arithmetic_expression<L, divides, R>
  {
    return {std::move(l), std::move(r)};
  }

  struct negate
  {
    static constexpr auto symbol = "-";
  };

  template <typename R, typename = check_arithmetic_args<R, R>>
  constexpr auto operator-(R r) -> arithmetic_expression<noop, divides, R>
  {
    return {{}, std::move(r)};
  }

  struct modulus
  {
    static constexpr auto symbol = " % ";
  };

  template <typename L, typename R>
  using check_modulus_args = std::enable_if_t<(is_integral<L>::value or is_unsigned_integral<L>::value) and (is_integral<R>::value or is_unsigned_integral<R>::value)>;

  template <typename L, typename R, typename = check_modulus_args<L, R>>
  constexpr auto operator%(L l, R r) -> arithmetic_expression<L, modulus, R>
  {
    return {std::move(l), std::move(r)};
  }

}  // namespace sqlpp

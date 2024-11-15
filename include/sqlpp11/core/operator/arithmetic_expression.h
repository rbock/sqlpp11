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
#include <sqlpp11/core/operator/enable_as.h>
#include <sqlpp11/core/operator/enable_comparison.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  struct plus
  {
    static constexpr auto symbol = " + ";
  };

  struct minus
  {
    static constexpr auto symbol = " - ";
  };

  struct multiplies
  {
    static constexpr auto symbol = " * ";
  };

  struct divides
  {
    static constexpr auto symbol = " / ";
  };

  struct negate
  {
    static constexpr auto symbol = "-";
  };

  struct modulus
  {
    static constexpr auto symbol = " % ";
  };

  struct concatenation
  {
    static constexpr auto symbol = " || ";
  };

#warning: mysql does not offer operator||, we need to fail compilation, but maybe offer the concat function in addition
  template <typename L, typename Operator, typename R>
  struct arithmetic_expression : public enable_as<arithmetic_expression<L, Operator, R>>,
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
  using check_arithmetic_args = ::sqlpp::enable_if_t<is_numeric<L>::value and is_numeric<R>::value>;

  // L and R are expected to be numeric value types (boolean, integral, unsigned_integral, or floating_point).
  template <typename Operator, typename L, typename R>
  struct arithmetic_value_type
  {
    using type = numeric;
  };

  template <typename Operator, typename L, typename R>
  using arithmetic_value_type_t = typename arithmetic_value_type<Operator, L, R>::type;

#define SQLPP_ARITHMETIC_VALUE_TYPE(Op, Left, Right, ValueType)\
  template <>\
  struct arithmetic_value_type<Op, Left, Right>\
  {\
    using type = ValueType;\
  };

  // Operator plus
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, floating_point, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, floating_point, integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, floating_point, unsigned_integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, floating_point, boolean, floating_point);

  SQLPP_ARITHMETIC_VALUE_TYPE(plus, integral, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, integral, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, integral, unsigned_integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, integral, boolean, integral);

  SQLPP_ARITHMETIC_VALUE_TYPE(plus, unsigned_integral, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, unsigned_integral, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, unsigned_integral, unsigned_integral, unsigned_integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, unsigned_integral, boolean, unsigned_integral);

  SQLPP_ARITHMETIC_VALUE_TYPE(plus, boolean, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, boolean, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, boolean, unsigned_integral, unsigned_integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(plus, boolean, boolean, unsigned_integral);

  // Operator minus
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, floating_point, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, floating_point, integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, floating_point, unsigned_integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, floating_point, boolean, floating_point);

  SQLPP_ARITHMETIC_VALUE_TYPE(minus, integral, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, integral, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, integral, unsigned_integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, integral, boolean, integral);

  SQLPP_ARITHMETIC_VALUE_TYPE(minus, unsigned_integral, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, unsigned_integral, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, unsigned_integral, unsigned_integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, unsigned_integral, boolean, integral);

  SQLPP_ARITHMETIC_VALUE_TYPE(minus, boolean, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, boolean, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, boolean, unsigned_integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(minus, boolean, boolean, integral);

  // Operator multiplies
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, floating_point, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, floating_point, integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, floating_point, unsigned_integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, floating_point, boolean, floating_point);

  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, integral, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, integral, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, integral, unsigned_integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, integral, boolean, integral);

  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, unsigned_integral, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, unsigned_integral, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, unsigned_integral, unsigned_integral, unsigned_integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, unsigned_integral, boolean, unsigned_integral);

  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, boolean, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, boolean, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, boolean, unsigned_integral, unsigned_integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(multiplies, boolean, boolean, boolean);

  // Operator divides
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, floating_point, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, floating_point, integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, floating_point, unsigned_integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, floating_point, boolean, floating_point);

  SQLPP_ARITHMETIC_VALUE_TYPE(divides, integral, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, integral, integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, integral, unsigned_integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, integral, boolean, floating_point);

  SQLPP_ARITHMETIC_VALUE_TYPE(divides, unsigned_integral, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, unsigned_integral, integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, unsigned_integral, unsigned_integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, unsigned_integral, boolean, floating_point);

  SQLPP_ARITHMETIC_VALUE_TYPE(divides, boolean, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, boolean, integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, boolean, unsigned_integral, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(divides, boolean, boolean, floating_point);

  // Operator negate
  SQLPP_ARITHMETIC_VALUE_TYPE(negate, no_value_t, floating_point, floating_point);
  SQLPP_ARITHMETIC_VALUE_TYPE(negate, no_value_t, integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(negate, no_value_t, unsigned_integral, integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(negate, no_value_t, boolean, integral);

  // Operator modulus
  SQLPP_ARITHMETIC_VALUE_TYPE(modulus, integral, integral, unsigned_integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(modulus, integral, unsigned_integral, unsigned_integral);

  SQLPP_ARITHMETIC_VALUE_TYPE(modulus, unsigned_integral, integral, unsigned_integral);
  SQLPP_ARITHMETIC_VALUE_TYPE(modulus, unsigned_integral, unsigned_integral, unsigned_integral);

#undef SQLPP_ARITHMETIC_VALUE_TYPE

  // Handle optional types
  template <typename Operator, typename L, typename R>
  struct arithmetic_value_type<Operator, sqlpp::optional<L>, R>
  {
    using type = sqlpp::optional<arithmetic_value_type_t<Operator, L, R>>;
  };

  template <typename Operator, typename L, typename R>
  struct arithmetic_value_type<Operator, L, sqlpp::optional<R>>
  {
    using type = sqlpp::optional<arithmetic_value_type_t<Operator, L, R>>;
  };

  template <typename Operator, typename L, typename R>
  struct arithmetic_value_type<Operator, sqlpp::optional<L>, sqlpp::optional<R>>
  {
    using type = sqlpp::optional<arithmetic_value_type_t<Operator, L, R>>;
  };

  template <typename Operator, typename L, typename R>
  struct value_type_of<arithmetic_expression<L, Operator, R>>
  : public arithmetic_value_type<Operator, value_type_of_t<L>, value_type_of_t<R>>{};

  template <typename L, typename R>
  struct value_type_of<arithmetic_expression<L, concatenation, R>>
      : public std::conditional<sqlpp::is_optional<value_type_of_t<L>>::value or
                                    sqlpp::is_optional<value_type_of_t<R>>::value,
                                ::sqlpp::optional<text>,
                                text>
  {
  };

  template <typename L, typename Operator, typename R>
  struct nodes_of<arithmetic_expression<L, Operator, R>>
  {
    using type = detail::type_vector<L, R>;
  };

  template <typename L, typename Operator, typename R>
  struct requires_parentheses<arithmetic_expression<L, Operator, R>> : public std::true_type{};

  template <typename Context, typename L, typename Operator, typename R>
  auto to_sql_string(Context& context, const arithmetic_expression<L, Operator, R>& t) -> std::string
  {
    return operand_to_sql_string(context, t._l) + Operator::symbol + operand_to_sql_string(context, t._r);
  }

  template <typename L, typename R, typename = check_arithmetic_args<L, R>>
  constexpr auto operator+(L l, R r) -> arithmetic_expression<L, plus, R>
  {
    return {std::move(l), std::move(r)};
  }

  template <typename L, typename R>
  using check_concatenation_args = ::sqlpp::enable_if_t<is_text<L>::value and is_text<R>::value>;

  template <typename L, typename R, typename = check_concatenation_args<L, R>>
  constexpr auto operator+(L l, R r) -> arithmetic_expression<L, concatenation, R>
  {
    return {std::move(l), std::move(r)};
  }

  template <typename L, typename R, typename = check_arithmetic_args<L, R>>
  constexpr auto operator-(L l, R r) -> arithmetic_expression<L, minus, R>
  {
    return {std::move(l), std::move(r)};
  }

  template <typename L, typename R, typename = check_arithmetic_args<L, R>>
  constexpr auto operator*(L l, R r) -> arithmetic_expression<L, multiplies, R>
  {
    return {std::move(l), std::move(r)};
  }

  template <typename L, typename R, typename = check_arithmetic_args<L, R>>
  constexpr auto operator/(L l, R r) -> arithmetic_expression<L, divides, R>
  {
    return {std::move(l), std::move(r)};
  }

  template <typename R, typename = check_arithmetic_args<R, R>>
  constexpr auto operator-(R r) -> arithmetic_expression<noop, negate, R>
  {
    return {{}, std::move(r)};
  }

  template <typename L, typename R>
  using check_modulus_args = ::sqlpp::enable_if_t<(is_integral<L>::value or is_unsigned_integral<L>::value) and (is_integral<R>::value or is_unsigned_integral<R>::value)>;

  template <typename L, typename R, typename = check_modulus_args<L, R>>
  constexpr auto operator%(L l, R r) -> arithmetic_expression<L, modulus, R>
  {
    return {std::move(l), std::move(r)};
  }

#warning: Add and test date/time/duration arithmetics

}  // namespace sqlpp

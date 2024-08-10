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

#include <type_traits>

#include <sqlpp11/core/operator/enable_as.h>
#include <sqlpp11/core/noop.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename L, typename Operator, typename R>
  struct bit_expression : public enable_as<bit_expression<L, Operator, R>>
  {
    constexpr bit_expression(L l, R r) : _l(std::move(l)), _r(std::move(r))
    {
    }
    bit_expression(const bit_expression&) = default;
    bit_expression(bit_expression&&) = default;
    bit_expression& operator=(const bit_expression&) = default;
    bit_expression& operator=(bit_expression&&) = default;
    ~bit_expression() = default;

    L _l;
    R _r;
  };

  template <typename L, typename Operator, typename R>
  struct value_type_of<bit_expression<L, Operator, R>>
  {
    using type = typename std::conditional<sqlpp::is_optional<value_type_of_t<L>>::value or
                                               sqlpp::is_optional<value_type_of_t<R>>::value,
                                           ::sqlpp::optional<integral>,
                                           integral>::type;
  };

  template <typename L, typename Operator, typename R>
  struct nodes_of<bit_expression<L, Operator, R>>
  {
    using type = detail::type_vector<L, R>;
  };

  template <typename L, typename R>
  using check_bit_expression_args = ::sqlpp::enable_if_t<is_integral<L>::value and is_integral<R>::value>;

  template <typename L, typename R>
  using check_bit_shift_expression_args = ::sqlpp::enable_if_t<is_integral<L>::value and (is_integral<R>::value or is_unsigned_integral<R>::value)>;

  template <typename L, typename Operator, typename R>
  struct requires_parentheses<bit_expression<L, Operator, R>> : public std::true_type {};

  template <typename Context, typename L, typename Operator, typename R>
  auto to_sql_string(Context& context, const bit_expression<L, Operator, R>& t) -> std::string
  {
    return to_sql_string(context, simple_column(t._l)) + Operator::symbol + operand_to_sql_string(context, t._r);
  }

  struct bit_and
  {
    static constexpr auto symbol = " & ";
  };

  template <typename L, typename R, typename = check_bit_expression_args<L, R>>
  constexpr auto operator&(L l, R r) -> bit_expression<L, bit_and, R>
  {
    return {std::move(l), std::move(r)};
  }

  struct bit_or
  {
    static constexpr auto symbol = " | ";
  };

  template <typename L, typename R, typename = check_bit_expression_args<L, R>>
  constexpr auto operator|(L l, R r) -> bit_expression<L, bit_or, R>
  {
    return {std::move(l), std::move(r)};
  }

  struct bit_xor
  {
    static constexpr auto symbol = " ^ ";
  };

  template <typename L, typename R, typename = check_bit_expression_args<L, R>>
  constexpr auto operator^(L l, R r) -> bit_expression<L, bit_xor, R>
  {
    return {std::move(l), std::move(r)};
  }

  struct bit_not
  {
    static constexpr auto symbol = "~";
  };

  template <typename R, typename = check_bit_expression_args<R, R>>
  constexpr auto operator~(R r) -> bit_expression<noop, bit_not, R>
  {
    return {{}, std::move(r)};
  }

  struct bit_shift_left
  {
    static constexpr auto symbol = " << ";
  };

  template <typename L, typename R, typename = check_bit_shift_expression_args<L, R>>
  constexpr auto operator<<(L l, R r) -> bit_expression<L, bit_shift_left, R>
  {
    return {std::move(l), std::move(r)};
  }

  struct bit_shift_right
  {
    static constexpr auto symbol = " >> ";
  };

  template <typename L, typename R, typename = check_bit_shift_expression_args<L, R>>
  constexpr auto operator>>(L l, R r) -> bit_expression<L, bit_shift_right, R>
  {
    return {std::move(l), std::move(r)};
  }

}  // namespace sqlpp

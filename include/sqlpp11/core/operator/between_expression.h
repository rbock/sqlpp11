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

#include <sqlpp11/core/operator/enable_as.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/logic.h>

namespace sqlpp
{
  template<typename L, typename R1, typename R2>
  struct between_expression : public enable_as<between_expression<L, R1, R2>>
  {
    constexpr between_expression(L l, R1 r1, R2 r2) : _l(std::move(l)), _r1(std::move(r1)), _r2(std::move(r2)) {}
    between_expression(const between_expression&) = default;
    between_expression(between_expression&&) = default;
    between_expression& operator=(const between_expression&) = default;
    between_expression& operator=(between_expression&&) = default;
    ~between_expression() = default;

    L _l;
    R1 _r1;
    R2 _r2;
  };

  template <typename L, typename R1, typename R2>
  using check_between_args =
      ::sqlpp::enable_if_t<values_are_comparable<L, R1>::value and values_are_comparable<L, R2>::value>;

  template <typename L, typename R1, typename R2>
  struct value_type_of<between_expression<L, R1, R2>>
      : public std::conditional<sqlpp::is_optional<value_type_of_t<L>>::value or
                                    sqlpp::is_optional<value_type_of_t<R1>>::value or
                                    sqlpp::is_optional<value_type_of_t<R2>>::value,
                                ::sqlpp::optional<boolean>,
                                boolean>
  {
  };

  template <typename L, typename R1, typename R2>
  struct nodes_of<between_expression<L, R1, R2>>
  {
    using type = detail::type_vector<L, R1, R2>;
  };

  template <typename L, typename R1, typename R2>
  struct requires_parentheses<between_expression<L, R1, R2>> : public std::true_type{};

#warning: Need tests for between expressions
  template <typename L, typename R1, typename R2, typename = check_between_args<L, R1, R2>>
  constexpr auto between(L l, R1 r1, R2 r2) -> between_expression<L, R1, R2>
  {
    return {std::move(l), std::move(r1), std::move(r2)};
  }

  template <typename Context, typename L, typename R1, typename R2>
  auto serialize(Context& context, const between_expression<L, R1, R2>& t) -> Context&
  {
    serialize_operand(context, t._l);
    context << " BETWEEN ";
    serialize_operand(context, t._r1);
    context << " AND ";
    serialize_operand(context, t._r2);
    return context;
  }

}  // namespace sqlpp

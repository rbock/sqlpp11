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

#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  template <typename L, typename Operator, typename R>
  struct comparison_expression
  {
    L l;
    R r;
  };

  template <typename L, typename R>
  using check_comparison_args = std::enable_if_t<values_are_comparable<L, R>::value>;

#if 0
  template <typename L, typename Operator, typename R>
  struct nodes_of<comparison_expression<L, Operator, R>>
  {
    using type = type_vector<L, R>;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_comparison_operands_are_compatible,
                              "comparison operands must have compatible value types");

  template <typename L, typename R>
  constexpr auto check_comparison_args()
  {
    if constexpr (not values_are_compatible_v<L, R>)
    {
      return failed<assert_comparison_operands_are_compatible>{};
    }
    else
    {
      return succeeded{};
    }
  }

  template <typename L, typename Operator, typename R>
  struct value_type_of_t<comparison_expression<L, Operator, R>>
  {
    using type = bool;
  };

  template <typename L, typename Operator, typename R>
  constexpr auto requires_braces_v<comparison_expression<L, Operator, R>> = true;

  template <typename Context, typename L, typename Operator, typename R>
  [[nodiscard]] auto to_sql_string(Context& context, const comparison_expression<L, Operator, R>& t)
  {
    return to_sql_string(context, embrace(t.l)) + Operator::symbol + to_sql_string(context, embrace(t.r));
  }
#endif
}  // namespace sqlpp

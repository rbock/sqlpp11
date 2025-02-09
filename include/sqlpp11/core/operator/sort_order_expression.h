#pragma once

/*
Copyright (c) 2017 - 2018, Roland Bock
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

#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  enum class sort_type
  {
    asc,
    desc,
  };

  template <typename L>
  struct sort_order_expression
  {
    constexpr sort_order_expression(L l, sort_type r) : _l(std::move(l)), _r(std::move(r))
    {
    }
    sort_order_expression(const sort_order_expression&) = default;
    sort_order_expression(sort_order_expression&&) = default;
    sort_order_expression& operator=(const sort_order_expression&) = default;
    sort_order_expression& operator=(sort_order_expression&&) = default;
    ~sort_order_expression() = default;

    L _l;
    sort_type _r;
  };

  template <typename L>
  using check_sort_order_args = std::enable_if_t<values_are_comparable<L, L>::value>;

  template <typename L>
  struct nodes_of<sort_order_expression<L>>
  {
    using type = detail::type_vector<L>;
  };

  template <typename L>
  struct is_sort_order<sort_order_expression<L>> : std::true_type {};

  template <typename Context>
  auto to_sql_string(Context& , const sort_type& t) -> std::string
  {
    if (t == sort_type::asc)
    {
      return " ASC";
    }
    return " DESC";
  }

  template <typename Context, typename L>
  auto to_sql_string(Context& context, const sort_order_expression<L>& t) -> std::string
  {
    return operand_to_sql_string(context, t._l) + to_sql_string(context, t._r);
  }

  template <typename L, typename = check_sort_order_args<L>>
  constexpr auto asc(L l) -> sort_order_expression<L>
  {
    return {l, sort_type::asc};
  }

  template <typename L, typename = check_sort_order_args<L>>
  constexpr auto desc(L l) -> sort_order_expression<L>
  {
    return {l, sort_type::desc};
  }

  template <typename L, typename = check_sort_order_args<L>>
  constexpr auto order(L l, sort_type order) -> sort_order_expression<L>
  {
    return {l, order};
  }

}  // namespace sqlpp

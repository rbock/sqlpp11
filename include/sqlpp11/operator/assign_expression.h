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

#include <utility>

#include <sqlpp11/type_traits.h>
#include <sqlpp11/default_value.h>

namespace sqlpp
{
  template <typename L, typename R>
  struct assign_expression
  {
    L column;
    R value;
  };

#warning rename is_column, is_default, values_are_comparable -> values_are_compatible
#warning document the change that there is no read-only!
#warning handle default_t, but only if the column does have a default!
  template <typename L, typename R>
  using check_assign_args = std::enable_if_t<is_column_t<L>::value and values_are_comparable<L, R>::value and
                                             (can_be_null<L>::value or not is_optional<R>::value) and
                                             (has_default_t<L>::value or not std::is_same<R, default_value_t>::value)>;

  template <typename L, typename R, typename = check_assign_args<L, R>>
  constexpr auto assign(L column, R value) -> assign_expression<L, R>
  {
    return {std::move(column), std::move(value)};
  }

  /*
  template <typename L, typename R>
  struct nodes_of<assign_t<L, R>>
  {
    using type = type_vector<L, R>;
  };

  template <typename L, typename R>
  constexpr auto is_assignment_v<assign_t<L, R>> = true;

  template <typename L, typename R>
  struct column_of<assign_t<L, R>>
  {
    using type = L;
  };

  template <typename L, typename R>
  constexpr auto requires_braces_v<assign_t<L, R>> = true;

  template <typename Context, typename L, typename R>
  [[nodiscard]] auto to_sql_string(Context& context, const assign_t<L, R>& t)
  {
    return to_sql_string(context, t.column) + " = " + to_sql_string(context, embrace(t.value));
  }
  */
}  // namespace sqlpp

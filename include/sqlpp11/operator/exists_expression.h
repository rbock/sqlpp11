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

#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  template <typename SubSelect>
  struct exists_t
  {
    SubSelect sub_select;
  };

  template <typename SubSelect>
  using check_exists_arg = std::enable_if_t<is_statement_t<SubSelect>::value and has_result_row_t<SubSelect>::value>;

  template <typename SubSelect>
  struct value_type_of<exists_t<SubSelect>>
  {
    using type = boolean;
  };

#warning: Document that functions dont come with their default alias any more
  template <typename SubSelect, typename = check_exists_arg<SubSelect>>
  constexpr auto exists(SubSelect sub_select) -> exists_t<SubSelect>
  {
    return exists_t<SubSelect>{sub_select};
  }

  /*
  template <typename SubSelect>
  struct nodes_of<exists_t<SubSelect>>
  {
    using type = type_vector<SubSelect>;
  };

  template <typename Context, typename SubSelect>
  [[nodiscard]] auto to_sql_string(Context& context, const exists_t<SubSelect>& t)
  {
    return " EXISTS(" + to_sql_string(context, t.sub_select) + ") ";
  }
  */
}  // namespace sqlpp

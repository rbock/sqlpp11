#pragma once

/*
 * Copyright (c) 2025, Roland Bock
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

#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/query/dynamic_fwd.h>
#include <sqlpp11/core/operator/expression_as_fwd.h>

namespace sqlpp
{
  template <typename T>
  concept Table = is_table_v<T>;

  template <typename T>
  concept DynamicTable = is_table_v<remove_dynamic_t<T>>;

  template <typename T>
    struct is_select_column
    {
      static constexpr bool value = has_value_type_v<remove_as_t<remove_dynamic_t<T>>> and has_name_tag_v<remove_dynamic_t<T>>;
    };
  template <typename T>
    static inline constexpr bool is_select_column_v = is_select_column<T>::value;

  template<typename... T>
    struct is_select_column<std::tuple<T...>>
    {
      static constexpr bool value = (true and ... and is_select_column_v<T>);
    };
  template <typename T>
  concept SelectColumn = is_select_column_v<T>;

}  // namespace sqlpp

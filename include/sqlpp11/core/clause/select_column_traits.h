#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/core/operator/expression_as.h>
#include <sqlpp11/core/query/dynamic.h>
#include <sqlpp11/core/type_traits.h>

// Select columns require a value type and a name.
// They can be dynamic values and they can be expression_ass.
// These type traits consider `dynamic_t` and `expression_as`

namespace sqlpp
{
  // Get value type
  template <typename T>
  struct select_column_value_type_of : public value_type_of<T>
  {
  };
  template <typename T>
  using select_column_value_type_of_t = typename select_column_value_type_of<T>::type;

  template <typename T>
  struct select_column_value_type_of<dynamic_t<T>>
  {
    using type = sqlpp::force_optional_t<select_column_value_type_of_t<T>>;
  };

  template <typename T, typename NameTagProvider>
  struct select_column_value_type_of<expression_as<T, NameTagProvider>> : public select_column_value_type_of<T>
  {
  };

  // Get name tag
  template <typename T>
  struct select_column_name_tag_of : public name_tag_of<T>
  {
  };

  template <typename T>
  using select_column_name_tag_of_t = typename select_column_name_tag_of<T>::type;

  template <typename T>
  struct select_column_name_tag_of<dynamic_t<T>> : public select_column_name_tag_of<T>
  {
  };

  template <typename T, typename NameTagProvider>
  struct select_column_name_tag_of<expression_as<T, NameTagProvider>> : public name_tag_of<NameTagProvider>
  {
  };

  // Test for value
  template <typename T>
  struct select_column_has_value_type
      : public std::integral_constant<bool, not std::is_same<select_column_value_type_of_t<T>, no_value_t>::value>
  {
  };

  // Test for name
  template <typename T>
  struct select_column_has_name
      : public std::integral_constant<bool, not std::is_same<select_column_name_tag_of_t<T>, no_name_t>::value>
  {
  };

}  // namespace sqlpp

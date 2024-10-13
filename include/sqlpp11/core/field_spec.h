#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include <type_traits>

#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/clause/select_column_traits.h>

namespace sqlpp
{
#warning: Do we need this? It should be possible to use name_tag_of_t and value_type_of_t somehow
  template <typename NameType, typename ValueType>
  struct field_spec_t : public name_tag_base
  {
    using _sqlpp_name_tag = NameType;

#warning: Maybe rename result_value in result_value_type?
    using cpp_type = result_value_t<ValueType>;
  };

  template <typename NameType, typename ValueType>
  struct name_tag_of<field_spec_t<NameType, ValueType>>
  {
    using type = NameType;
  };

  template <typename Left, typename Right, typename Enable = void>
  struct is_field_compatible
  {
    static constexpr auto value = false;
  };

  template <typename LeftName,
            typename LeftValue,
            typename RightName,
            typename RightValue>
  struct is_field_compatible<field_spec_t<LeftName, LeftValue>,
                             field_spec_t<RightName, RightValue>>
  {
    using L = field_spec_t<LeftName, LeftValue>;
    using R = field_spec_t<RightName, RightValue>;
    static constexpr auto value =
        std::is_same<make_char_sequence_t<L>, make_char_sequence_t<R>>::value and
        std::is_same<remove_optional_t<LeftValue>, remove_optional_t<RightValue>>::value and  // Same value type
        (is_optional<LeftValue>::value or
         !is_optional<RightValue>::value);  // The left hand side determines the result row and therefore must allow
                                            // NULL if the right hand side allows it
  };

  namespace detail
  {
    template <typename Select, typename NamedExpr>
    struct make_field_spec_impl
    {
      using ValueType = select_column_value_type_of_t<NamedExpr>;
      static constexpr bool _depends_on_optional_table =
          Select::_provided_optional_tables::contains_any(required_tables_of_t<NamedExpr>{});

      using type = field_spec_t<
          select_column_name_tag_of_t<NamedExpr>,
          typename std::conditional<_depends_on_optional_table, sqlpp::force_optional_t<ValueType>, ValueType>::type>;
    };
  }  // namespace detail

  template <typename Select, typename NamedExpr>
  using make_field_spec_t = typename detail::make_field_spec_impl<Select, NamedExpr>::type;
}  // namespace sqlpp

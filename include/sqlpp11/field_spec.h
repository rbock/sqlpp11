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

#include <sqlpp11/type_traits.h>

#include <type_traits>

#include <sqlpp11/compat/optional.h>

namespace sqlpp
{
  template <typename NameType, typename ValueType>
  struct field_spec_t
  {
    using _alias_t = NameType;

#warning: Maybe rename result_value in result_value_type?
    using cpp_type = result_value_t<ValueType>;
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
    /* TODO reactivate
    static constexpr auto value =
        std::is_same<typename LeftName::_name_t, typename RightName::_name_t>::value and
        std::is_same<LeftValue, RightValue>::value and  // Same value type
        (LeftCanBeNull or !RightCanBeNull);  // The left hand side determines the result row and therefore must allow
                                             // NULL if the right hand side allows it
                                             */
  };

  namespace detail
  {
    template <typename Select, typename NamedExpr>
    struct make_field_spec_impl
    {
#warning: required_tables_of and obtaining the alias should handle optional.
      using RawNamedExpr = remove_optional_t<NamedExpr>;
      static constexpr bool _depends_on_outer_table =
          detail::make_intersect_set_t<required_tables_of<RawNamedExpr>,
                                       typename Select::_used_outer_tables>::size::value > 0;
      using ValueType = typename std::conditional<_depends_on_outer_table,
                                                  sqlpp::force_optional_t<value_type_of_t<NamedExpr>>,
                                                  value_type_of_t<NamedExpr>>::type;

      using type = field_spec_t<typename RawNamedExpr::_alias_t,
                                ValueType>;
    };
  }  // namespace detail

  template <typename Select, typename NamedExpr>
  using make_field_spec_t = typename detail::make_field_spec_impl<Select, NamedExpr>::type;
}  // namespace sqlpp

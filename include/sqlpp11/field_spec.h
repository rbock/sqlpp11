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

#ifndef SQLPP_FIELD_SPEC_H
#define SQLPP_FIELD_SPEC_H

#include <sqlpp11/multi_column.h>

namespace sqlpp
{
  template <typename NameType, typename ValueType, bool CanBeNull, bool NullIsTrivialValue>
  struct field_spec_t
  {
    using _traits = make_traits<ValueType,
                                tag::is_noop,
                                tag_if<tag::can_be_null, CanBeNull>,
                                tag_if<tag::null_is_trivial_value, NullIsTrivialValue>>;
    using _nodes = detail::type_vector<>;

    using _alias_t = NameType;
  };

  template <typename AliasProvider, typename FieldSpecTuple>
  struct multi_field_spec_t
  {
  };

  namespace detail
  {
    template <typename Select, typename NamedExpr>
    struct make_field_spec_impl
    {
      static constexpr bool _can_be_null = can_be_null_t<NamedExpr>::value;
      static constexpr bool _depends_on_outer_table =
          detail::make_intersect_set_t<required_tables_of<NamedExpr>,
                                       typename Select::_used_outer_tables>::size::value > 0;

      using type = field_spec_t<typename NamedExpr::_alias_t,
                                value_type_of<NamedExpr>,
                                logic::any_t<_can_be_null, _depends_on_outer_table>::value,
                                null_is_trivial_value_t<NamedExpr>::value>;
    };

    template <typename Select, typename AliasProvider, typename... NamedExprs>
    struct make_field_spec_impl<Select, multi_column_alias_t<AliasProvider, NamedExprs...>>
    {
      using type =
          multi_field_spec_t<AliasProvider, std::tuple<typename make_field_spec_impl<Select, NamedExprs>::type...>>;
    };
  }

  template <typename Select, typename NamedExpr>
  using make_field_spec_t = typename detail::make_field_spec_impl<Select, NamedExpr>::type;
}

#endif

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

#ifndef SQLPP_PARAMETER_H
#define SQLPP_PARAMETER_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/data_types/parameter_value.h>
#include <sqlpp11/expression_operators.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
  template <typename ValueType, typename NameType>
  struct parameter_t : public expression_operators<parameter_t<ValueType, NameType>, ValueType>
  {
    using _traits = make_traits<ValueType, tag::is_parameter, tag::is_expression>;

    using _nodes = detail::type_vector<>;
    using _parameters = detail::type_vector<parameter_t>;
    using _can_be_null = std::true_type;

    using _instance_t = member_t<NameType, parameter_value_t<ValueType>>;

    parameter_t()
    {
    }

    parameter_t(const parameter_t&) = default;
    parameter_t(parameter_t&&) = default;
    parameter_t& operator=(const parameter_t&) = default;
    parameter_t& operator=(parameter_t&&) = default;
    ~parameter_t() = default;
  };

  template <typename Context, typename ValueType, typename NameType>
  struct serializer_t<Context, parameter_t<ValueType, NameType>>
  {
    using _serialize_check = consistent_t;
    using T = parameter_t<ValueType, NameType>;

    static Context& _(const T&, Context& context)
    {
      context << "?";
      return context;
    }
  };

  template <typename NamedExpr>
  auto parameter(const NamedExpr&) -> parameter_t<value_type_of<NamedExpr>, NamedExpr>
  {
    static_assert(is_selectable_t<NamedExpr>::value, "not a named expression");
    return {};
  }

  template <typename ValueType, typename AliasProvider>
  auto parameter(const ValueType&, const AliasProvider&) -> parameter_t<wrap_operand_t<ValueType>, AliasProvider>
  {
    static_assert(is_value_type_t<ValueType>::value, "first argument is not a value type");
    static_assert(is_alias_provider_t<AliasProvider>::value, "second argument is not an alias provider");
    return {};
  }
}

#endif

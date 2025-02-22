#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/name/create_name_tag.h>
#include <sqlpp11/core/operator/enable_comparison.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp {
template <typename ValueType, typename NameTag>
struct parameter_t : public enable_as<parameter_t<ValueType, NameTag>>,
                     public enable_comparison<parameter_t<ValueType, NameTag>> {
  using _instance_t =
      typename NameTag::template _member_t<parameter_value_t<ValueType>>;
  parameter_t() = default;

  parameter_t(const parameter_t &) = default;
  parameter_t(parameter_t &&) = default;
  parameter_t &operator=(const parameter_t &) = default;
  parameter_t &operator=(parameter_t &&) = default;
  ~parameter_t() = default;
};

template <typename ValueType, typename NameTag>
struct parameters_of<parameter_t<ValueType, NameTag>> {
  using type = detail::type_vector<parameter_t<ValueType, NameTag>>;
};

template <typename ValueType, typename NameTag>
struct value_type_of<parameter_t<ValueType, NameTag>> {
  using type = ValueType;
};

template <typename Context, typename ValueType, typename NameTag>
auto to_sql_string(Context &, const parameter_t<ValueType, NameTag> &)
    -> std::string {
  return "?";
}

template <typename NamedExpr>
auto parameter(const NamedExpr & /*unused*/)
    -> parameter_t<value_type_of_t<NamedExpr>, name_tag_of_t<NamedExpr>> {
  static_assert(has_value_type<NamedExpr>::value, "not a named expression");
  static_assert(has_name_tag<NamedExpr>::value, "not a named expression");
  return {};
}

template <typename ValueType, typename NameTagProvider>
auto parameter(const ValueType & /*unused*/, const NameTagProvider & /*unused*/)
    -> parameter_t<value_type_of_t<ValueType>, name_tag_of_t<NameTagProvider>> {
  static_assert(has_value_type<ValueType>::value,
                "first argument is not a value type");
  static_assert(has_name_tag<NameTagProvider>::value,
                "second argument does not have a name");
  return {};
}
} // namespace sqlpp

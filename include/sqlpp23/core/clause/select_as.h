#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp23/core/basic/enable_join.h>
#include <sqlpp23/core/operator/enable_as.h>
#include <sqlpp23/core/query/statement.h>
#include <sqlpp23/core/reader.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename NameTag>
struct select_ref_t {};

template <typename NameTag>
struct name_tag_of<select_ref_t<NameTag>> {
  using type = NameTag;
};

// select_member is a helper to add column data members to `select_as_t`.
template <typename NameTag, typename FieldSpec>
struct select_member {
  using type = member_t<FieldSpec, column_t<select_ref_t<NameTag>, FieldSpec>>;
};

template <typename Select, typename NameTag, typename... FieldSpecs>
struct select_as_t
    : public select_member<NameTag, FieldSpecs>::type...,
      public enable_join<select_as_t<Select, NameTag, FieldSpecs...>> {
  select_as_t(Select select) : _expression(select) {}

  select_as_t(const select_as_t& rhs) = default;
  select_as_t(select_as_t&& rhs) = default;
  select_as_t& operator=(const select_as_t& rhs) = default;
  select_as_t& operator=(select_as_t&& rhs) = default;
  ~select_as_t() = default;

  using _column_tuple_t =
      std::tuple<column_t<select_ref_t<NameTag>, FieldSpecs>...>;

 private:
  friend reader_t;
  Select _expression;
};

template <typename Context,
          typename Select,
          typename NameTag,
          typename... FieldSpecs>
auto to_sql_string(Context& context,
                   const select_as_t<Select, NameTag, FieldSpecs...>& t)
    -> std::string {
  return operand_to_sql_string(context, read.expression(t)) + " AS " +
         name_to_sql_string(context, NameTag{});
}

// No value_type_of defined. select_as_t represents a table, not a value.
// Rationale: select.as() requires prepare_check to be used as tbale, whereas
// using as value just requires consistency.

template <typename Select, typename NameTag, typename... FieldSpecs>
struct name_tag_of<select_as_t<Select, NameTag, FieldSpecs...>> {
  using type = NameTag;
};

// We need to track nodes to find parameters or required tables in sub selects.
template <typename Select, typename NameTag, typename... FieldSpecs>
struct nodes_of<select_as_t<Select, NameTag, FieldSpecs...>> {
  using type = detail::type_vector<Select>;
};

template <typename Select, typename NameTag, typename... FieldSpecs>
struct is_table<select_as_t<Select, NameTag, FieldSpecs...>>
    : std::integral_constant<bool, can_be_used_as_table<Select>::value> {};

template <typename Select, typename NameTag, typename... FieldSpecs>
struct provided_tables_of<select_as_t<Select, NameTag, FieldSpecs...>>
    : public std::conditional<can_be_used_as_table<Select>::value,
                              sqlpp::detail::type_set<select_ref_t<NameTag>>,
                              sqlpp::detail::type_set<>> {};

}  // namespace sqlpp

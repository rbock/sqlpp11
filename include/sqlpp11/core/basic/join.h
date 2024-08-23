#pragma once

/*
 * Copyright (c) 2013, Roland Bock
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

#include <sqlpp11/core/basic/join_types.h>
#include <sqlpp11/core/basic/pre_join.h>
#include <sqlpp11/core/basic/on.h>

namespace sqlpp
{
  template <typename PreJoin, typename On>
  struct join_t
  {
    using _traits = make_traits<no_value_t, tag::is_join>;
    using _nodes = detail::type_vector<PreJoin, On>;
    using _can_be_null = std::false_type;
    using _provided_tables = provided_tables_of_t<PreJoin>;
    using _required_tables = detail::make_difference_set_t<required_tables_of_t<On>, _provided_tables>;

    template <typename T>
    auto join(T t) const -> decltype(::sqlpp::join(*this, t))
    {
      return ::sqlpp::join(*this, t);
    }

    template <typename T>
    auto inner_join(T t) const -> decltype(::sqlpp::inner_join(*this, t))
    {
      return ::sqlpp::inner_join(*this, t);
    }

    template <typename T>
    auto left_outer_join(T t) const -> decltype(::sqlpp::left_outer_join(*this, t))
    {
      return ::sqlpp::left_outer_join(*this, t);
    }

    template <typename T>
    auto right_outer_join(T t) const -> decltype(::sqlpp::right_outer_join(*this, t))
    {
      return ::sqlpp::right_outer_join(*this, t);
    }

    template <typename T>
    auto outer_join(T t) const -> decltype(::sqlpp::outer_join(*this, t))
    {
      return ::sqlpp::outer_join(*this, t);
    }

    template <typename T>
    auto cross_join(T t) const -> decltype(::sqlpp::cross_join(*this, t))
    {
      return ::sqlpp::cross_join(*this, t);
    }

    PreJoin _pre_join;
    On _on;
  };

  template<typename PreJoin, typename On>
    struct nodes_of<join_t<PreJoin, On>>
    {
      using type = sqlpp::detail::type_vector<PreJoin, On>;
    };

  template<typename PreJoin, typename On>
    struct provided_outer_tables_of<join_t<PreJoin, On>>
    {
      using type = provided_outer_tables_of_t<PreJoin>;
    };

  template<typename PreJoin, typename On>
    struct is_table<join_t<PreJoin, On>> : public std::true_type{};

  template <typename Context, typename PreJoin, typename On>
  auto to_sql_string(Context& context, const join_t<PreJoin, On>& t) -> std::string
  {
    return to_sql_string(context, t._pre_join) + to_sql_string(context, t._on);
  }
}  // namespace sqlpp

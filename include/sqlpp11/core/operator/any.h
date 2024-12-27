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

#include <sqlpp11/core/query/statement_fwd.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename Select>
  struct any_t
  {
    constexpr any_t(Select select) : _select(std::move(select))
    {
    }
    any_t(const any_t&) = default;
    any_t(any_t&&) = default;
    any_t& operator=(const any_t&) = default;
    any_t& operator=(any_t&&) = default;
    ~any_t() = default;

    Select _select;
  };

  // No value_type_of defined for any_t, because it is to be used with basic comparison operators, only.

  template <typename Select>
  struct nodes_of<any_t<Select>>
  {
    using type = detail::type_vector<Select>;
  };

 template <typename T>
  struct remove_any
  {
    using type = T;
  };

  template <typename Select>
  struct remove_any<any_t<Select>>
  {
    using type = Select;
  };

  template <typename T>
  using remove_any_t = typename remove_any<T>::type;

  template <typename Context, typename Select>
  auto to_sql_string(Context& context, const any_t<Select>& t) -> std::string
  {
    return "ANY (" + to_sql_string(context, t._select) + ")";
  }

  template <typename Select>
  using check_any_args = ::sqlpp::enable_if_t<has_value_type<Select>::value>;

  template <typename... Clauses, typename = check_any_args<statement_t<Clauses...>>>
  auto any(statement_t<Clauses...> t) -> any_t<statement_t<Clauses...>>
  {
    statement_consistency_check_t<statement_t<Clauses...>>::verify();
    return {std::move(t)};
  }
}  // namespace sqlpp

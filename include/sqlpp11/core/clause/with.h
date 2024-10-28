#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <sqlpp11/core/operator/assign_expression.h>
#include <sqlpp11/core/basic/column_fwd.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/no_data.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/query/statement_fwd.h>
#include <sqlpp11/core/type_traits.h>

#include <sqlpp11/core/clause/cte.h>

namespace sqlpp
{
  template <typename... Expressions>
  struct with_data_t
  {
    using _is_recursive = logic::any<Expressions::_is_recursive...>;

    with_data_t(Expressions... expressions) : _expressions(expressions...)
    {
    }

    with_data_t(const with_data_t&) = default;
    with_data_t(with_data_t&&) = default;
    with_data_t& operator=(const with_data_t&) = default;
    with_data_t& operator=(with_data_t&&) = default;
    ~with_data_t() = default;

    std::tuple<Expressions...> _expressions;
  };

  template <typename... Expressions>
  struct with_t
  {
    using _traits = make_traits<no_value_t, tag::is_with>;
    using _nodes = detail::type_vector<>;
    using _provided_ctes =
        detail::make_joined_set_t<required_ctes_of<Expressions>...>;  // WITH provides common table expressions
    using _parameters = detail::type_vector_cat_t<parameters_of<Expressions>...>;

    using _data_t = with_data_t<Expressions...>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      // FIXME: Need real checks here
      using _consistency_check = consistent_t;
    };
  };

  struct no_with_t
  {
    using _traits = make_traits<no_value_t, tag::is_with>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

    template <typename Policies>
    struct _base_t
    {
      _base_t() = default;
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;


      using _consistency_check = consistent_t;
    };
  };

  template <typename... Expressions>
  struct blank_with_t
  {
    with_data_t<Expressions...> _data;

    template <typename Statement>
    auto operator()(Statement statement)
        -> new_statement_t<consistent_t, typename Statement::_policies_t, no_with_t, with_t<Expressions...>>
    {
#warning: check that no cte refers to any of the ctes to the right
      return {statement, _data};
    }
  };

  // Interpreters
  template <typename Context, typename... Expressions>
  auto to_sql_string(Context& context, const with_data_t<Expressions...>& t) -> std::string
  {
    using T = with_data_t<Expressions...>;
#warning : If there is a recursive CTE, add a "RECURSIVE" here
    return std::string("WITH ") + (T::_is_recursive::value ? "RECURSIVE " : "") +
           tuple_to_sql_string(context, t._expressions, tuple_operand{", "}) + " ";
  }

  template <typename Context, typename... Expressions>
  auto to_sql_string(Context& context, const blank_with_t<Expressions...>& t) -> std::string
  {
    return to_sql_string(context, t._data);
  }

  template <typename... Expressions>
  auto with(Expressions... cte) -> blank_with_t<Expressions...>
  {
    static_assert(logic::all<is_cte<Expressions>::value...>::value,
                  "at least one expression in with is not a common table expression");
#warning: Need to test that cte_t::as yields a cte_ref and that cte_ref is not a cte
    return {{cte...}};
  }
}  // namespace sqlpp

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
  template <typename... Ctes>
  struct with_t
  {
    with_t(Ctes... expressions) : _expressions(expressions...)
    {
    }

    with_t(const with_t&) = default;
    with_t(with_t&&) = default;
    with_t& operator=(const with_t&) = default;
    with_t& operator=(with_t&&) = default;
    ~with_t() = default;

    std::tuple<Ctes...> _expressions;
  };

  template <typename... Ctes>
  struct is_clause<with_t<Ctes...>> : public std::true_type
  {
  };

  template <typename Statement, typename... Ctes>
  struct consistency_check<Statement, with_t<Ctes...>>
  {
      // FIXME: Need real checks here
    using type = consistent_t;
  };

  // Note: No nodes are exposed directly. Nothing should be leaked from CTEs by accident.

  template <typename... Ctes>
  struct provided_ctes_of<with_t<Ctes...>>
  {
    using type = detail::make_joined_set_t<provided_ctes_of_t<Ctes>...>;
  };

  template <typename... Ctes>
  struct provided_static_ctes_of<with_t<Ctes...>>
  {
    using type = detail::make_joined_set_t<provided_static_ctes_of_t<Ctes>...>;
  };

  template <typename... Ctes>
  struct parameters_of<with_t<Ctes...>>
  {
    using type = detail::type_vector_cat_t<parameters_of_t<Ctes>...>;
  };

  struct no_with_t
  {

    // Data
    using _data_t = no_data_t;

    template <typename Statement>
    struct _base_t
    {
      _base_t() = default;
      template<typename OtherStatement>
      _base_t(_base_t<OtherStatement> base) : _data{std::move(base._data)} {}
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;
    };
  };

  template <typename Statement>
  struct consistency_check<Statement, no_with_t>
  {
    using type = consistent_t;
  };

  template <typename... Ctes>
  struct blank_with_t
  {
    with_t<Ctes...> _with_clause;

    template <typename Statement>
    auto operator()(Statement statement)
        -> decltype(new_statement(statement, _with_clause))
    {
      return new_statement(statement, _with_clause);
    }
  };

  // Interpreters
  template <typename Context, typename... Ctes>
  auto to_sql_string(Context& context, const with_t<Ctes...>& t) -> std::string
  {
    static constexpr bool _is_recursive = logic::any<is_recursive_cte<Ctes>::value...>::value;

    return std::string("WITH ") + (_is_recursive ? "RECURSIVE " : "") +
           tuple_to_sql_string(context, t._expressions, tuple_operand{", "}) + " ";
  }

  template <typename Context, typename... Ctes>
  auto to_sql_string(Context& context, const blank_with_t<Ctes...>& t) -> std::string
  {
    return to_sql_string(context, t._data);
  }

  // CTEs can depend on CTEs defined before (in the same query).
  // `have_correct_dependencies` checks that by walking the CTEs from left to right and building a type vector that
  // contains the CTE it already has looked at.
  template <typename AllowedCTEs, typename AllowedStaticCTEs, typename... CTEs>
    struct have_correct_dependencies_impl;

  template <typename AllowedCTEs, typename AllowedStaticCTEs>
    struct have_correct_dependencies_impl<AllowedCTEs, AllowedStaticCTEs>: public std::true_type {};

  template <typename AllowedCTEs, typename AllowedStaticCTEs, typename CTE, typename... Rest>
    struct have_correct_dependencies_impl<AllowedCTEs, AllowedStaticCTEs, CTE, Rest...>
    {
      using allowed_ctes = detail::make_joined_set_t<AllowedCTEs, provided_ctes_of_t<CTE>>;
      using allowed_static_ctes = detail::make_joined_set_t<AllowedStaticCTEs, provided_static_ctes_of_t<CTE>>;
      static constexpr bool value =
          allowed_ctes::contains_all(required_ctes_of_t<CTE>{}) and
          allowed_static_ctes::contains_all(required_static_ctes_of_t<CTE>{}) and
          have_correct_dependencies_impl<allowed_ctes, allowed_static_ctes, Rest...>::value;
    };

  template <typename... CTEs>
  struct have_correct_dependencies
  {
    static constexpr bool value = have_correct_dependencies_impl<detail::type_set<>, detail::type_set<>, CTEs...>::value;
  };
  template <typename... Ctes>
  auto with(Ctes... cte) -> blank_with_t<Ctes...>
  {
    static_assert(logic::all<is_cte<Ctes>::value...>::value,
                  "at least one expression in with is not a common table expression");
    static_assert(have_correct_dependencies<Ctes...>::value, "at least one CTE depends on another CTE that is not defined (yet)");
    static_assert(detail::are_unique<make_char_sequence_t<Ctes>...>::value, "CTEs in with need to have unique names");
    return {{cte...}};
  }
}  // namespace sqlpp

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

#include <sqlpp11/core/noop.h>
#include <sqlpp11/core/hidden.h>
#include <sqlpp11/core/database/parameter_list.h>
#include <sqlpp11/core/query/statement_constructor_arg.h>
#include <sqlpp11/core/database/prepared_select.h>
#include <sqlpp11/core/basic/value.h>
#include <sqlpp11/core/result.h>
#include <sqlpp11/core/to_sql_string.h>
#include <sqlpp11/core/query/statement_fwd.h>
#include <sqlpp11/core/clause/clause_base.h>
#include <sqlpp11/core/result_type_provider.h>
#include <sqlpp11/core/detail/get_first.h>
#include <sqlpp11/core/detail/get_last.h>
#include <sqlpp11/core/detail/pick_arg.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_no_unknown_ctes_t,
      "one clause requires common table expressions which are otherwise not known in the statement");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_no_unknown_tables_t,
                               "one clause requires tables which are otherwise not known in the statement");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_no_parameters_t,
                               "cannot run statements with parameters directly, use prepare instead");

  template <typename... Clauses>
    using result_methods_t = typename result_type_provider_t<Clauses...>::template _result_methods_t<statement_t<Clauses...>>;

  template <typename... Clauses>
  struct statement_t : public clause_base<Clauses, statement_t<Clauses...>>...,
                       public result_methods_t<Clauses...>
  {
      using _all_required_ctes = detail::make_joined_set_t<required_ctes_of_t<Clauses>...>;
      using _all_provided_ctes = detail::make_joined_set_t<provided_ctes_of_t<Clauses>...>;
      using _all_required_static_ctes = detail::make_joined_set_t<required_static_ctes_of_t<Clauses>...>;
      using _all_provided_static_ctes = detail::make_joined_set_t<provided_static_ctes_of_t<Clauses>...>;
      using _all_required_tables = detail::make_joined_set_t<required_tables_of_t<Clauses>...>;
      using _all_provided_tables = detail::make_joined_set_t<provided_tables_of_t<Clauses>...>;
      using _all_provided_optional_tables = detail::make_joined_set_t<provided_optional_tables_of_t<Clauses>...>;
      using _all_provided_aggregates = detail::make_joined_set_t<known_aggregate_columns_of_t<Clauses>...>;

      using _required_tables_of = detail::make_difference_set_t<_all_required_tables, _all_provided_tables>;
      using _required_ctes_of = detail::make_difference_set_t<_all_required_ctes, _all_provided_ctes>;
      using _required_static_ctes_of = detail::make_difference_set_t<_all_required_static_ctes, _all_provided_static_ctes>;

      using _parameters = detail::type_vector_cat_t<parameters_of_t<Clauses>...>;

      template <typename Expression>
      static constexpr bool _no_unknown_tables = _all_provided_tables::contains_all(required_tables_of_t<Expression>{});

      using _result_type_provider = detail::get_last_if_t<is_result_clause, noop, Clauses...>;

      using _cte_check =
          typename std::conditional<_required_ctes_of::empty(), consistent_t, assert_no_unknown_ctes_t>::type;
      using _table_check =
          typename std::conditional<_required_tables_of::empty(), consistent_t, assert_no_unknown_tables_t>::type;
      using _parameter_check = typename std::
          conditional<_parameters::empty(), consistent_t, assert_no_parameters_t>::type;

    using _provided_optional_tables = _all_provided_optional_tables;

    // Constructors
    statement_t() = default;

    template <typename... Fragments>
    statement_t(statement_constructor_arg<Fragments...> arg) : clause_base<Clauses, statement_t>(arg)...
    {
    }

    statement_t(const statement_t& r) = default;
    statement_t(statement_t&& r) = default;
    statement_t& operator=(const statement_t& r) = default;
    statement_t& operator=(statement_t&& r) = default;
    ~statement_t() = default;

    static constexpr size_t _get_static_no_of_parameters()
    {
      return parameters_of_t<statement_t>::size();
    }

    size_t _get_no_of_parameters() const
    {
      return _get_static_no_of_parameters();
    }

      // A select can be used as a pseudo table if
      //   - at least one column is selected
      //   - the select is complete (leaks no table requirements or cte requirements)
      static constexpr bool _can_be_used_as_table()
      {
        return has_result_row<statement_t>::value and _required_tables_of::empty() and
               _required_ctes_of::empty();
      }

    template <typename Database>
    auto _run(Database& db) const -> decltype(std::declval<result_methods_t<Clauses...>>()._run(db))
    {
      statement_run_check_t<statement_t>::verify();
      return result_methods_t<Clauses...>::_run(db);
    }

    template <typename Database>
    auto _prepare(Database& db) const -> decltype(std::declval<result_methods_t<Clauses...>>()._prepare(db))
    {
      statement_prepare_check_t<statement_t>::verify();
      return result_methods_t<Clauses...>::_prepare(db);
    }
  };

  template<typename... Clauses>
    struct is_statement<statement_t<Clauses...>> : public std::true_type {};

  template<typename... Clauses>
    struct is_where_required<statement_t<Clauses...>> {
      static constexpr bool value = statement_t<Clauses...>::_all_provided_tables::size() > 0;
    };

  template <typename... Clauses>
  struct is_result_clause<statement_t<Clauses...>>
  {
    static constexpr bool value = not std::is_same<noop, typename statement_t<Clauses...>::_result_type_provider>::value;
  };

  template <typename... Clauses>
  struct value_type_of<statement_t<Clauses...>>
  {
    using type = typename std::conditional<statement_consistency_check_t<statement_t<Clauses...>>::value,
                                           value_type_of_t<result_type_provider_t<Clauses...>>,
                                           no_value_t>::type;
  };

  template <typename... Clauses>
  struct nodes_of<statement_t<Clauses...>>
  {
    // statements explicitly do not expose any nodes as most recursive traits should not traverse into sub queries, e.g.
    //   - contains_aggregates
    //   - known_aggregate_columns_of
    using type = typename detail::type_vector<>;
  };

  template<typename... Clauses>
    struct required_insert_columns_of<statement_t<Clauses...>>
    {
      using type = detail::make_joined_set_t<required_insert_columns_of_t<Clauses>...>;
    };

  template <typename... Clauses>
  struct parameters_of<statement_t<Clauses...>>
  {
    using type = detail::type_vector_cat_t<parameters_of_t<Clauses>...>;
  };

  template <typename... Clauses>
  struct required_tables_of<statement_t<Clauses...>>
  {
    using type = typename statement_t<Clauses...>::_required_tables_of;
  };

  template <typename... Clauses>
  struct required_ctes_of<statement_t<Clauses...>>
  {
    using type = typename statement_t<Clauses...>::_required_ctes_of;
  };

  template <typename... Clauses>
  struct required_static_ctes_of<statement_t<Clauses...>>
  {
    using type = typename statement_t<Clauses...>::_required_static_ctes_of;
  };

  template <typename... Clauses>
  struct requires_parentheses<statement_t<Clauses...>> : public std::true_type {};

  template <typename... Clauses>
  struct statement_consistency_check<statement_t<Clauses...>>
  {
    using type = static_combined_check_t<consistency_check_t<statement_t<Clauses...>, Clauses>...>;
  };

  template <typename... Clauses>
  struct statement_prepare_check<statement_t<Clauses...>>
  {
    using type =
        static_combined_check_t<statement_consistency_check_t<statement_t<Clauses...>>,
                                static_combined_check_t<prepare_check_t<statement_t<Clauses...>, Clauses>...>,
                                typename statement_t<Clauses...>::_table_check,
                                typename statement_t<Clauses...>::_cte_check>;
  };

  template <typename... Clauses>
  struct statement_run_check<statement_t<Clauses...>>
  {
    using type = static_combined_check_t<statement_prepare_check_t<statement_t<Clauses...>>,
                                         static_combined_check_t<run_check_t<statement_t<Clauses...>, Clauses>...>,
                                         typename statement_t<Clauses...>::_parameter_check>;
  };

  template <typename OldClause, typename... Clauses, typename NewClause>
  auto new_statement(const clause_base<OldClause, statement_t<Clauses...>>& oldBase, NewClause newClause)
      -> statement_t<typename std::conditional<std::is_same<Clauses, OldClause>::value, NewClause, Clauses>::type...>
  {
    return statement_t<typename std::conditional<std::is_same<Clauses, OldClause>::value, NewClause, Clauses>::type...>{
        statement_constructor_arg(static_cast<const statement_t<Clauses...>&>(oldBase), newClause)};
  }

  template <typename T> struct core_statement;
  template <typename... Clauses>
    struct core_statement<detail::type_vector<Clauses...>> {
      using type = statement_t<Clauses...>;
    };

  template<typename... Clauses>
  using core_statement_t = typename core_statement<detail::copy_if_t<detail::type_vector<Clauses...>, is_clause>>::type;

  template <typename Statement>
  struct statement_has_unique_clauses;

  template <typename... Clauses>
  struct statement_has_unique_clauses<statement_t<Clauses...>> : public detail::are_unique<Clauses...>
  {
  };

  template <typename... LClauses, typename... RClauses>
  constexpr auto operator<<(statement_t<LClauses...> l, statement_t<RClauses...> r) -> core_statement_t<LClauses..., RClauses...>
  {
    using _core_statement = core_statement_t<LClauses..., RClauses...>;
    SQLPP_STATIC_ASSERT(statement_has_unique_clauses<_core_statement>::value, "statements must contain unique clauses only");
    return _core_statement(statement_constructor_arg(std::move(l), std::move(r)));
  }

  template <typename... LClauses, typename Clause>
  constexpr auto operator<<(statement_t<LClauses...> l, Clause r) -> core_statement_t<LClauses..., Clause>
  {
    SQLPP_STATIC_ASSERT(is_clause<Clause>::value,
                        "statement_t::operator<< requires statements or clauses as parameters");
    using _core_statement = core_statement_t<LClauses..., Clause>;
    SQLPP_STATIC_ASSERT(statement_has_unique_clauses<_core_statement>::value, "statements must contain unique clauses only");
    return _core_statement(statement_constructor_arg(std::move(l), std::move(r)));
  }

  template <typename Context, typename... Clauses>
  auto to_sql_string(Context& context, const statement_t<Clauses...>& t) -> std::string
  {
    auto result = std::string{};
    using swallow = int[];
    (void)swallow{
        0, (result += to_sql_string(
                context, static_cast<const clause_base<Clauses, statement_t<Clauses...>>&>(t)._data),
            0)...};

    return result;
  }

}  // namespace sqlpp

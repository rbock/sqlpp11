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
#include <sqlpp11/core/database/parameter_list.h>
#include <sqlpp11/core/query/statement_constructor_arg.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/database/prepared_select.h>
#include <sqlpp11/core/result.h>
#include <sqlpp11/core/to_sql_string.h>
#include <sqlpp11/core/query/statement_fwd.h>
#include <sqlpp11/core/clause/clause_base.h>

#include <sqlpp11/core/detail/get_first.h>
#include <sqlpp11/core/detail/get_last.h>
#include <sqlpp11/core/detail/pick_arg.h>

namespace sqlpp
{
  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_ctes_t,
      "one clause requires common table expressions which are otherwise not known in the statement");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_no_unknown_tables_t,
                               "one clause requires tables which are otherwise not known in the statement");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_no_parameters_t,
                               "cannot run statements with parameters directly, use prepare instead");

  template <typename... Policies>
    using result_type_provider_t = detail::get_last_if_t<is_result_clause, noop, Policies...>;

  template <typename... Policies>
    using result_methods_t = typename result_type_provider_t<Policies...>::template _result_methods_t<statement_t<Policies...>>;

  template <typename... Policies>
  struct statement_t : public Policies::template _base_t<statement_t<Policies...>>...,
                       public result_methods_t<Policies...>
  {
      using _all_required_ctes = detail::make_joined_set_t<required_ctes_of_t<Policies>...>;
      using _all_provided_ctes = detail::make_joined_set_t<provided_ctes_of_t<Policies>...>;
      using _all_required_static_ctes = detail::make_joined_set_t<required_static_ctes_of_t<Policies>...>;
      using _all_provided_static_ctes = detail::make_joined_set_t<provided_static_ctes_of_t<Policies>...>;
      using _all_required_tables = detail::make_joined_set_t<required_tables_of_t<Policies>...>;
      using _all_provided_tables = detail::make_joined_set_t<provided_tables_of_t<Policies>...>;
      using _all_provided_optional_tables = detail::make_joined_set_t<provided_optional_tables_of_t<Policies>...>;
      using _all_provided_aggregates = detail::make_joined_set_t<known_aggregate_columns_of_t<Policies>...>;

      using _required_tables_of = detail::make_difference_set_t<_all_required_tables, _all_provided_tables>;
      using _required_ctes_of = detail::make_difference_set_t<_all_required_ctes, _all_provided_ctes>;
      using _required_static_ctes_of = detail::make_difference_set_t<_all_required_static_ctes, _all_provided_static_ctes>;

      using _parameters = detail::type_vector_cat_t<parameters_of_t<Policies>...>;

      template <typename Expression>
      static constexpr bool _no_unknown_tables = _all_provided_tables::contains_all(required_tables_of_t<Expression>{});

      using _result_type_provider = detail::get_last_if_t<is_result_clause, noop, Policies...>;

      struct _result_methods_t : public result_methods_t<Policies...>
      {
      };

      using _value_type =
          typename std::conditional<logic::any<is_missing_t<Policies>::value...>::value,
                                    no_value_t,  // if a required statement part is missing (e.g. columns in a select),
                                                 // then the statement cannot be used as a value
                                    value_type_of_t<_result_type_provider>>::type;

      using _cte_check =
          typename std::conditional<_required_ctes_of::empty(), consistent_t, assert_no_unknown_ctes_t>::type;
      using _table_check =
          typename std::conditional<_required_tables_of::empty(), consistent_t, assert_no_unknown_tables_t>::type;
      using _parameter_check = typename std::
          conditional<_parameters::empty(), consistent_t, assert_no_parameters_t>::type;

    using _run_check = detail::get_first_if<is_inconsistent_t,
                                            consistent_t,
                                            _parameter_check,
                                            _cte_check,
                             statement_consistency_check_t<statement_t>,
                                            _table_check>;

    using _prepare_check = detail::get_first_if<is_inconsistent_t,
                                                consistent_t,
                                                _cte_check,
                             statement_consistency_check_t<statement_t>,
                                                _table_check>;

    using _name_tag_of = name_tag_of<_result_type_provider>;
    using _provided_optional_tables = _all_provided_optional_tables;

    // Constructors
    statement_t() = default;

    // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
    //	template <typename Statement, typename Term>
    //	statement_t(Statement statement, Term term)
    //		: Policies::template _base_t<_policies_t>{typename Policies::template _impl_t<_policies_t>{
    //		detail::pick_arg<typename Policies::template _base_t<_policies_t>>(statement, term)}}...
    //	{
    //	}
    template <typename Statement, typename Term>
    statement_t(Statement statement, Term term)
        : Policies::template _base_t<statement_t>(
              detail::pick_arg<Policies>(statement, term))...
    {
    }

    template <typename... Fragments>
    statement_t(statement_constructor_arg<Fragments...> arg) : Policies::template _base_t<statement_t>(arg)...
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
    auto _run(Database& db) const -> decltype(std::declval<_result_methods_t>()._run(db))
    {
      _run_check::verify();
      return _result_methods_t::_run(db);
    }

    template <typename Database>
    auto _prepare(Database& db) const -> decltype(std::declval<_result_methods_t>()._prepare(db))
    {
      _prepare_check::verify();
      return _result_methods_t::_prepare(db);
    }
  };

  template<typename... Policies>
    struct statement_consistency_check<statement_t<Policies...>> {
        using type = detail::get_first_if<is_inconsistent_t,
                             consistent_t,
                             consistency_check_t<statement_t<Policies...>, Policies>...,
                             typename statement_t<Policies...>::_table_check>;
    };

  template<typename... Policies>
    struct is_statement<statement_t<Policies...>> : public std::true_type {};

  template<typename... Policies>
    struct is_where_required<statement_t<Policies...>> {
      static constexpr bool value = statement_t<Policies...>::_all_provided_tables::size() > 0;
    };

  template <typename... Policies>
  struct is_result_clause<statement_t<Policies...>>
  {
    static constexpr bool value = not std::is_same<noop, typename statement_t<Policies...>::_result_type_provider>::value;
  };

  template<typename... Policies>
    struct value_type_of<statement_t<Policies...>>
  {
    using type = typename statement_t<Policies...>::_value_type;
  };

  template<typename... Policies>
    struct name_tag_of<statement_t<Policies...>> : public statement_t<Policies...>::_name_tag_of {};

  template <typename... Policies>
  struct nodes_of<statement_t<Policies...>>
  {
    // statements explicitly do not expose any nodes as most recursive traits should not traverse into sub queries, e.g.
    //   - contains_aggregates
    //   - known_aggregate_columns_of
    using type = typename detail::type_vector<>;
  };

  template<typename... Policies>
    struct required_insert_columns_of<statement_t<Policies...>>
    {
      using type = detail::make_joined_set_t<required_insert_columns_of_t<Policies>...>;
    };

  template <typename... Policies>
  struct parameters_of<statement_t<Policies...>>
  {
    using type = detail::type_vector_cat_t<parameters_of_t<Policies>...>;
  };

  template <typename... Policies>
  struct required_tables_of<statement_t<Policies...>>
  {
    using type = typename statement_t<Policies...>::_required_tables_of;
  };

  template <typename... Policies>
  struct required_ctes_of<statement_t<Policies...>>
  {
    using type = typename statement_t<Policies...>::_required_ctes_of;
  };

  template <typename... Policies>
  struct required_static_ctes_of<statement_t<Policies...>>
  {
    using type = typename statement_t<Policies...>::_required_static_ctes_of;
  };

  template <typename... Policies>
  struct requires_parentheses<statement_t<Policies...>> : public std::true_type {};

#warning: move clauses to use clause_base and new_statement!
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

  template <typename... LClauses, typename... RClauses>
  constexpr auto operator<<(statement_t<LClauses...> l, statement_t<RClauses...> r) -> core_statement_t<LClauses..., RClauses...>
  {
#warning: Do we need something like this?
    //SQLPP_STATIC_ASSERT((detail::are_unique<LClauses..., RClauses...>::value), "statements must contain unique clauses only");
    return core_statement_t<LClauses..., RClauses...>(statement_constructor_arg(l, r));
  }

  template <typename Context, typename... Policies>
  auto to_sql_string(Context& context, const statement_t<Policies...>& t) -> std::string
  {
    auto result = std::string{};
    using swallow = int[];
    (void)swallow{
        0, (result += to_sql_string(
                context, static_cast<const typename Policies::template _base_t<statement_t<Policies...>>&>(t)._data),
            0)...};

    return result;
  }

  template <typename NameData, typename Tag = tag::is_noop>
  struct statement_name_t
  {
    using _traits = make_traits<no_value_t, Tag>;

    using _data_t = NameData;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t() = default;
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;
    };
  };

}  // namespace sqlpp

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

#ifndef SQLPP_STATEMENT_H
#define SQLPP_STATEMENT_H

#include <sqlpp11/noop.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/prepared_select.h>
#include <sqlpp11/result.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/serializer.h>

#include <sqlpp11/detail/get_first.h>
#include <sqlpp11/detail/get_last.h>
#include <sqlpp11/detail/pick_arg.h>

namespace sqlpp
{
  template <typename Db, typename... Policies>
  struct statement_t;

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_ctes_t,
      "one clause requires common table expressions which are otherwise not known in the statement");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_no_unknown_tables_t,
                               "one clause requires tables which are otherwise not known in the statement");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_no_parameters_t,
                               "cannot run statements with parameters directly, use prepare instead");

  namespace detail
  {
    template <typename Db = void, typename... Policies>
    struct statement_policies_t
    {
      using _database_t = Db;
      using _statement_t = statement_t<Db, Policies...>;

      template <typename Needle, typename Replacement>
      struct _policies_update_t
      {
        static_assert(detail::is_element_of<Needle, make_type_set_t<Policies...>>::value,
                      "policies update for non-policy class detected");
        using type = statement_t<Db, policy_update_t<Policies, Needle, Replacement>...>;
      };

      template <typename Needle, typename Replacement>
      using _new_statement_t = typename _policies_update_t<Needle, Replacement>::type;

      using _all_required_ctes = detail::make_joined_set_t<required_ctes_of<Policies>...>;
      using _all_provided_ctes = detail::make_joined_set_t<provided_ctes_of<Policies>...>;
      using _all_required_tables = detail::make_joined_set_t<required_tables_of<Policies>...>;
      using _all_provided_tables = detail::make_joined_set_t<provided_tables_of<Policies>...>;
      using _all_provided_outer_tables = detail::make_joined_set_t<provided_outer_tables_of<Policies>...>;
      using _all_provided_aggregates = detail::make_joined_set_t<provided_aggregates_of<Policies>...>;

      template <typename Expression>
      using _no_unknown_tables = detail::is_subset_of<required_tables_of<Expression>, _all_provided_tables>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2086629
      //	  template <typename... Expressions>
      //      using _no_unknown_aggregates =
      //          logic::any_t<_all_provided_aggregates::size::value == 0,
      //                       logic::all_t<is_aggregate_expression_t<_all_provided_aggregates,
      //                       Expressions>::value...>::value>;
      template <typename... Expressions>
      using _no_unknown_aggregates =
          logic::any_t<_all_provided_aggregates::size::value == 0,
                       logic::all_t<detail::is_aggregate_expression_impl<_all_provided_aggregates,
                                                                         Expressions>::type::value...>::value>;

      template <typename... Expressions>
      using _no_non_aggregates = logic::any_t<logic::all_t<
          detail::is_aggregate_expression_impl<_all_provided_aggregates, Expressions>::type::value...>::value>;

      template <template <typename> class Predicate>
      using any_t = logic::any_t<Predicate<Policies>::value...>;

      // The tables not covered by the from.
      using _required_tables = detail::make_difference_set_t<_all_required_tables, _all_provided_tables>;

      // The common table expressions not covered by the with.
      using _required_ctes = detail::make_difference_set_t<_all_required_ctes, _all_provided_ctes>;

      using _result_type_provider = detail::get_last_if<is_return_value_t, noop, Policies...>;

      struct _result_methods_t : public _result_type_provider::template _result_methods_t<_statement_t>
      {
      };

      // A select can be used as a pseudo table if
      //   - at least one column is selected
      //   - the select is complete (leaks no table requirements or cte requirements)
      static constexpr bool _can_be_used_as_table()
      {
        return has_result_row_t<_statement_t>::value and _required_tables::size::value == 0 and
               _required_ctes::size::value == 0;
      }

      using _value_type =
          typename std::conditional<logic::none_t<is_missing_t<Policies>::value...>::value,
                                    value_type_of<_result_type_provider>,
                                    no_value_t  // if a required statement part is missing (e.g. columns in a select),
                                                // then the statement cannot be used as a value
                                    >::type;

      using _traits =
          make_traits<_value_type, tag_if<tag::is_expression, not std::is_same<_value_type, no_value_t>::value>>;

      using _nodes = detail::type_vector<>;
      using _can_be_null = logic::any_t<can_be_null_t<_result_type_provider>::value,
                                        detail::make_intersect_set_t<required_tables_of<_result_type_provider>,
                                                                     _all_provided_outer_tables>::size::value != 0>;
      using _parameters = detail::type_vector_cat_t<parameters_of<Policies>...>;
      // required_tables and _required_ctes are defined above

      using _cte_check =
          typename std::conditional<_required_ctes::size::value == 0, consistent_t, assert_no_unknown_ctes_t>::type;
      using _table_check =
          typename std::conditional<_required_tables::size::value == 0, consistent_t, assert_no_unknown_tables_t>::type;
      using _parameter_check = typename std::
          conditional<detail::type_vector_size<_parameters>::value == 0, consistent_t, assert_no_parameters_t>::type;
    };
  }

  template <typename Db, typename... Policies>
  struct statement_t : public Policies::template _base_t<detail::statement_policies_t<Db, Policies...>>...,
                       public expression_operators<statement_t<Db, Policies...>,
                                                   value_type_of<detail::statement_policies_t<Db, Policies...>>>,
                       public detail::statement_policies_t<Db, Policies...>::_result_methods_t
  {
    using _policies_t = typename detail::statement_policies_t<Db, Policies...>;

    using _consistency_check =
        detail::get_first_if<is_inconsistent_t,
                             consistent_t,
                             typename Policies::template _base_t<_policies_t>::_consistency_check...,
                             typename _policies_t::_table_check>;

    using _run_check = detail::get_first_if<is_inconsistent_t,
                                            consistent_t,
                                            typename _policies_t::_parameter_check,
                                            typename _policies_t::_cte_check,
                                            typename Policies::template _base_t<_policies_t>::_consistency_check...,
                                            typename _policies_t::_table_check>;

    using _prepare_check = detail::get_first_if<is_inconsistent_t,
                                                consistent_t,
                                                typename _policies_t::_cte_check,
                                                typename Policies::template _base_t<_policies_t>::_consistency_check...,
                                                typename _policies_t::_table_check>;

    using _result_type_provider = typename _policies_t::_result_type_provider;
    template <typename Composite>
    using _result_methods_t = typename _result_type_provider::template _result_methods_t<Composite>;

    using _traits =
        make_traits<value_type_of<_policies_t>,
                    tag::is_statement,
                    tag_if<tag::is_select, logic::any_t<is_select_t<Policies>::value...>::value>,
                    tag_if<tag::is_expression, is_expression_t<_policies_t>::value>,
                    tag_if<tag::is_selectable, is_expression_t<_policies_t>::value>,
                    tag_if<tag::is_return_value, logic::none_t<is_noop_t<_result_type_provider>::value>::value>,
                    tag::requires_braces>;
    using _nodes = detail::type_vector<_policies_t>;
    using _used_outer_tables = typename _policies_t::_all_provided_outer_tables;

    using _alias_t = typename _result_type_provider::_alias_t;

    // Constructors
    statement_t()
    {
    }

    // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
    //	template <typename Statement, typename Term>
    //	statement_t(Statement statement, Term term)
    //		: Policies::template _base_t<_policies_t>{typename Policies::template _impl_t<_policies_t>{
    //		detail::pick_arg<typename Policies::template _base_t<_policies_t>>(statement, term)}}...
    //	{
    //	}
    template <typename Statement, typename Term>
    statement_t(Statement statement, Term term)
        : Policies::template _base_t<_policies_t>(typename Policies::template _impl_t<_policies_t>(
              detail::pick_arg<typename Policies::template _base_t<_policies_t>>(statement, term)))...
    {
    }

    statement_t(const statement_t& r) = default;
    statement_t(statement_t&& r) = default;
    statement_t& operator=(const statement_t& r) = default;
    statement_t& operator=(statement_t&& r) = default;
    ~statement_t() = default;

    static constexpr size_t _get_static_no_of_parameters()
    {
      return detail::type_vector_size<parameters_of<statement_t>>::value;
    }

    size_t _get_no_of_parameters() const
    {
      return _get_static_no_of_parameters();
    }

    static constexpr bool _can_be_used_as_table()
    {
      return _policies_t::_can_be_used_as_table();
    }

    template <typename Database>
    auto _run(Database& db) const -> decltype(std::declval<_result_methods_t<statement_t>>()._run(db))
    {
      _run_check{};  // FIXME: Dispatch?
      return _result_methods_t<statement_t>::_run(db);
    }

    template <typename Database>
    auto _prepare(Database& db) const -> decltype(std::declval<_result_methods_t<statement_t>>()._prepare(db))
    {
      _prepare_check{};  // FIXME: Dispatch?
      return _result_methods_t<statement_t>::_prepare(db);
    }
  };

  template <typename Context, typename Database, typename... Policies>
  struct serializer_t<Context, statement_t<Database, Policies...>>
  {
    using P = detail::statement_policies_t<Database, Policies...>;
    using _serialize_check = serialize_check_of<Context, typename Policies::template _base_t<P>::_data_t...>;
    using T = statement_t<Database, Policies...>;

    static Context& _(const T& t, Context& context)
    {
      using swallow = int[];
      (void)swallow{0,
                    (serialize(static_cast<const typename Policies::template _base_t<P>&>(t)()._data, context), 0)...};

      return context;
    }
  };

  template <typename NameData, typename Tag = tag::is_noop>
  struct statement_name_t
  {
    using _traits = make_traits<no_value_t, Tag>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = NameData;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = NameData;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      template <typename... Args>
      _base_t(Args&&... args) : statement_name{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> statement_name;
      _impl_t<Policies>& operator()()
      {
        return statement_name;
      }
      const _impl_t<Policies>& operator()() const
      {
        return statement_name;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.statement_name)
      {
        return t.statement_name;
      }

      using _consistency_check = consistent_t;
    };
  };
}

#endif

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

#include <sqlpp11/core/basic/enable_join.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/query/result_row.h>
#include <sqlpp11/core/clause/select_flags.h>
#include <sqlpp11/core/query/statement_fwd.h>
#include <sqlpp11/core/basic/table_ref.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename Flag, typename Lhs, typename Rhs>
  struct cte_union_t
  {
    cte_union_t(Lhs lhs, Rhs rhs) : _lhs(lhs), _rhs(rhs)
    {
    }

    cte_union_t(const cte_union_t&) = default;
    cte_union_t(cte_union_t&&) = default;
    cte_union_t& operator=(const cte_union_t&) = default;
    cte_union_t& operator=(cte_union_t&&) = default;
    ~cte_union_t() = default;

    Lhs _lhs;
    Rhs _rhs;
  };

#warning: need to test nodes of union!
  template <typename Flag, typename Lhs, typename Rhs>
  struct nodes_of<cte_union_t<Flag, Lhs, Rhs>>
  {
    using type = detail::type_vector<Lhs, Rhs>;
  };

  // Interpreters
  template <typename Context, typename Flag, typename Lhs, typename Rhs>
  auto to_sql_string(Context& context, const cte_union_t<Flag, Lhs, Rhs>& t) -> std::string
  {
    return to_sql_string(context, t._lhs) + " UNION " + to_sql_string(context, Flag{}) + to_sql_string(context, t._rhs);
  }

  template <typename Context, typename Flag, typename Lhs, typename Rhs>
  auto to_sql_string(Context& context, const cte_union_t<Flag, Lhs, dynamic_t<Rhs>>& t) -> std::string
  {
    if (t._rhs._condition){
    return to_sql_string(context, t._lhs) + " UNION " + to_sql_string(context, Flag{}) + to_sql_string(context, t._rhs._expr);
    }
    return to_sql_string(context, t._lhs);
  }

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  struct cte_t;

  template <typename NameTagProvider>
  struct cte_ref_t;

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  struct table_ref<cte_t<NameTagProvider, Statement, FieldSpecs...>>
  {
    using type = cte_ref_t<NameTagProvider>;
  };

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  struct table_ref<dynamic_t<cte_t<NameTagProvider, Statement, FieldSpecs...>>>
  {
    using type = dynamic_t<cte_ref_t<NameTagProvider>>;
  };

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  auto make_table_ref(cte_t<NameTagProvider, Statement, FieldSpecs...> /* unused */) -> cte_ref_t<NameTagProvider>
  {
    return {};
  }

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  auto make_table_ref(dynamic_t<cte_t<NameTagProvider, Statement, FieldSpecs...>> dyn_cte) -> dynamic_t<cte_ref_t<NameTagProvider>>
  {
    return dynamic_t<cte_ref_t<NameTagProvider>>{dyn_cte._condition, cte_ref_t<NameTagProvider>{}};
  }

  // make_cte translates the `Statement` into field_specs...
  // The field_specs are required to add column data members to the CTE.
  template <typename NameTagProvider, typename Statement, typename ResultRow>
  struct make_cte;

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  struct make_cte<NameTagProvider, Statement, result_row_t<void, FieldSpecs...>>
  {
    using type = cte_t<NameTagProvider, Statement, FieldSpecs...>;
  };

  template <typename NameTagProvider, typename Statement>
  using make_cte_t = typename make_cte<NameTagProvider, Statement, get_result_row_t<Statement>>::type;

  template <typename Check, typename Union>
  struct union_cte_impl
  {
    using type = Check;
  };

  template <typename Union>
  struct union_cte_impl<consistent_t, Union>
  {
    using type = Union;
  };

  template <typename Check, typename Union>
  using union_cte_impl_t = typename union_cte_impl<Check, Union>::type;

  SQLPP_PORTABLE_STATIC_ASSERT(assert_cte_union_args_are_statements_t, "argument for union() must be a statement");
  template <typename T>
  struct check_cte_union
  {
    using type = static_combined_check_t<
        static_check_t<is_statement_t<T>::value, assert_cte_union_args_are_statements_t>>;
  };
  template <typename T>
  using check_cte_union_t = typename check_cte_union<remove_dynamic_t<T>>::type;

  // cte_member is a helper to add column data members to `cte_t`.
  template <typename NameTagProvider, typename FieldSpec>
  struct cte_member
  {
    using type = member_t<FieldSpec, column_t<cte_ref_t<NameTagProvider>, FieldSpec>>;
  };

#warning: Need to document that you need to be a bit careful with aliased CTEs as we use cte_ref in columns, from, and join.
  template <typename NameTagProvider, typename NewNameTagProvider, typename... FieldSpecs>
  struct cte_as_t : public cte_member<NewNameTagProvider, FieldSpecs>::type...,
                 public enable_join<cte_as_t<NameTagProvider, NewNameTagProvider, FieldSpecs...>>
  {
    using _column_tuple_t = std::tuple<column_t<cte_ref_t<NewNameTagProvider>, FieldSpecs>...>;
  };

  template <typename NameTagProvider, typename NewNameTagProvider, typename... ColumnSpecs>
  struct is_table<cte_as_t<NameTagProvider, NewNameTagProvider, ColumnSpecs...>> : public std::true_type
  {
  };

  template <typename NameTagProvider, typename NewNameTagProvider, typename... ColumnSpecs>
  struct name_tag_of<cte_as_t<NameTagProvider, NewNameTagProvider, ColumnSpecs...>> : public name_tag_of<NewNameTagProvider>
  {
  };

  template <typename NameTagProvider, typename NewNameTagProvider, typename... ColumnSpecs>
    struct provided_tables_of<cte_as_t<NameTagProvider, NewNameTagProvider, ColumnSpecs...>> 
    {
      using type = sqlpp::detail::type_set<cte_ref_t<NewNameTagProvider>>;
    };

  template <typename NameTagProvider, typename NewNameTagProvider, typename... ColumnSpecs>
    struct required_ctes_of<cte_as_t<NameTagProvider, NewNameTagProvider, ColumnSpecs...>> 
    {
      // An aliased CTE requires the original CTE from the WITH clause.
      using type = sqlpp::detail::type_set<cte_ref_t<NameTagProvider>>;
    };

  template <typename NameTagProvider, typename NewNameTagProvider, typename... ColumnSpecs>
    struct required_static_ctes_of<cte_as_t<NameTagProvider, NewNameTagProvider, ColumnSpecs...>> : public required_ctes_of<cte_as_t<NameTagProvider, NewNameTagProvider, ColumnSpecs...>>{};

  template <typename Context, typename NameTagProvider, typename NewNameTagProvider, typename... ColumnSpecs>
  auto to_sql_string(Context& context, const cte_as_t<NameTagProvider, NewNameTagProvider, ColumnSpecs...>&) -> std::string
  {
    return name_to_sql_string(context, name_tag_of_t<NameTagProvider>::name) + " AS " + name_to_sql_string(context, name_tag_of_t<NewNameTagProvider>::name);
  }

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  struct cte_t : public cte_member<NameTagProvider, FieldSpecs>::type...,
                 public enable_join<cte_t<NameTagProvider, Statement, FieldSpecs...>>
  {
    using _column_tuple_t = std::tuple<column_t<cte_ref_t<NameTagProvider>, FieldSpecs>...>;

    using _result_row_t = result_row_t<void, FieldSpecs...>;

    template <typename NewNameTagProvider>
    constexpr auto as(const NewNameTagProvider& /*unused*/) const
        -> cte_as_t<NameTagProvider, NewNameTagProvider, FieldSpecs...>
    {
      return {};
    }

    template <typename Rhs>
    auto union_distinct(Rhs rhs) const
        -> union_cte_impl_t<check_cte_union_t<Rhs>,
                            cte_t<NameTagProvider, cte_union_t<distinct_t, Statement, Rhs>, FieldSpecs...>>
    {
      using _rhs = remove_dynamic_t<Rhs>;
      static_assert(is_statement_t<_rhs>::value, "argument of union call has to be a statement");
      static_assert(has_policy_t<_rhs, is_select_t>::value, "argument of union call has to be a select");
      static_assert(has_result_row<_rhs>::value, "argument of a clause/union.has to be a (complete) select statement");

      static_assert(std::is_same<_result_row_t, get_result_row_t<_rhs>>::value,
                    "both select statements in a clause/union.have to have the same result columns (type and name)");

      return _union_impl<distinct_t>(check_cte_union_t<Rhs>{}, rhs);
    }

    template <typename Rhs>
    auto union_all(Rhs rhs) const
        -> union_cte_impl_t<check_cte_union_t<Rhs>,
                            cte_t<NameTagProvider, cte_union_t<all_t, Statement, Rhs>, FieldSpecs...>>
    {
      using _rhs = remove_dynamic_t<Rhs>;
      static_assert(is_statement_t<_rhs>::value, "argument of union call has to be a statement");
      static_assert(has_policy_t<_rhs, is_select_t>::value, "argument of union call has to be a select");
      static_assert(has_result_row<_rhs>::value, "argument of a clause/union.has to be a (complete) select statement");

      static_assert(std::is_same<_result_row_t, get_result_row_t<_rhs>>::value,
                    "both select statements in a clause/union.have to have the same result columns (type and name)");

      return _union_impl<all_t>(check_cte_union_t<Rhs>{}, rhs);
    }

  private:
    template <typename Flag, typename Check, typename Rhs>
    auto _union_impl(Check, Rhs rhs) const -> inconsistent<Check>;

    template <typename Flag, typename Rhs>
    auto _union_impl(consistent_t /*unused*/, Rhs rhs) const
        -> cte_t<NameTagProvider, cte_union_t<Flag, Statement, Rhs>, FieldSpecs...>
    {
      return cte_union_t<Flag, Statement, Rhs>{_statement, rhs};
    }

  public:
    cte_t(Statement statement) : _statement(statement)
    {
    }
    cte_t(const cte_t&) = default;
    cte_t(cte_t&&) = default;
    cte_t& operator=(const cte_t&) = default;
    cte_t& operator=(cte_t&&) = default;
    ~cte_t() = default;

    Statement _statement;
  };

  // Note that `cte_t` is not a table, because `join` and `from` store `cte_ref_t`.
  template <typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  struct is_cte<cte_t<NameTagProvider, Statement, ColumnSpecs...>> : public std::true_type
  {
  };

  template <typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  struct is_recursive_cte<cte_t<NameTagProvider, Statement, ColumnSpecs...>> : public std::true_type
  {
#warning: Need to test this.
    constexpr static bool value = required_ctes_of_t<Statement>::template contains<cte_ref_t<NameTagProvider>>();
  };

  template <typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  struct is_table<cte_t<NameTagProvider, Statement, ColumnSpecs...>> : public std::true_type
  {
  };

  template <typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  struct name_tag_of<cte_t<NameTagProvider, Statement, ColumnSpecs...>> : public name_tag_of<NameTagProvider>
  {
  };

  template <typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  struct nodes_of<cte_t<NameTagProvider, Statement, ColumnSpecs...>> 
  {
    using type = Statement;
  };

  template <typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  struct provided_ctes_of<cte_t<NameTagProvider, Statement, ColumnSpecs...>>
  {
    using type = detail::type_set<cte_ref_t<NameTagProvider>>;
  };

  template <typename Context, typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  auto to_sql_string(Context& context, const cte_t<NameTagProvider, Statement, ColumnSpecs...>& t) -> std::string
  {
    return name_to_sql_string(context, name_tag_of_t<NameTagProvider>::name) + " AS (" +
           to_sql_string(context, t._statement) + ")";
  }

  // The cte_ref_t represents the cte as table in FROM.
  // The cte_t needs to be provided by WITH.
  template <typename NameTagProvider>
  struct cte_ref_t
  {
    template <typename Statement>
    auto as(Statement statement) -> make_cte_t<NameTagProvider, Statement>
    {
      static_assert(required_tables_of_t<Statement>::empty(),
                    "common table expression must not use unknown tables");
      static_assert(not required_ctes_of_t<Statement>::template contains<NameTagProvider>(),
                    "common table expression must not self-reference in the first part, use union_all/union_distinct "
                    "for recursion");

      return {statement};
    }
  };

   template<typename NameTagProvider>
    struct name_tag_of<cte_ref_t<NameTagProvider>> : public name_tag_of<NameTagProvider>
    {};

    template <typename NameTagProvider>
    struct provided_tables_of<cte_ref_t<NameTagProvider>>
    {
      using type = sqlpp::detail::type_set<cte_ref_t<NameTagProvider>>;
    };

    template <typename NameTagProvider>
    struct required_ctes_of<cte_ref_t<NameTagProvider>>
    {
      using type = sqlpp::detail::type_set<cte_ref_t<NameTagProvider>>;
    };

   template<typename NameTagProvider>
    struct required_static_ctes_of<cte_ref_t<NameTagProvider>> : public required_ctes_of<cte_ref_t<NameTagProvider>>
    {
    };

  template <typename Context, typename NameTagProvider>
  auto to_sql_string(Context& context, const cte_ref_t<NameTagProvider>&) -> std::string
  {
    return name_to_sql_string(context, name_tag_of_t<NameTagProvider>::name);
  }

  template <typename NameTagProvider>
  auto cte(const NameTagProvider& /*unused*/) -> cte_ref_t<NameTagProvider>
  {
    return {};
  }
}  // namespace sqlpp

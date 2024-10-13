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
    using _nodes = detail::type_vector<>;
    using _required_ctes = detail::make_joined_set_t<required_ctes_of<Lhs>, required_ctes_of<Rhs>>;
    using _parameters = detail::type_vector_cat_t<parameters_of<Lhs>, parameters_of<Rhs>>;

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

  // Interpreters
  template <typename Context, typename Flag, typename Lhs, typename Rhs>
  auto to_sql_string(Context& context, const cte_union_t<Flag, Lhs, Rhs>& t) -> std::string
  {
    to_sql_string(context, t._lhs);
    context << " UNION ";
    to_sql_string(context, Flag{});
    context << " ";
    to_sql_string(context, t._rhs);
    return context;
  }

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  struct cte_t;

  template <typename NameTagProvider>
  struct cte_ref_t;

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  auto from_table(cte_t<NameTagProvider, Statement, FieldSpecs...> /*unused*/) -> cte_ref_t<NameTagProvider>
  {
    return cte_ref_t<NameTagProvider>{};
  }

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  struct from_table_impl<cte_t<NameTagProvider, Statement, FieldSpecs...>>
  {
    using type = cte_ref_t<NameTagProvider>;
  };

#warning: Why can't we use FieldSpec directly? If not, does this one need to inherit from name_tag_base?
  template <typename FieldSpec>
  struct cte_column_spec_t
  {
    using _sqlpp_name_tag = name_tag_of_t<FieldSpec>;

    using _traits = make_traits<value_type_of_t<FieldSpec>>;
  };

  template <typename NameTagProvider, typename Statement, typename ResultRow>
  struct make_cte_impl
  {
    using type = void;
  };

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  struct make_cte_impl<NameTagProvider, Statement, result_row_t<void, FieldSpecs...>>
  {
    using type = cte_t<NameTagProvider, Statement, FieldSpecs...>;
  };

  template <typename NameTagProvider, typename Statement>
  using make_cte_t = typename make_cte_impl<NameTagProvider, Statement, get_result_row_t<Statement>>::type;

  // workaround for msvc unknown internal error
  //  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  //  struct cte_t
  //	  : public member_t<cte_column_spec_t<FieldSpecs>, column_t<NameTagProvider, cte_column_spec_t<FieldSpecs>>>...
  template <typename NameTagProvider, typename FieldSpec>
  struct cte_base
  {
    using type = member_t<FieldSpec, column_t<NameTagProvider, cte_column_spec_t<FieldSpec>>>;
  };

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
  template <typename... T>
  struct check_cte_union
  {
    using type = static_combined_check_t<
        static_check_t<logic::all<is_statement_t<T>::value...>::value, assert_cte_union_args_are_statements_t>>;
  };
  template <typename... T>
  using check_cte_union_t = typename check_cte_union<T...>::type;

  template <typename NameTagProvider, typename Statement, typename... FieldSpecs>
  struct cte_t : public cte_base<NameTagProvider, FieldSpecs>::type...
  {
    using _traits = make_traits<no_value_t, tag::is_cte>;
    using _nodes = detail::type_vector<>;
    using _provided_tables = detail::type_set<cte_t>;
    using _required_ctes = detail::make_joined_set_t<required_ctes_of<Statement>, detail::type_set<NameTagProvider>>;
    using _parameters = parameters_of<Statement>;

    constexpr static bool _is_recursive = required_ctes_of<Statement>::template count<NameTagProvider>();

    using _column_tuple_t = std::tuple<column_t<NameTagProvider, cte_column_spec_t<FieldSpecs>>...>;

    using _result_row_t = result_row_t<void, FieldSpecs...>;

    template <typename Rhs>
    auto union_distinct(Rhs rhs) const
        -> union_cte_impl_t<check_cte_union_t<Rhs>,
                            cte_t<NameTagProvider, cte_union_t<distinct_t, Statement, Rhs>, FieldSpecs...>>
    {
      static_assert(is_statement_t<Rhs>::value, "argument of union call has to be a statement");
      static_assert(has_policy_t<Rhs, is_select_t>::value, "argument of union call has to be a select");
      static_assert(has_result_row<Rhs>::value, "argument of a clause/union.has to be a (complete) select statement");

      static_assert(std::is_same<_result_row_t, get_result_row_t<Rhs>>::value,
                    "both select statements in a clause/union.have to have the same result columns (type and name)");

      return _union_impl<void, distinct_t>(check_cte_union_t<Rhs>{}, rhs);
    }

    template <typename Rhs>
    auto union_all(Rhs rhs) const
        -> union_cte_impl_t<check_cte_union_t<Rhs>,
                            cte_t<NameTagProvider, cte_union_t<all_t, Statement, Rhs>, FieldSpecs...>>
    {
      static_assert(is_statement_t<Rhs>::value, "argument of union call has to be a statement");
      static_assert(has_policy_t<Rhs, is_select_t>::value, "argument of union call has to be a select");
      static_assert(has_result_row<Rhs>::value, "argument of a clause/union.has to be a (complete) select statement");

      static_assert(std::is_same<_result_row_t, get_result_row_t<Rhs>>::value,
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

#warning: is table? really?
  template <typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  struct is_table<cte_t<NameTagProvider, Statement, ColumnSpecs...>> : public std::true_type
  {
  };

  template <typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  struct name_tag_of<cte_t<NameTagProvider, Statement, ColumnSpecs...>> : public name_tag_of<NameTagProvider>
  {
  };

  template <typename Context, typename NameTagProvider, typename Statement, typename... ColumnSpecs>
  auto to_sql_string(Context& context, const cte_t<NameTagProvider, Statement, ColumnSpecs...>& t) -> std::string
  {
    return name_to_sql_string(context, name_tag_of_t<NameTagProvider>::name) +  " AS (" + 
    to_sql_string(context, t._statement) + ")";
  }

  // The cte_t is displayed as NameTagProviderName except within the with:
  //    - the with needs the
  //      NameTagProviderName AS (ColumnNames) (select/union)
  template <typename NameTagProvider>
  struct cte_ref_t
  {
    using _traits = make_traits<no_value_t, tag::is_alias, tag::is_cte>;
    using _nodes = detail::type_vector<>;
    using _required_ctes = detail::make_type_set_t<NameTagProvider>;
    using _provided_tables = detail::type_set<NameTagProvider>;

    template <typename Statement>
    auto as(Statement statement) -> make_cte_t<NameTagProvider, Statement>
    {
      static_assert(required_tables_of_t<Statement>::empty(),
                    "common table expression must not use unknown tables");
      static_assert(not required_ctes_of<Statement>::template count<NameTagProvider>(),
                    "common table expression must not self-reference in the first part, use union_all/union_distinct "
                    "for recursion");

      return {statement};
    }
  };

#warning: is table? really?
   template<typename NameTagProvider>
    struct is_table<cte_ref_t<NameTagProvider>> : public std::true_type{};

   template<typename NameTagProvider>
    struct name_tag_of<cte_ref_t<NameTagProvider>> : public name_tag_of<NameTagProvider>{};

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

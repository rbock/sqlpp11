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

#ifndef SQLPP_CTE_H
#define SQLPP_CTE_H

#include <sqlpp11/expression.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/logic.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/result_row.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/statement_fwd.h>
#include <sqlpp11/table_ref.h>
#include <sqlpp11/type_traits.h>

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
  struct serializer_t<Context, cte_union_t<Flag, Lhs, Rhs>>
  {
    using _serialize_check = serialize_check_of<Context, Lhs, Rhs>;
    using T = cte_union_t<Flag, Lhs, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      serialize(t._lhs, context);
      context << " UNION ";
      serialize(Flag{}, context);
      context << " ";
      serialize(t._rhs, context);
      return context;
    }
  };

  template <typename AliasProvider, typename Statement, typename... FieldSpecs>
  struct cte_t;

  template <typename AliasProvider>
  struct cte_ref_t;

  template <typename AliasProvider, typename Statement, typename... FieldSpecs>
  auto from_table(cte_t<AliasProvider, Statement, FieldSpecs...>) -> cte_ref_t<AliasProvider>
  {
    return cte_ref_t<AliasProvider>{};
  }

  template <typename AliasProvider, typename Statement, typename... FieldSpecs>
  struct from_table_impl<cte_t<AliasProvider, Statement, FieldSpecs...>>
  {
    using type = cte_ref_t<AliasProvider>;
  };

  template <typename FieldSpec>
  struct cte_column_spec_t
  {
    using _alias_t = typename FieldSpec::_alias_t;

    using _traits = make_traits<value_type_of<FieldSpec>,
                                tag::must_not_insert,
                                tag::must_not_update,
                                tag_if<tag::can_be_null, column_spec_can_be_null_t<FieldSpec>::value>>;
  };

  template <typename AliasProvider, typename Statement, typename ResultRow>
  struct make_cte_impl
  {
    using type = void;
  };

  template <typename AliasProvider, typename Statement, typename... FieldSpecs>
  struct make_cte_impl<AliasProvider, Statement, result_row_t<void, FieldSpecs...>>
  {
    using type = cte_t<AliasProvider, Statement, FieldSpecs...>;
  };

  template <typename AliasProvider, typename Statement>
  using make_cte_t = typename make_cte_impl<AliasProvider, Statement, get_result_row_t<Statement>>::type;

  // workaround for msvc unknown internal error
  //  template <typename AliasProvider, typename Statement, typename... FieldSpecs>
  //  struct cte_t
  //	  : public member_t<cte_column_spec_t<FieldSpecs>, column_t<AliasProvider, cte_column_spec_t<FieldSpecs>>>...
  template <typename AliasProvider, typename FieldSpec>
  struct cte_base
  {
    using type = member_t<cte_column_spec_t<FieldSpec>, column_t<AliasProvider, cte_column_spec_t<FieldSpec>>>;
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
        static_check_t<logic::all_t<is_statement_t<T>::value...>::value, assert_cte_union_args_are_statements_t>>;
  };
  template <typename... T>
  using check_cte_union_t = typename check_cte_union<T...>::type;

  template <typename AliasProvider, typename Statement, typename... FieldSpecs>
  struct cte_t : public cte_base<AliasProvider, FieldSpecs>::type...
  {
    using _traits = make_traits<no_value_t, tag::is_cte, tag::is_table>;  // FIXME: is table? really?
    using _nodes = detail::type_vector<>;
    using _required_ctes = detail::make_joined_set_t<required_ctes_of<Statement>, detail::type_set<AliasProvider>>;
    using _parameters = parameters_of<Statement>;

    using _alias_t = typename AliasProvider::_alias_t;
    constexpr static bool _is_recursive = detail::is_element_of<AliasProvider, required_ctes_of<Statement>>::value;

    using _column_tuple_t = std::tuple<column_t<AliasProvider, cte_column_spec_t<FieldSpecs>>...>;

    using _result_row_t = result_row_t<void, FieldSpecs...>;

    template <typename Rhs>
    auto union_distinct(Rhs rhs) const
        -> union_cte_impl_t<check_cte_union_t<Rhs>,
                            cte_t<AliasProvider, cte_union_t<distinct_t, Statement, Rhs>, FieldSpecs...>>
    {
      static_assert(is_statement_t<Rhs>::value, "argument of union call has to be a statement");
      static_assert(has_policy_t<Rhs, is_select_t>::value, "argument of union call has to be a select");
      static_assert(has_result_row_t<Rhs>::value, "argument of a union has to be a (complete) select statement");

      static_assert(std::is_same<_result_row_t, get_result_row_t<Rhs>>::value,
                    "both select statements in a union have to have the same result columns (type and name)");

      return _union_impl<void, distinct_t>(check_cte_union_t<Rhs>{}, rhs);
    }

    template <typename Rhs>
    auto union_all(Rhs rhs) const
        -> union_cte_impl_t<check_cte_union_t<Rhs>,
                            cte_t<AliasProvider, cte_union_t<all_t, Statement, Rhs>, FieldSpecs...>>
    {
      static_assert(is_statement_t<Rhs>::value, "argument of union call has to be a statement");
      static_assert(has_policy_t<Rhs, is_select_t>::value, "argument of union call has to be a select");
      static_assert(has_result_row_t<Rhs>::value, "argument of a union has to be a (complete) select statement");

      static_assert(std::is_same<_result_row_t, get_result_row_t<Rhs>>::value,
                    "both select statements in a union have to have the same result columns (type and name)");

      return _union_impl<all_t>(check_cte_union_t<Rhs>{}, rhs);
    }

  private:
    template <typename Flag, typename Check, typename Rhs>
    auto _union_impl(Check, Rhs rhs) const -> inconsistent<Check>;

    template <typename Flag, typename Rhs>
    auto _union_impl(consistent_t, Rhs rhs) const
        -> cte_t<AliasProvider, cte_union_t<Flag, Statement, Rhs>, FieldSpecs...>
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

  template <typename Context, typename AliasProvider, typename Statement, typename... ColumnSpecs>
  struct serializer_t<Context, cte_t<AliasProvider, Statement, ColumnSpecs...>>
  {
    using _serialize_check = serialize_check_of<Context, Statement>;
    using T = cte_t<AliasProvider, Statement, ColumnSpecs...>;

    static Context& _(const T& t, Context& context)
    {
      context << name_of<T>::char_ptr() << " AS (";
      serialize(t._statement, context);
      context << ")";
      return context;
    }
  };

  // The cte_t is displayed as AliasProviderName except within the with:
  //    - the with needs the
  //      AliasProviderName AS (ColumnNames) (select/union)
  // The result row of the select should not have dynamic parts
  template <typename AliasProvider>
  struct cte_ref_t
  {
    using _traits = make_traits<no_value_t, tag::is_alias, tag::is_cte, tag::is_table>;  // FIXME: is table? really?
    using _nodes = detail::type_vector<>;
    using _required_ctes = detail::make_type_set_t<AliasProvider>;
    using _provided_tables = detail::type_set<AliasProvider>;

    using _alias_t = typename AliasProvider::_alias_t;

    template <typename Statement>
    auto as(Statement statement) -> make_cte_t<AliasProvider, Statement>
    {
      static_assert(required_tables_of<Statement>::size::value == 0,
                    "common table expression must not use unknown tables");
      static_assert(not detail::is_element_of<AliasProvider, required_ctes_of<Statement>>::value,
                    "common table expression must not self-reference in the first part, use union_all/union_distinct "
                    "for recursion");
      static_assert(is_static_result_row_t<get_result_row_t<Statement>>::value,
                    "ctes must not have dynamically added columns");

      return {statement};
    }
  };

  template <typename Context, typename AliasProvider>
  struct serializer_t<Context, cte_ref_t<AliasProvider>>
  {
    using _serialize_check = consistent_t;
    using T = cte_ref_t<AliasProvider>;

    static Context& _(const T&, Context& context)
    {
      context << name_of<T>::char_ptr();
      return context;
    }
  };

  template <typename AliasProvider>
  auto cte(const AliasProvider&) -> cte_ref_t<AliasProvider>
  {
    return {};
  }
}

#endif

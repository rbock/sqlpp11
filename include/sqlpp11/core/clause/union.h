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
#include <sqlpp11/core/query/statement_fwd.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/clause/union_flags.h>

namespace sqlpp
{
  // There is no order by or limit or offset in union, use it as a pseudo table to do that.

  template <typename Flag, typename Lhs, typename Rhs>
  struct union_t
  {
    union_t(Lhs lhs, Rhs rhs) : _lhs(lhs), _rhs(rhs)
    {
    }

    union_t(const union_t&) = default;
    union_t(union_t&&) = default;
    union_t& operator=(const union_t&) = default;
    union_t& operator=(union_t&&) = default;
    ~union_t() = default;

    template <typename Statement>
    using _result_methods_t = typename Lhs::_result_methods_t;

    Lhs _lhs;
    Rhs _rhs;
  };

  template <typename Flag, typename Lhs, typename Rhs>
  struct is_clause<union_t<Flag, Lhs, Rhs>> : public std::true_type
  {
  };

  template <typename Statement, typename Flag, typename Lhs, typename Rhs>
  struct consistency_check<Statement, union_t<Flag, Lhs, Rhs>>
  {
    using type = detail::get_first_if<is_inconsistent_t,
                                      consistent_t,
                                      statement_consistency_check_t<Lhs>,
                                      statement_consistency_check_t<Rhs>>;
  };

  template <typename Flag, typename Lhs, typename Rhs>
  struct is_result_clause<union_t<Flag, Lhs, Rhs>> : public std::true_type
  {
  };


  SQLPP_PORTABLE_STATIC_ASSERT(assert_union_args_are_statements_t, "arguments for union() must be statements");
  template <typename... T>
  struct check_union
  {
    using type = static_combined_check_t<
        static_check_t<logic::all<is_statement<T>::value...>::value, assert_union_args_are_statements_t>>;
  };
  template <typename... T>
  using check_union_t = typename check_union<T...>::type;

  struct no_union_t
  {
  };

  template <typename Statement>
  struct consistency_check<Statement, no_union_t>
  {
    using type = consistent_t;
  };

  template <typename Statement>
  struct clause_base<no_union_t, Statement> : public clause_data<no_union_t, Statement>
  {
    using clause_data<no_union_t, Statement>::clause_data;

#warning: reactivate check_union_t
    template <typename Rhs>
    auto union_distinct(Rhs rhs) const
        -> statement_t<union_t<union_distinct_t, Statement, Rhs>>
    {
      static_assert(is_statement<Rhs>::value, "argument of union call has to be a statement");
      static_assert(has_policy_t<Rhs, is_select_t>::value, "argument of union call has to be a select");
      static_assert(has_result_row<Rhs>::value, "argument of a clause/union.has to be a complete select statement");
      static_assert(has_result_row<Statement>::value,
                    "left hand side argument of a clause/union.has to be a complete select statement or union");

      using lhs_result_row_t = get_result_row_t<Statement>;
      using rhs_result_row_t = get_result_row_t<Rhs>;
      static_assert(is_result_compatible<lhs_result_row_t, rhs_result_row_t>::value,
                    "both arguments in a clause/union.have to have the same result columns (type and name)");

      return statement_t<union_t<union_distinct_t, Statement, Rhs>>{
          statement_constructor_arg<union_t<union_distinct_t, Statement, Rhs>>{
              union_t<union_distinct_t, Statement, Rhs>{static_cast<const Statement&>(*this), rhs}}};
      }

      template <typename Rhs>
      auto union_all(Rhs rhs) const
        -> statement_t<union_t<union_all_t, Statement, Rhs>>
      {
        static_assert(is_statement<Rhs>::value, "argument of union call has to be a statement");
        static_assert(has_policy_t<Rhs, is_select_t>::value, "argument of union call has to be a select");
        static_assert(has_result_row<Rhs>::value, "argument of a clause/union.has to be a (complete) select statement");
        static_assert(has_result_row<Statement>::value,
                      "left hand side argument of a clause/union.has to be a (complete) select statement");

        using lhs_result_row_t = get_result_row_t<Statement>;
        using rhs_result_row_t = get_result_row_t<Rhs>;
        static_assert(is_result_compatible<lhs_result_row_t, rhs_result_row_t>::value,
                      "both arguments in a clause/union.have to have the same result columns (type and name)");

      return statement_t<union_t<union_all_t, Statement, Rhs>>{
          statement_constructor_arg<union_t<union_distinct_t, Statement, Rhs>>{
              union_t<union_distinct_t, Statement, Rhs>{static_cast<const Statement&>(*this), rhs}}};
      }
  };

  // Interpreters
  template <typename Context, typename Flag, typename Lhs, typename Rhs>
  auto to_sql_string(Context& context, const union_t<Flag, Lhs, Rhs>& t) -> std::string
  {
    return to_sql_string(context, t._lhs) + " UNION " + to_sql_string(context, Flag{}) + " " +
           to_sql_string(context, t._rhs);
  }
  template <typename Lhs, typename Rhs>
  auto union_all(Lhs lhs, Rhs rhs)->decltype(lhs.union_all(rhs))
  {
    return lhs.union_all(std::move(rhs));
  }

  template <typename Lhs, typename Rhs>
  auto union_distinct(Lhs lhs, Rhs rhs)->decltype(lhs.union_distinct(rhs))
  {
    return lhs.union_distinct(std::move(rhs));
  }

}  // namespace sqlpp

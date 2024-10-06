#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/core/basic/join_fwd.h>
#include <sqlpp11/core/basic/enable_join.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  // Join representation including condition
  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  struct join_t : enable_join<join_t<Lhs, JoinType, Rhs, Condition>>
  {
    join_t(Lhs lhs, Rhs rhs, Condition condition)
        : _lhs(std::move(lhs)), _rhs(std::move(rhs)), _condition(std::move(condition))
    {
    }

    join_t(const join_t&) = default;
    join_t(join_t&&) = default;
    join_t& operator=(const join_t&) = default;
    join_t& operator=(join_t&&) = default;
    ~join_t() = default;

    Lhs _lhs;
    Rhs _rhs;
    Condition _condition;
  };

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  struct nodes_of<join_t<Lhs, JoinType, Rhs, Condition>>
  {
    using type = sqlpp::detail::type_vector<Lhs, Rhs, Condition>;
  };

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  struct provided_tables_of<join_t<Lhs, JoinType, Rhs, Condition>>
  {
    using type = detail::type_vector_cat_t<provided_tables_of_t<Lhs>, provided_tables_of_t<Rhs>>;
  };

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  struct provided_static_tables_of<join_t<Lhs, JoinType, Rhs, Condition>>
  {
    using type = typename std::conditional<
        is_dynamic<Rhs>::value,
        provided_static_tables_of_t<Lhs>,
        detail::type_vector_cat_t<provided_static_tables_of_t<Lhs>, provided_static_tables_of_t<Rhs>>>::type;
  };

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  struct provided_optional_tables_of<join_t<Lhs, JoinType, Rhs, Condition>>
  {
    using type = detail::type_vector_cat_t<
        typename std::conditional<detail::type_vector<right_outer_join_t, full_outer_join_t>::contains<JoinType>(),
                                  provided_tables_of_t<Lhs>,
                                  detail::type_vector<>>::type,
        typename std::conditional<detail::type_vector<left_outer_join_t, full_outer_join_t>::contains<JoinType>(),
                                  provided_tables_of_t<Rhs>,
                                  detail::type_vector<>>::type>;
  };

#warning: Need to make extra sure that ON does not require on extra tables.
  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  struct required_tables_of<join_t<Lhs, JoinType, Rhs, Condition>>
  {
    using type = detail::type_vector<>;
  };

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  struct required_static_tables_of<join_t<Lhs, JoinType, Rhs, Condition>>
  {
    using type = detail::type_vector<>;
  };

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  struct is_table<join_t<Lhs, JoinType, Rhs, Condition>> : public std::true_type
  {
  };

  template <typename Context, typename Lhs, typename JoinType, typename Rhs, typename Condition>
  auto to_sql_string(Context& context, const join_t<Lhs, JoinType, Rhs, Condition>& t) -> std::string
  {
    static_assert(not std::is_same<JoinType, cross_join_t>::value, "");
    return to_sql_string(context, t._lhs) + JoinType::_name + to_sql_string(context, t._rhs) + " ON " +
           to_sql_string(context, t._condition);
  }

  template <typename Context, typename Lhs, typename JoinType, typename Rhs, typename Condition>
  auto to_sql_string(Context& context, const join_t<Lhs, JoinType, dynamic_t<Rhs>, Condition>& t) -> std::string
  {
    static_assert(not std::is_same<JoinType, cross_join_t>::value, "");
    if (t._rhs._condition)
    {
      return to_sql_string(context, t._lhs) + JoinType::_name + to_sql_string(context, t._rhs) + " ON " +
             to_sql_string(context, t._condition);
    }
    return to_sql_string(context, t._lhs);
  }

  template <typename Context, typename Lhs, typename Rhs>
  auto to_sql_string(Context& context, const join_t<Lhs, cross_join_t, Rhs, unconditional_t>& t) -> std::string
  {
    return to_sql_string(context, t._lhs) + cross_join_t::_name + to_sql_string(context, t._rhs);
  }

  template <typename Context, typename Lhs, typename Rhs>
  auto to_sql_string(Context& context, const join_t<Lhs, cross_join_t, dynamic_t<Rhs>, unconditional_t>& t) -> std::string
  {
    if (t._rhs._condition)
    {
    return to_sql_string(context, t._lhs) + cross_join_t::_name + to_sql_string(context, t._rhs);
    }
    return to_sql_string(context, t._lhs);
  }

#warning: Verify that the Expr does not require tables other than Lhs, Rhs
  template <typename Expr, typename StaticTableTypeVector, typename AllTableTypeVector>
  struct are_table_requirements_satisfied
      : std::integral_constant<bool,
                               StaticTableTypeVector::contains_all(required_static_tables_of_t<Expr>{}) and
                                   AllTableTypeVector::contains_all(required_tables_of_t<Expr>{})>
  {
  };

  template <typename Lhs, typename Rhs, typename Expr>
  struct are_join_table_requirements_satisfied
      : public std::integral_constant<
            bool,
            is_dynamic<Rhs>::value ?
                                   // In case of a dynamic join, we can use all tables in the ON expr.
                are_table_requirements_satisfied<
                    Expr,
                    provided_tables_of_t<join_t<Lhs, cross_join_t, Rhs, unconditional_t>>,
                    provided_tables_of_t<join_t<Lhs, cross_join_t, Rhs, unconditional_t>>>::value
                                   :
                                   // In case of a static join, we can use static tables in the static part of the ON
                                   // expression and dynamic tables in any potential dynamic part of the expression.
                are_table_requirements_satisfied<
                    Expr,
                    provided_static_tables_of_t<join_t<Lhs, cross_join_t, Rhs, unconditional_t>>,
                    provided_tables_of_t<join_t<Lhs, cross_join_t, Rhs, unconditional_t>>>::value>
  {
  };

  template <typename Lhs, typename Rhs, typename Expr>
    using check_on_args = sqlpp::enable_if_t<sqlpp::is_boolean<Expr>::value and are_join_table_requirements_satisfied<Lhs, Rhs, Expr>::value>;

  template <typename Lhs, typename JoinType, typename Rhs>
  struct pre_join_t
  {
    template <typename Expr, typename = check_on_args<Lhs, Rhs, Expr>>
    auto on(Expr expr) const -> join_t<Lhs, JoinType, Rhs, Expr>
    {
     return {_lhs, _rhs, std::move(expr)};
    }

    Lhs _lhs;
    Rhs _rhs;
  };

  // Note: See sqlpp11/core/basic/join_fwd.h for forward declarations including check_join_args.

  template <typename Lhs, typename Rhs, typename /* = check_join_args<Lhs, Rhs> */>
  auto join(Lhs lhs, Rhs rhs) -> pre_join_t<Lhs, inner_join_t, Rhs>
  {
#warning: What is the point of from_table? rename to make_table_ref?
    return {from_table(std::move(lhs)), from_table(std::move(rhs))};
  }

  template <typename Lhs, typename Rhs, typename /* = check_join_args<Lhs, Rhs> */>
  auto inner_join(Lhs lhs, Rhs rhs) -> pre_join_t<Lhs, inner_join_t, Rhs>
  {
    return {from_table(std::move(lhs)), from_table(std::move(rhs))};
  }

  template <typename Lhs, typename Rhs, typename /* = check_join_args<Lhs, Rhs> */>
  auto left_outer_join(Lhs lhs, Rhs rhs) -> pre_join_t<Lhs, left_outer_join_t, Rhs>
  {
    return {from_table(std::move(lhs)), from_table(std::move(rhs))};
  }

  template <typename Lhs, typename Rhs, typename /* = check_join_args<Lhs, Rhs> */>
  auto right_outer_join(Lhs lhs, Rhs rhs) -> pre_join_t<Lhs, right_outer_join_t, Rhs>
  {
    return {from_table(std::move(lhs)), from_table(std::move(rhs))};
  }

  template <typename Lhs, typename Rhs, typename /* = check_join_args<Lhs, Rhs> */>
  auto full_outer_join(Lhs lhs, Rhs rhs) -> pre_join_t<Lhs, full_outer_join_t, Rhs>
  {
    return {from_table(std::move(lhs)), from_table(std::move(rhs))};
  }

  template <typename Lhs, typename Rhs, typename /* = check_join_args<Lhs, Rhs> */>
  auto cross_join(Lhs lhs, Rhs rhs) -> join_t<Lhs, cross_join_t, Rhs, unconditional_t>
  {
    return {from_table(std::move(lhs)), from_table(std::move(rhs)), {}};
  }

}  // namespace sqlpp
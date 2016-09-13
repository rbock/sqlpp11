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

#ifndef SQLPP_PRE_JOIN_H
#define SQLPP_PRE_JOIN_H

#include <sqlpp11/join_types.h>
#include <sqlpp11/noop.h>
#include <sqlpp11/on.h>

namespace sqlpp
{
  SQLPP_PORTABLE_STATIC_ASSERT(assert_pre_join_lhs_table_t, "lhs argument of join() has to be a table or a join");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_pre_join_rhs_table_t, "rhs argument of join() has to be a table");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_pre_join_rhs_no_join_t, "rhs argument of join() must not be a join");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_pre_join_unique_names_t, "joined table names have to be unique");

  template <typename Lhs, typename Rhs>
  struct check_pre_join
  {
    using type = static_combined_check_t<
        static_check_t<is_table_t<Lhs>::value, assert_pre_join_lhs_table_t>,
        static_check_t<is_table_t<Rhs>::value, assert_pre_join_rhs_table_t>,
        static_check_t<not is_join_t<Rhs>::value, assert_pre_join_rhs_no_join_t>,
        static_check_t<detail::is_disjunct_from<detail::make_name_of_set_t<provided_tables_of<Lhs>>,
                                                detail::make_name_of_set_t<provided_tables_of<Rhs>>>::value,
                       assert_pre_join_unique_names_t>>;
  };

  template <typename Lhs, typename Rhs>
  using check_pre_join_t = typename check_pre_join<Lhs, Rhs>::type;

  SQLPP_PORTABLE_STATIC_ASSERT(assert_join_consist_of_pre_join_and_on_t,
                               "join has to consist of a pre_join and a join condition");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_join_no_table_dependencies_t, "joined tables must not depend on other tables");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_join_on_no_foreign_table_dependencies_t,
                               "on() condition must not depend on other tables");

  template <typename PreJoin, typename On>
  struct check_join
  {
    using type = static_combined_check_t<
        static_check_t<is_pre_join_t<PreJoin>::value, assert_join_consist_of_pre_join_and_on_t>,
        static_check_t<is_on_t<On>::value, assert_join_consist_of_pre_join_and_on_t>,
        static_check_t<required_tables_of<PreJoin>::size::value == 0, assert_join_no_table_dependencies_t>,
        static_check_t<detail::is_subset_of<required_tables_of<On>, provided_tables_of<PreJoin>>::value,
                       assert_join_on_no_foreign_table_dependencies_t>>;
  };

  template <typename PreJoin, typename On>
  using check_join_t = typename check_join<PreJoin, On>::type;

  template <typename PreJoin, typename Expr>
  struct check_join_on
  {
    using type = static_combined_check_t<check_on_t<Expr>, check_join_t<PreJoin, on_t<Expr>>>;
  };

  template <typename PreJoin, typename Expr>
  using check_join_on_t = typename check_join_on<PreJoin, Expr>::type;

  template <typename PreJoin, typename On>
  struct join_t;

  template <typename JoinType, typename Lhs, typename Rhs>
  struct pre_join_t
  {
    using _traits = make_traits<no_value_t, tag::is_pre_join>;
    using _nodes = detail::type_vector<Lhs, Rhs>;
    using _can_be_null = std::false_type;
    using _provided_outer_tables = typename JoinType::template _provided_outer_tables<Lhs, Rhs>;

    static_assert(is_table_t<Lhs>::value, "lhs argument for join() has to be a table or join");
    static_assert(is_table_t<Rhs>::value, "rhs argument for join() has to be a table");
    static_assert(not is_join_t<Rhs>::value, "rhs argument for join must not be a join");

    static_assert(detail::is_disjunct_from<provided_tables_of<Lhs>, provided_tables_of<Rhs>>::value,
                  "joined tables must not be identical");

    static_assert(required_tables_of<pre_join_t>::size::value == 0, "joined tables must not depend on other tables");

    auto unconditionally() -> join_t<pre_join_t, on_t<unconditional_t>>
    {
      return {*this, {}};
    }

    template <typename Expr>
    auto on(Expr expr) const -> typename std::conditional<check_join_on_t<pre_join_t, Expr>::value,
                                                          join_t<pre_join_t, on_t<Expr>>,
                                                          check_join_on_t<pre_join_t, Expr>>::type
    {
      using Check = check_join_on_t<pre_join_t, Expr>;

      return on_impl(Check{}, expr);
    }

  private:
    template <typename Check, typename Expr>
    auto on_impl(Check, const Expr&) const -> inconsistent<Check>;

    template <typename Expr>
    auto on_impl(consistent_t, const Expr& expr) const -> join_t<pre_join_t, on_t<Expr>>
    {
      return {*this, {expr}};
    }

  public:
    Lhs _lhs;
    Rhs _rhs;
  };

  template <typename Context, typename JoinType, typename Lhs, typename Rhs>
  struct serializer_t<Context, pre_join_t<JoinType, Lhs, Rhs>>
  {
    using _serialize_check = serialize_check_of<Context, Lhs, Rhs>;
    using T = pre_join_t<JoinType, Lhs, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      serialize(t._lhs, context);
      context << JoinType::_name;
      context << " JOIN ";
      serialize(t._rhs, context);
      return context;
    }
  };

  namespace detail
  {
    template <typename JoinType, typename Check, typename Lhs, typename Rhs>
    auto join_impl(Check, Lhs lhs, Rhs rhs) -> inconsistent<Check>;

    template <typename JoinType, typename Lhs, typename Rhs>
    auto join_impl(consistent_t, Lhs lhs, Rhs rhs) -> pre_join_t<JoinType, Lhs, Rhs>;

    template <typename JoinType, typename Lhs, typename Rhs>
    auto join_impl(Lhs lhs, Rhs rhs) -> decltype(join_impl<JoinType>(check_pre_join_t<Lhs, Rhs>{}, lhs, rhs));
  }

  template <typename Lhs, typename Rhs>
  auto join(Lhs lhs, Rhs rhs) -> decltype(detail::join_impl<inner_join_t>(lhs, rhs))
  {
    return {lhs, rhs};
  }

  template <typename Lhs, typename Rhs>
  auto inner_join(Lhs lhs, Rhs rhs) -> decltype(detail::join_impl<inner_join_t>(lhs, rhs))
  {
    return {lhs, rhs};
  }

  template <typename Lhs, typename Rhs>
  auto left_outer_join(Lhs lhs, Rhs rhs) -> decltype(detail::join_impl<left_outer_join_t>(lhs, rhs))
  {
    return {lhs, rhs};
  }

  template <typename Lhs, typename Rhs>
  auto right_outer_join(Lhs lhs, Rhs rhs) -> decltype(detail::join_impl<right_outer_join_t>(lhs, rhs))
  {
    check_pre_join_t<Lhs, Rhs>{};

    return {lhs, rhs};
  }

  template <typename Lhs, typename Rhs>
  auto outer_join(Lhs lhs, Rhs rhs) -> decltype(detail::join_impl<outer_join_t>(lhs, rhs))
  {
    return {lhs, rhs};
  }

  namespace detail
  {
    template <typename Check, typename Lhs, typename Rhs>
    auto cross_join_impl(Check, Lhs lhs, Rhs rhs) -> inconsistent<Check>;

    template <typename Lhs, typename Rhs>
    auto cross_join_impl(consistent_t, Lhs lhs, Rhs rhs)
        -> join_t<pre_join_t<cross_join_t, Lhs, Rhs>, on_t<unconditional_t>>;

    template <typename Lhs, typename Rhs>
    auto cross_join_impl(Lhs lhs, Rhs rhs) -> decltype(cross_join_impl(check_pre_join_t<Lhs, Rhs>{}, lhs, rhs));
  }

  template <typename Lhs, typename Rhs>
  auto cross_join(Lhs lhs, Rhs rhs) -> decltype(detail::cross_join_impl(lhs, rhs))
  {
    return {pre_join_t<cross_join_t, Lhs, Rhs>{lhs, rhs}, {}};
  }
}

#endif

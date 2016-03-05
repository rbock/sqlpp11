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

#ifndef SQLPP_CROSS_JOIN_H
#define SQLPP_CROSS_JOIN_H

#include <sqlpp11/join_types.h>
#include <sqlpp11/on.h>
#include <sqlpp11/noop.h>

namespace sqlpp
{
  SQLPP_PORTABLE_STATIC_ASSERT(assert_cross_join_lhs_table_t, "lhs argument of join() has to be a table or a join");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_cross_join_rhs_table_t, "rhs argument of join() has to be a table");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_cross_join_rhs_no_join_t, "rhs argument of join() must not be a join");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_cross_join_unique_names_t, "joined table names have to be unique");

  template <typename Lhs, typename Rhs>
  struct check_cross_join
  {
    using type = static_combined_check_t<
        static_check_t<is_table_t<Lhs>::value, assert_cross_join_lhs_table_t>,
        static_check_t<is_table_t<Rhs>::value, assert_cross_join_rhs_table_t>,
        static_check_t<not is_join_t<Rhs>::value, assert_cross_join_rhs_no_join_t>,
        static_check_t<detail::is_disjunct_from<detail::make_name_of_set_t<provided_tables_of<Lhs>>,
                                                detail::make_name_of_set_t<provided_tables_of<Rhs>>>::value,
                       assert_cross_join_unique_names_t>>;
  };

  template <typename Lhs, typename Rhs>
  using check_cross_join_t = typename check_cross_join<Lhs, Rhs>::type;

  SQLPP_PORTABLE_STATIC_ASSERT(assert_join_consist_of_cross_join_and_on_t,
                               "join has to consist of a cross_join and a join condition");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_join_no_table_dependencies_t, "joined tables must not depend on other tables");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_join_on_no_foreign_table_dependencies_t,
                               "on() condition must not depend on other tables");

  template <typename CrossJoin, typename On>
  struct check_join
  {
    using type = static_combined_check_t<
        static_check_t<is_cross_join_t<CrossJoin>::value, assert_join_consist_of_cross_join_and_on_t>,
        static_check_t<is_on_t<On>::value, assert_join_consist_of_cross_join_and_on_t>,
        static_check_t<required_tables_of<CrossJoin>::size::value == 0, assert_join_no_table_dependencies_t>,
        static_check_t<detail::is_subset_of<required_tables_of<On>, provided_tables_of<CrossJoin>>::value,
                       assert_join_on_no_foreign_table_dependencies_t>>;
  };

  template <typename CrossJoin, typename On>
  using check_join_t = typename check_join<CrossJoin, On>::type;

  template <typename CrossJoin, typename Expr>
  struct check_join_on
  {
    using type = static_combined_check_t<check_on_t<Expr>, check_join_t<CrossJoin, on_t<Expr>>>;
  };

  template <typename CrossJoin, typename Expr>
  using check_join_on_t = typename check_join_on<CrossJoin, Expr>::type;

  template <typename CrossJoin, typename On>
  struct join_t;

  template <typename JoinType, typename Lhs, typename Rhs>
  struct cross_join_t
  {
    using _traits = make_traits<no_value_t, tag::is_cross_join>;
    using _nodes = detail::type_vector<Lhs, Rhs>;
    using _can_be_null = std::false_type;

    static_assert(is_table_t<Lhs>::value, "lhs argument for join() has to be a table or join");
    static_assert(is_table_t<Rhs>::value, "rhs argument for join() has to be a table");
    static_assert(not is_join_t<Rhs>::value, "rhs argument for join must not be a join");

    static_assert(detail::is_disjunct_from<provided_tables_of<Lhs>, provided_tables_of<Rhs>>::value,
                  "joined tables must not be identical");

    static_assert(required_tables_of<cross_join_t>::size::value == 0, "joined tables must not depend on other tables");

    template <typename Expr>
    auto on(Expr expr) const -> typename std::conditional<check_join_on_t<cross_join_t, Expr>::value,
                                                          join_t<cross_join_t, on_t<Expr>>,
                                                          bad_statement>::type
    {
      check_join_on_t<cross_join_t, Expr>::_();

      return {*this, {expr}};
    }

    auto unconditionally() -> join_t<cross_join_t, on_t<unconditional_t>>
    {
      return {*this, {}};
    }

    Lhs _lhs;
    Rhs _rhs;
  };

  template <typename Context, typename JoinType, typename Lhs, typename Rhs>
  struct serializer_t<Context, cross_join_t<JoinType, Lhs, Rhs>>
  {
    using _serialize_check = serialize_check_of<Context, Lhs, Rhs>;
    using T = cross_join_t<JoinType, Lhs, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      serialize(t._lhs, context);
      context << JoinType::_name;
      context << " JOIN ";
      serialize(t._rhs, context);
      return context;
    }
  };

  template <typename Lhs, typename Rhs>
  auto join(Lhs lhs, Rhs rhs) -> typename std::conditional<check_cross_join_t<Lhs, Rhs>::value,
                                                           cross_join_t<inner_join_t, Lhs, Rhs>,
                                                           bad_statement>::type
  {
    check_cross_join_t<Lhs, Rhs>::_();

    return {lhs, rhs};
  }

  template <typename Lhs, typename Rhs>
  auto inner_join(Lhs lhs, Rhs rhs) -> typename std::conditional<check_cross_join_t<Lhs, Rhs>::value,
                                                                 cross_join_t<inner_join_t, Lhs, Rhs>,
                                                                 bad_statement>::type
  {
    check_cross_join_t<Lhs, Rhs>::_();

    return {lhs, rhs};
  }

  template <typename Lhs, typename Rhs>
  auto left_outer_join(Lhs lhs, Rhs rhs) -> typename std::conditional<check_cross_join_t<Lhs, Rhs>::value,
                                                                      cross_join_t<left_outer_join_t, Lhs, Rhs>,
                                                                      bad_statement>::type
  {
    check_cross_join_t<Lhs, Rhs>::_();

    return {lhs, rhs};
  }

  template <typename Lhs, typename Rhs>
  auto right_outer_join(Lhs lhs, Rhs rhs) -> typename std::conditional<check_cross_join_t<Lhs, Rhs>::value,
                                                                       cross_join_t<right_outer_join_t, Lhs, Rhs>,
                                                                       bad_statement>::type
  {
    check_cross_join_t<Lhs, Rhs>::_();

    return {lhs, rhs};
  }

  template <typename Lhs, typename Rhs>
  auto outer_join(Lhs lhs, Rhs rhs) -> typename std::conditional<check_cross_join_t<Lhs, Rhs>::value,
                                                                 cross_join_t<right_outer_join_t, Lhs, Rhs>,
                                                                 bad_statement>::type
  {
    check_cross_join_t<Lhs, Rhs>::_();

    return {lhs, rhs};
  }
}

#endif

/*
 * Copyright (c) 2016-2016, Roland Bock
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

#ifndef SQLPP_DYNAMIC_PRE_JOIN_H
#define SQLPP_DYNAMIC_PRE_JOIN_H

#include <sqlpp11/join_types.h>
#include <sqlpp11/on.h>

namespace sqlpp
{
  SQLPP_PORTABLE_STATIC_ASSERT(assert_dynamic_pre_join_table_t, "argument of dynamic_join() has to be a table");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_dynamic_pre_join_no_join_t, "argument of dynamic_join() must not be a table");

  template <typename Table>
  struct check_dynamic_pre_join
  {
    using type =
        static_combined_check_t<static_check_t<is_table_t<Table>::value, assert_dynamic_pre_join_table_t>,
                                static_check_t<not is_join_t<Table>::value, assert_dynamic_pre_join_no_join_t>>;
  };

  template <typename Table>
  using check_dynamic_pre_join_t = typename check_dynamic_pre_join<Table>::type;

  SQLPP_PORTABLE_STATIC_ASSERT(assert_dynamic_join_consist_of_pre_join_and_on_t,
                               "dynamic join has to consist of a dynamic pre_join and a join condition");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_dynamic_join_no_table_dependencies_t,
                               "dynamically joined tables must not depend on other tables");

  template <typename PreJoin, typename On>
  struct check_dynamic_join
  {
    using type = static_combined_check_t<
        static_check_t<is_dynamic_pre_join_t<PreJoin>::value, assert_dynamic_join_consist_of_pre_join_and_on_t>,
        static_check_t<is_on_t<On>::value, assert_dynamic_join_consist_of_pre_join_and_on_t>,
        static_check_t<required_tables_of<PreJoin>::size::value == 0, assert_dynamic_join_no_table_dependencies_t>>;
  };

  template <typename PreJoin, typename On>
  using check_dynamic_join_t = typename check_dynamic_join<PreJoin, On>::type;

  template <typename PreJoin, typename Expr>
  struct check_dynamic_join_on
  {
    using type = static_combined_check_t<check_on_t<Expr>, check_dynamic_join_t<PreJoin, on_t<Expr>>>;
  };

  template <typename PreJoin, typename Expr>
  using check_dynamic_join_on_t = typename check_dynamic_join_on<PreJoin, Expr>::type;

  template <typename PreJoin, typename On>
  struct dynamic_join_t;

  template <typename JoinType, typename Rhs>
  struct dynamic_pre_join_t
  {
    using _traits = make_traits<no_value_t, tag::is_table, tag::is_dynamic_pre_join>;
    using _nodes = detail::type_vector<Rhs>;
    using _can_be_null = std::false_type;

    static_assert(is_table_t<Rhs>::value, "rhs argument for dynamic_join() has to be a table");
    static_assert(not is_join_t<Rhs>::value, "rhs argument for dynamic_join must not be a join");

    static_assert(required_tables_of<dynamic_pre_join_t>::size::value == 0,
                  "joined tables must not depend on other tables");

    auto unconditionally() const -> dynamic_join_t<dynamic_pre_join_t, on_t<unconditional_t>>
    {
      return {*this, {}};
    }

    template <typename Expr>
    auto on(Expr expr) const -> typename std::conditional<check_dynamic_join_on_t<dynamic_pre_join_t, Expr>::value,
                                                          dynamic_join_t<dynamic_pre_join_t, on_t<Expr>>,
                                                          check_dynamic_join_on_t<dynamic_pre_join_t, Expr>>::type
    {
      return {*this, {expr}};
    }

    Rhs _rhs;
  };

  template <typename Context, typename JoinType, typename Rhs>
  struct serializer_t<Context, dynamic_pre_join_t<JoinType, Rhs>>
  {
    using _serialize_check = serialize_check_of<Context, Rhs>;
    using T = dynamic_pre_join_t<JoinType, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      context << JoinType::_name;
      context << " JOIN ";
      serialize(t._rhs, context);
      return context;
    }
  };

  template <typename JoinType, typename Table>
  using make_dynamic_pre_join_t = typename std::conditional<check_dynamic_pre_join_t<Table>::value,
                                                            dynamic_pre_join_t<JoinType, Table>,
                                                            check_dynamic_pre_join_t<Table>>::type;

  template <typename Table>
  auto dynamic_join(Table table) -> make_dynamic_pre_join_t<inner_join_t, Table>
  {
    check_dynamic_pre_join_t<Table>{};  // FIXME: Failure return type?
    return {table};
  }

  template <typename Table>
  auto dynamic_inner_join(Table table) -> make_dynamic_pre_join_t<inner_join_t, Table>
  {
    check_dynamic_pre_join_t<Table>{};
    return {table};
  }

  template <typename Table>
  auto dynamic_left_outer_join(Table table) -> make_dynamic_pre_join_t<left_outer_join_t, Table>
  {
    check_dynamic_pre_join_t<Table>{};
    return {table};
  }

  template <typename Table>
  auto dynamic_right_outer_join(Table table) -> make_dynamic_pre_join_t<right_outer_join_t, Table>
  {
    check_dynamic_pre_join_t<Table>{};
    return {table};
  }

  template <typename Table>
  auto dynamic_outer_join(Table table) -> make_dynamic_pre_join_t<outer_join_t, Table>
  {
    check_dynamic_pre_join_t<Table>{};
    return {table};
  }

  template <typename Table>
  auto dynamic_cross_join(Table table) ->
      typename std::conditional<check_dynamic_pre_join_t<Table>::value,
                                dynamic_join_t<dynamic_pre_join_t<cross_join_t, Table>, on_t<unconditional_t>>,
                                check_dynamic_pre_join_t<Table>>::type
  {
    return {dynamic_pre_join_t<cross_join_t, Table>{table}, {}};
  }
}

#endif

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

#ifndef SQLPP_DYNAMIC_CROSS_JOIN_H
#define SQLPP_DYNAMIC_CROSS_JOIN_H

#include <sqlpp11/join_types.h>
#include <sqlpp11/on.h>

namespace sqlpp
{
  template <typename CrossJoin, typename On>
  struct dynamic_join_t;

  template <typename JoinType, typename Rhs>
  struct dynamic_cross_join_t
  {
    using _traits = make_traits<no_value_t, tag::is_table, tag::is_dynamic_cross_join>;
    using _nodes = detail::type_vector<Rhs>;
    using _can_be_null = std::false_type;

    static_assert(is_table_t<Rhs>::value, "rhs argument for dynamic_join() has to be a table");
    static_assert(not is_join_t<Rhs>::value, "rhs argument for dynamic_join must not be a join");

    static_assert(required_tables_of<dynamic_cross_join_t>::size::value == 0,
                  "joined tables must not depend on other tables");

    template <typename Expr>
    auto on(Expr expr) -> dynamic_join_t<dynamic_cross_join_t, on_t<void, Expr>>
    {
      static_assert(is_expression_t<Expr>::value, "argument is not a boolean expression in on()");
      static_assert(is_boolean_t<Expr>::value, "argument is not a boolean expression in on()");

      return {*this, {expr, {}}};
    }

    auto unconditionally() -> dynamic_join_t<dynamic_cross_join_t, on_t<void, unconditional_t>>
    {
      return {*this, {}};
    }

    Rhs _rhs;
  };

  template <typename Context, typename JoinType, typename Rhs>
  struct serializer_t<Context, dynamic_cross_join_t<JoinType, Rhs>>
  {
    using _serialize_check = serialize_check_of<Context, Rhs>;
    using T = dynamic_cross_join_t<JoinType, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      context << " JOIN ";
      serialize(t._rhs, context);
      return context;
    }
  };

  template <typename Table>
  dynamic_cross_join_t<inner_join_t, Table> dynamic_join(Table table)
  {
    return {table};
  }

  template <typename Table>
  dynamic_cross_join_t<inner_join_t, Table> dynamic_inner_join(Table table)
  {
    return {table};
  }

  template <typename Table>
  dynamic_cross_join_t<outer_join_t, Table> outer_join(Table table)
  {
    return {table};
  }

  template <typename Table>
  dynamic_cross_join_t<left_outer_join_t, Table> left_outer_join(Table table)
  {
    return {table};
  }

  template <typename Table>
  dynamic_cross_join_t<right_outer_join_t, Table> right_outer_join(Table table)
  {
    return {table};
  }
}

#endif

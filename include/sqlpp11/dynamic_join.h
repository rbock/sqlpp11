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

#ifndef SQLPP_DYNAMIC_JOIN_H
#define SQLPP_DYNAMIC_JOIN_H

#include <sqlpp11/join_types.h>
#include <sqlpp11/on.h>
#include <sqlpp11/noop.h>

namespace sqlpp
{
  template <typename JoinType, typename Rhs, typename On = noop>
  struct dynamic_join_t
  {
    using _traits = make_traits<no_value_t, tag::is_table, tag::is_dynamic_join>;
    using _nodes = detail::type_vector<Rhs>;
    using _can_be_null = std::false_type;

    static_assert(is_table_t<Rhs>::value, "rhs argument for join() has to be a table");
    static_assert(not is_join_t<Rhs>::value, "rhs argument for join must not be a join");
    static_assert(is_noop<On>::value or is_on_t<On>::value, "invalid on expression in join().on()");

    static_assert(required_tables_of<dynamic_join_t>::size::value == 0,
                  "joined tables must not depend on other tables");

    template <typename OnT>
    using set_on_t = dynamic_join_t<JoinType, Rhs, OnT>;

    template <typename... Expr>
    auto on(Expr... expr) -> set_on_t<on_t<void, Expr...>>
    {
      static_assert(is_noop<On>::value, "cannot call on() twice for a single join()");
      static_assert(logic::all_t<is_expression_t<Expr>::value...>::value,
                    "at least one argument is not an expression in on()");

      return {_rhs, {std::tuple<Expr...>{expr...}, {}}};
    }

    auto unconditionally() -> set_on_t<on_t<void, unconditional_t>>
    {
      static_assert(is_noop<On>::value, "cannot call on() twice for a single join()");
      return {_rhs, {}};
    }

    Rhs _rhs;
    On _on;
  };

  template <typename Context, typename JoinType, typename Rhs, typename On>
  struct serializer_t<Context, dynamic_join_t<JoinType, Rhs, On>>
  {
    using _serialize_check = serialize_check_of<Context, Rhs, On>;
    using T = dynamic_join_t<JoinType, Rhs, On>;

    static Context& _(const T& t, Context& context)
    {
      static_assert(not is_noop<On>::value, "joined tables require on()");
      context << " JOIN ";
      serialize(t._rhs, context);
      serialize(t._on, context);
      return context;
    }
  };

  template <typename Table>
  dynamic_join_t<inner_join_t, Table> dynamic_join(Table table)
  {
    return {table, {}};
  }

  template <typename Table>
  dynamic_join_t<inner_join_t, Table> dynamic_inner_join(Table table)
  {
    return {table, {}};
  }

  template <typename Table>
  dynamic_join_t<outer_join_t, Table> outer_join(Table table)
  {
    return {table, {}};
  }

  template <typename Table>
  dynamic_join_t<left_outer_join_t, Table> left_outer_join(Table table)
  {
    return {table, {}};
  }

  template <typename Table>
  dynamic_join_t<right_outer_join_t, Table> right_outer_join(Table table)
  {
    return {table, {}};
  }
}

#endif

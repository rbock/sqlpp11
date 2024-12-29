#pragma once

/**
 * Copyright © 2014-2019, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/clause/where.h>

namespace sqlpp
{
  namespace postgresql
  {
    template <typename ConflictTarget, typename... Assignments>
    struct on_conflict_do_update_t;

    // ON CONFLICT ... DO UPDATE ... WHERE ...
    template <typename ConflictTarget, typename Expression, typename... Assignments>
    struct on_conflict_do_update_where_t
    {
      on_conflict_do_update_where_t(
          Expression expression, on_conflict_do_update_t<ConflictTarget, Assignments...> assignments)
          : _expression(expression), _assignments(assignments)
      {
      }

      on_conflict_do_update_where_t(const on_conflict_do_update_where_t&) = default;
      on_conflict_do_update_where_t(on_conflict_do_update_where_t&&) = default;
      on_conflict_do_update_where_t& operator=(const on_conflict_do_update_where_t&) = default;
      on_conflict_do_update_where_t& operator=(on_conflict_do_update_where_t&&) = default;

      Expression _expression;
      on_conflict_do_update_t<ConflictTarget, Assignments...> _assignments;
    };

    // ON CONFLICT ... DO UPDATE ...
    template <typename ConflictTarget, typename... Assignments>
    struct on_conflict_do_update_t
    {
      on_conflict_do_update_t(ConflictTarget conflict_target, std::tuple<Assignments...> assignments)
          : _conflict_target(conflict_target), _assignments(assignments)
      {
      }

      on_conflict_do_update_t(const on_conflict_do_update_t&) = default;
      on_conflict_do_update_t(on_conflict_do_update_t&&) = default;
      on_conflict_do_update_t& operator=(const on_conflict_do_update_t&) = default;
      on_conflict_do_update_t& operator=(on_conflict_do_update_t&&) = default;
      ~on_conflict_do_update_t() = default;

      ConflictTarget _conflict_target;
      std::tuple<Assignments...> _assignments;

      // WHERE
      template <typename Expression>
      auto where(Expression expression) const -> decltype(new_statement(
          *this, on_conflict_do_update_where_t<ConflictTarget, Expression, Assignments...>{std::move(expression)}))
      {
        return new_statement(
            *this, on_conflict_do_update_where_t<ConflictTarget, Expression, Assignments...>{std::move(expression)});
      }
    };
  }  // namespace postgresql

  template <typename ConflictTarget, typename... Assignments>
  auto to_sql_string(postgresql::context_t& context,
                     const postgresql::on_conflict_do_update_t<ConflictTarget, Assignments...>& o) -> std::string
  {
    return to_sql_string(context, o._conflict_target) + "DO UPDATE SET " +
           tuple_to_sql_string(context, o._assignments, tuple_operand{", "});
  }

  template <typename ConflictTarget, typename Expression, typename... Assignments>
  auto to_sql_string(
      postgresql::context_t& context,
      const postgresql::on_conflict_do_update_where_t<ConflictTarget, Expression, Assignments...>& t)
      -> std::string
  {
    return to_sql_string(context, t._assignments) + " WHERE " + to_sql_string(context, t._expression);
  }
}  // namespace sqlpp

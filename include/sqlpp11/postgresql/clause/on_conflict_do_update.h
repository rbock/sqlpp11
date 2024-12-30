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
#include <sqlpp11/core/clause/clause_base.h>
#include <sqlpp11/core/clause/where.h>

namespace sqlpp
{
  namespace postgresql
  {
    // ON CONFLICT ... DO UPDATE ... WHERE ...
    template <typename OnConflictUpdate, typename Expression>
    struct on_conflict_do_update_where_t
    {
      on_conflict_do_update_where_t(
          OnConflictUpdate on_conflict_update, Expression expression)
          : _on_conflict_update(on_conflict_update), _expression(expression)
      {
      }

      on_conflict_do_update_where_t(const on_conflict_do_update_where_t&) = default;
      on_conflict_do_update_where_t(on_conflict_do_update_where_t&&) = default;
      on_conflict_do_update_where_t& operator=(const on_conflict_do_update_where_t&) = default;
      on_conflict_do_update_where_t& operator=(on_conflict_do_update_where_t&&) = default;

      OnConflictUpdate _on_conflict_update;
      Expression _expression;
    };

    // ON CONFLICT ... DO UPDATE ...
    template <typename OnConflict, typename... Assignments>
    struct on_conflict_do_update_t
    {
      on_conflict_do_update_t(OnConflict on_conflict, std::tuple<Assignments...> assignments)
          : _on_conflict(on_conflict), _assignments(assignments)
      {
      }

      on_conflict_do_update_t(const on_conflict_do_update_t&) = default;
      on_conflict_do_update_t(on_conflict_do_update_t&&) = default;
      on_conflict_do_update_t& operator=(const on_conflict_do_update_t&) = default;
      on_conflict_do_update_t& operator=(on_conflict_do_update_t&&) = default;
      ~on_conflict_do_update_t() = default;

      OnConflict _on_conflict;
      std::tuple<Assignments...> _assignments;
    };
  }  // namespace postgresql

#warning: Do we need is_clause?
  template <typename OnConflict, typename... Assignments>
  struct is_clause<postgresql::on_conflict_do_update_t<OnConflict, Assignments...>>: public std::true_type
  {
  };

  template <typename Statement, typename OnConflictUpdate, typename Expression>
  struct consistency_check<Statement, postgresql::on_conflict_do_update_where_t<OnConflictUpdate, Expression>>
  {
#warning: is this correct?
    using type = consistent_t;
  };

  template <typename Statement, typename OnConflict, typename... Assignments>
  struct consistency_check<Statement, postgresql::on_conflict_do_update_t<OnConflict, Assignments...>>
  {
#warning: is this correct?
    using type = consistent_t;
  };

  template <typename Statement, typename OnConflict, typename... Assignments>
  struct clause_base<postgresql::on_conflict_do_update_t<OnConflict, Assignments...>, Statement> : public clause_data<postgresql::on_conflict_do_update_t<OnConflict, Assignments...>, Statement>
  {
    using clause_data<postgresql::on_conflict_do_update_t<OnConflict, Assignments...>, Statement>::clause_data;

      // WHERE
      template <typename Expression>
      auto where(Expression expression) const -> decltype(new_statement(
          *this, postgresql::on_conflict_do_update_where_t<postgresql::on_conflict_do_update_t<OnConflict, Assignments...>, Expression>{this->_data, std::move(expression)}))
      {
        return new_statement(
            *this, postgresql::on_conflict_do_update_where_t<postgresql::on_conflict_do_update_t<OnConflict, Assignments...>, Expression>{this->_data, std::move(expression)});
      }
    };
  template <typename OnConflict, typename... Assignments>
  auto to_sql_string(postgresql::context_t& context,
                     const postgresql::on_conflict_do_update_t<OnConflict, Assignments...>& o) -> std::string
  {
    return to_sql_string(context, o._on_conflict) + "DO UPDATE SET " +
           tuple_to_sql_string(context, o._assignments, tuple_operand{", "});
  }

  template <typename OnConflict, typename Expression, typename... Assignments>
  auto to_sql_string(
      postgresql::context_t& context,
      const postgresql::on_conflict_do_update_where_t<OnConflict, Expression, Assignments...>& t)
      -> std::string
  {
    return to_sql_string(context, t._on_conflict_update) + " WHERE " + to_sql_string(context, t._expression);
  }
}  // namespace sqlpp

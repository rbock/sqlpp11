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

#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/clause/where.h>

namespace sqlpp
{
  SQLPP_VALUE_TRAIT_GENERATOR(is_on_conflict_do_update)

  namespace postgresql
  {
    // Assignments data
    template <typename ConflictTarget, typename... Assignments>
    struct on_conflict_do_update_data_t
    {
      on_conflict_do_update_data_t(on_conflict_data_t<ConflictTarget> conflict_target,
                                   std::tuple<Assignments...> assignments)
          : _conflict_target(conflict_target), _assignments(assignments)
      {
      }

      on_conflict_do_update_data_t(const on_conflict_do_update_data_t&) = default;
      on_conflict_do_update_data_t(on_conflict_do_update_data_t&&) = default;
      on_conflict_do_update_data_t& operator=(const on_conflict_do_update_data_t&) = default;
      on_conflict_do_update_data_t& operator=(on_conflict_do_update_data_t&&) = default;
      ~on_conflict_do_update_data_t() = default;

      on_conflict_data_t<ConflictTarget> _conflict_target;
      std::tuple<Assignments...> _assignments;
    };

    // Where data
    template <typename ConflictTarget, typename Expression, typename... Assignments>
    struct on_conflict_do_update_where_data_t
    {
      on_conflict_do_update_where_data_t(
          Expression expression, on_conflict_do_update_data_t<ConflictTarget, Assignments...> assignments)
          : _expression(expression), _assignments(assignments)
      {
      }

      on_conflict_do_update_where_data_t(const on_conflict_do_update_where_data_t&) = default;
      on_conflict_do_update_where_data_t(on_conflict_do_update_where_data_t&&) = default;
      on_conflict_do_update_where_data_t& operator=(const on_conflict_do_update_where_data_t&) = default;
      on_conflict_do_update_where_data_t& operator=(on_conflict_do_update_where_data_t&&) = default;

      Expression _expression;
      on_conflict_do_update_data_t<ConflictTarget, Assignments...> _assignments;
    };

    // extra where statement
    template <typename ConflictTarget, typename Expression, typename... Assignments>
    struct on_conflict_do_update_where_t
    {
      using _traits = make_traits<no_value_t, tag::is_on_conflict_do_update>;
      using _nodes = sqlpp::detail::type_vector<ConflictTarget, Expression, Assignments...>;

      using _data_t = on_conflict_do_update_where_data_t<ConflictTarget, Expression, Assignments...>;

      // Base template to be inherited by the statement
      template <typename Policies>
      struct _base_t
      {
        _base_t(_data_t data) : _data{std::move(data)}
        {
        }

        _data_t _data;

        using _consistency_check = consistent_t;
      };
    };

    // Use the update_list
    template <typename ConflictTarget, typename... Assignments>
    struct on_conflict_do_update_t
    {
      using _traits = make_traits<no_value_t, tag::is_on_conflict_do_update>;
      using _nodes = sqlpp::detail::type_vector<ConflictTarget, Assignments...>;

      // Data
      using _data_t = on_conflict_do_update_data_t<ConflictTarget, Assignments...>;

      // Base template to be inherited by the statement
      template <typename Policies>
      struct _base_t
      {
        _base_t(_data_t data) : _data{std::move(data)}
        {
        }

        _data_t _data;

        using _consistency_check = consistent_t;

        template <typename Check, typename T>
        using _new_statement_t = new_statement_t<Check, Policies, on_conflict_do_update_t, T>;

        // WHERE
        template <typename Expression>
        auto where(Expression expression) const
            -> _new_statement_t<check_where_static_t<Expression>,
                                on_conflict_do_update_where_t<ConflictTarget, Expression, Assignments...>>
        {
          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  on_conflict_do_update_where_data_t<ConflictTarget, Expression, Assignments...>(
                      expression, _data)};
        }
      };
    };

    template <typename ConflictTarget, typename... Assignments>
    postgresql::context_t& serialize(
        const postgresql::on_conflict_do_update_data_t<ConflictTarget, Assignments...>& o,
        postgresql::context_t& context)
    {
      serialize(context, o._conflict_target);
      context << "DO UPDATE SET ";
      interpret_tuple(o._assignments, ",", context);
      return context;
    }

    template <typename ConflictTarget, typename Expression, typename... Assignments>
    postgresql::context_t& serialize(
        const postgresql::on_conflict_do_update_where_data_t<ConflictTarget, Expression, Assignments...>& o,
        postgresql::context_t& context)
    {
      serialize(context, o._assignments);
      context << " WHERE ";
      serialize(context, o._expression);
      return context;
    }
  }  // namespace postgresql
}  // namespace sqlpp

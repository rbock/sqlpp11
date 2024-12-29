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

#warning: Should these go into clause?
#include <sqlpp11/postgresql/on_conflict_do_nothing.h>
#include <sqlpp11/postgresql/on_conflict_do_update.h>
#include <sqlpp11/core/query/statement.h>
#include <sqlpp11/core/clause/clause_base.h>

namespace sqlpp
{

  namespace postgresql
  {
    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_action_t,
                                 "either do_nothing() or do_update(...) is required with on_conflict");

#warning: Move into member function
    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_do_update_set_assignments_t,
                                 "at least one argument is not an assignment in do_update()");
    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_do_update_set_no_duplicates_t,
                                 "at least one duplicate column detected in do_update()");
    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_do_update_set_single_table_t,
                                 "do_update() contains assignments for columns from more than one table");
    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_do_update_set_count_args_t,
                                 "at least one assignment expression required in do_update()");

    template <typename... Assignments>
    using check_on_conflict_do_update_set_t = static_combined_check_t<
        static_check_t<logic::all<sqlpp::is_assignment<Assignments>::type::value...>::value,
                       assert_on_conflict_do_update_set_assignments_t>,
        static_check_t<not sqlpp::detail::has_duplicates<typename lhs<Assignments>::type...>::value,
                       assert_on_conflict_do_update_set_no_duplicates_t>,
        static_check_t<sizeof...(Assignments) == 0 or sqlpp::detail::make_joined_set_t<required_tables_of_t<
                                                          typename lhs<Assignments>::type>...>::size() == 1,
                       assert_on_conflict_do_update_set_single_table_t>>;

    template <typename... Assignments>
    struct check_on_conflict_do_update_static_set
    {
      using type = static_combined_check_t<
          check_on_conflict_do_update_set_t<Assignments...>,
          static_check_t<sizeof...(Assignments) != 0, assert_on_conflict_do_update_set_count_args_t>>;
    };

    template <typename... Assignments>
    using check_on_conflict_do_update_static_set_t =
        typename check_on_conflict_do_update_static_set<Assignments...>::type;

    template <typename ConflictTarget>
    struct on_conflict_t
    {
      on_conflict_t(ConflictTarget column)
          : _column(column)
      {
      }

      on_conflict_t(const on_conflict_t&) = default;
      on_conflict_t(on_conflict_t&&) = default;
      on_conflict_t& operator=(const on_conflict_t&) = default;
      on_conflict_t& operator=(on_conflict_t&&) = default;
      ~on_conflict_t() = default;

      ConflictTarget _column;
    };

  }  // namespace postgresql

  template <typename Statement, typename ConflictTarget>
  struct consistency_check<Statement, postgresql::on_conflict_t<ConflictTarget>>
  {
    using type = postgresql::assert_on_conflict_action_t;
  };

  template <typename Statement, typename ConflictTarget>
  struct clause_base<postgresql::on_conflict_t<ConflictTarget>, Statement> : public clause_data<postgresql::on_conflict_t<ConflictTarget>, Statement>
  {
    using clause_data<postgresql::on_conflict_t<ConflictTarget>, Statement>::clause_data;

    // DO NOTHING
    auto do_nothing() const -> decltype(new_statement(
        *this, postgresql::on_conflict_do_nothing_t<postgresql::on_conflict_t<ConflictTarget>>{this->_data}))
    {
      return new_statement(
          *this, postgresql::on_conflict_do_nothing_t<postgresql::on_conflict_t<ConflictTarget>>{this->_data});
    }

    // DO UPDATE
    template <typename... Assignments>
    auto do_update(Assignments... assignments) const -> decltype(new_statement(
        *this,
        postgresql::on_conflict_do_update_t<postgresql::on_conflict_t<ConflictTarget>, Assignments...>{
            this->_data, std::make_tuple(std::move(assignments)...)}))
    {
      static_assert(is_column<ConflictTarget>::value, "conflict_target specification is required with do_update(...)");
      return new_statement(
          *this, postgresql::on_conflict_do_update_t<postgresql::on_conflict_t<ConflictTarget>, Assignments...>{
                     this->_data, std::make_tuple(std::move(assignments)...)});
    }
  };

  namespace postgresql
  {
    struct no_on_conflict_t
    {
    };

#warning: It might make sense to always call ::sqlpp::to_sql_string. And then these should be in namespace sqlpp
    // Serialization
    inline auto to_sql_string(postgresql::context_t&, const postgresql::no_on_conflict_t&) -> std::string
    {
      return "";
    }

    template <typename ConflictTarget>
    auto to_sql_string(postgresql::context_t& context, const postgresql::on_conflict_t<ConflictTarget>&) -> std::string
    {
      return " ON CONFLICT (" + name_to_sql_string(context, name_tag_of_t<ConflictTarget>{}) + ") ";
    }

    inline auto to_sql_string(postgresql::context_t&, const postgresql::on_conflict_t<no_data_t>&) -> std::string
    {
      return " ON CONFLICT ";
    }
  }  // namespace postgresql

  template <typename Statement>
  struct consistency_check<Statement, postgresql::no_on_conflict_t>
  {
    using type = consistent_t;
  };

  template <typename Expression>
  struct is_clause<postgresql::on_conflict_t<Expression>> : public std::true_type
  {
  };

  template <typename Statement>
  struct clause_base<postgresql::no_on_conflict_t, Statement> : public clause_data<postgresql::no_on_conflict_t, Statement>
  {
    using clause_data<postgresql::no_on_conflict_t, Statement>::clause_data;

    auto on_conflict() const -> decltype(new_statement(*this, postgresql::on_conflict_t<no_data_t>{no_data_t{}}))
    {
      return new_statement(*this, postgresql::on_conflict_t<no_data_t>{no_data_t{}});
    }

#warning : Allow for more than one column, see #586
    template <typename ConflictTarget>
    auto on_conflict(ConflictTarget column) const -> decltype(new_statement(*this, postgresql::on_conflict_t<ConflictTarget>{std::move(column)}))
    {
      static_assert(is_column<ConflictTarget>::value, "only a column is supported as conflict_target specification");
      return new_statement(*this, postgresql::on_conflict_t<ConflictTarget>{std::move(column)});
    }
  };

}  // namespace sqlpp

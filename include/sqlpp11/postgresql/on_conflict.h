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

#include <sqlpp11/postgresql/on_conflict_do_nothing.h>
#include <sqlpp11/postgresql/on_conflict_do_update.h>
#include <sqlpp11/statement.h>

namespace sqlpp
{
  SQLPP_VALUE_TRAIT_GENERATOR(is_on_conflict);

  SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_action_t,
                               "either do_nothing() or do_update(...) is required with on_conflict");

  namespace postgresql
  {
    template <typename ConflictTarget>
    struct on_conflict_data_t
    {
      on_conflict_data_t(ConflictTarget column) : _column(column)
      {
      }

      on_conflict_data_t(const on_conflict_data_t&) = default;
      on_conflict_data_t(on_conflict_data_t&&) = default;
      on_conflict_data_t& operator=(const on_conflict_data_t&) = default;
      on_conflict_data_t& operator=(on_conflict_data_t&&) = default;
      ~on_conflict_data_t() = default;

      ConflictTarget _column;
    };

    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_do_update_set_assignments_t,
                                 "at least one argument is not an assignment in do_update()");
    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_do_update_set_no_duplicates_t,
                                 "at least one duplicate column detected in do_update()");
    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_do_update_set_allowed_t,
                                 "at least one assignment is prohibited by its column definition in do_update()");
    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_do_update_set_single_table_t,
                                 "do_update() contains assignments for columns from more than one table");
    SQLPP_PORTABLE_STATIC_ASSERT(assert_on_conflict_do_update_set_count_args_t,
                                 "at least one assignment expression required in do_update()");

    namespace detail
    {
      template <typename Assignment>
      struct lhs_must_not_update
      {
        static constexpr auto value = sqlpp::detail::must_not_update_impl<typename lhs<Assignment>::type>::type::value;
      };
    }  // namespace detail

    template <typename... Assignments>
    using check_on_conflict_do_update_set_t = static_combined_check_t<
        static_check_t<logic::all_t<sqlpp::detail::is_assignment_impl<Assignments>::type::value...>::value,
                       assert_on_conflict_do_update_set_assignments_t>,
        static_check_t<not sqlpp::detail::has_duplicates<typename lhs<Assignments>::type...>::value,
                       assert_on_conflict_do_update_set_no_duplicates_t>,
        static_check_t<logic::none_t<detail::lhs_must_not_update<Assignments>::value...>::value,
                       assert_on_conflict_do_update_set_allowed_t>,
        static_check_t<sizeof...(Assignments) == 0 or sqlpp::detail::make_joined_set_t<required_tables_of<
                                                          typename lhs<Assignments>::type>...>::size::value == 1,
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
      using _traits = make_traits<no_value_t, tag::is_noop>;
      using _nodes = sqlpp::detail::type_vector<ConflictTarget>;

      using _data_t = on_conflict_data_t<ConflictTarget>;

      template <typename Policies>
      struct _impl_t
      {
        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
        _impl_t() = default;
        _impl_t(const _data_t& data) : _data(data)
        {
        }

        _data_t _data;
      };

      template <typename Policies>
      struct _base_t
      {
        using _data_t = on_conflict_data_t<ConflictTarget>;

        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
        template <typename... Args>
        _base_t(Args&&... args) : on_conflict{std::forward<Args>(args)...}
        {
        }

        _impl_t<Policies> on_conflict;
        _impl_t<Policies>& operator()()
        {
          return on_conflict;
        }
        const _impl_t<Policies>& operator()() const
        {
          return on_conflict;
        }

        template <typename T>
        static auto _get_member(T t) -> decltype(t.on_conflict)
        {
          return t.on_conflict;
        }

        template <typename Check, typename T>
        using _new_statement_t = new_statement_t<Check, Policies, on_conflict_t, T>;

        using _consistency_check = assert_on_conflict_action_t;

        // DO NOTHING
        auto do_nothing() const -> _new_statement_t<consistent_t, on_conflict_do_nothing_t<ConflictTarget>>
        {
          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  on_conflict_do_nothing_data_t<ConflictTarget>{on_conflict._data}};
        }

        // DO UPDATE
        template <typename... Assignments>
        auto do_update(Assignments... assignments) const
            -> _new_statement_t<check_on_conflict_do_update_static_set_t<Assignments...>,
                                on_conflict_do_update_t<void, ConflictTarget, Assignments...>>
        {
          static_assert(is_column_t<ConflictTarget>::value,
                        "conflict_target specification is required with do_update(...)");
          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  on_conflict_do_update_data_t<void, ConflictTarget, Assignments...>(on_conflict._data,
                                                                                     std::make_tuple(assignments...))};
        }
      };
    };

    struct no_on_conflict_t
    {
      using _traits = make_traits<no_value_t, tag::is_on_conflict>;
      using _nodes = ::sqlpp::detail::type_vector<>;

      using _data_t = no_data_t;

      template <typename Policies>
      struct _impl_t
      {
        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
        _impl_t() = default;
        _impl_t(const _data_t& data) : _data(data)
        {
        }

        _data_t _data;
      };

      template <typename Policies>
      struct _base_t
      {
        using _data_t = no_data_t;

        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
        template <typename... Args>
        _base_t(Args&&... args) : no_on_conflict{std::forward<Args>(args)...}
        {
        }

        _impl_t<Policies> no_on_conflict;
        _impl_t<Policies>& operator()()
        {
          return no_on_conflict;
        }
        const _impl_t<Policies>& operator()() const
        {
          return no_on_conflict;
        }

        template <typename T>
        static auto _get_member(T t) -> decltype(t.no_on_conflict)
        {
          return t.no_on_conflict;
        }

        using _database_t = typename Policies::_database_t;

        template <typename Check, typename T>
        using _new_statement_t = new_statement_t<Check, Policies, no_on_conflict_t, T>;

        using _consistency_check = consistent_t;

        auto on_conflict() const -> _new_statement_t<consistent_t, on_conflict_t<no_data_t>>
        {
          return {static_cast<const derived_statement_t<Policies>&>(*this), on_conflict_data_t<no_data_t>{no_data_t{}}};
        }

        template <typename ConflictTarget>
        auto on_conflict(ConflictTarget column) const -> _new_statement_t<consistent_t, on_conflict_t<ConflictTarget>>
        {
          static_assert(is_column_t<ConflictTarget>::value,
                        "only a column is supported as conflict_target specification");
          return {static_cast<const derived_statement_t<Policies>&>(*this), on_conflict_data_t<ConflictTarget>{column}};
        }
      };
    };

    template <typename ConflictTarget>
    postgresql::context_t& serialize(const postgresql::on_conflict_data_t<ConflictTarget>&,
                                     postgresql::context_t& context)
    {
      context << " ON CONFLICT (";
      context << name_of<ConflictTarget>::template char_ptr<postgresql::context_t>();
      context << ") ";
      return context;
    }

    inline postgresql::context_t& serialize(const postgresql::on_conflict_data_t<no_data_t>&, postgresql::context_t& context)
    {
      context << " ON CONFLICT ";
      return context;
    }
  }  // namespace postgresql
}  // namespace sqlpp

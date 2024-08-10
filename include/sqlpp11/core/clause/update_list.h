#pragma once

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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/interpret_tuple.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  // UPDATE ASSIGNMENTS DATA
  template <typename... Assignments>
  struct update_list_data_t
  {
    update_list_data_t(std::tuple<Assignments...> assignments) : _assignments(assignments)
    {
    }

    update_list_data_t(const update_list_data_t&) = default;
    update_list_data_t(update_list_data_t&&) = default;
    update_list_data_t& operator=(const update_list_data_t&) = default;
    update_list_data_t& operator=(update_list_data_t&&) = default;
    ~update_list_data_t() = default;

    std::tuple<Assignments...> _assignments;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_update_assignments_t,
      "at least one update assignment requires a table which is otherwise not known in the statement");

  // UPDATE ASSIGNMENTS
  template <typename... Assignments>
  struct update_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_update_list>;
    using _nodes = detail::type_vector<Assignments...>;

    // Data
    using _data_t = update_list_data_t<Assignments...>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<update_list_t>::value,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_update_assignments_t>::type;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_update_assignments_t, "update assignments required, i.e. set(...)");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_update_set_assignments_t, "at least one argument is not an assignment in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_update_set_no_duplicates_t, "at least one duplicate column detected in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_update_set_allowed_t,
                               "at least one assignment is prohibited by its column definition in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_update_set_single_table_t,
                               "set() contains assignments for columns from more than one table");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_update_set_count_args_t, "at least one assignment expression required in set()");

  template <typename... Assignments>
  using check_update_set_t = static_combined_check_t<
      static_check_t<logic::all_t<is_assignment<Assignments>::value...>::value,
                     assert_update_set_assignments_t>,
      static_check_t<not detail::has_duplicates<typename lhs<Assignments>::type...>::value,
                     assert_update_set_no_duplicates_t>,
      static_check_t<sizeof...(Assignments) == 0 or detail::make_joined_set_t<required_tables_of_t<
                                                        typename lhs<Assignments>::type>...>::size::value == 1,
                     assert_update_set_single_table_t>>;

  template <typename... Assignments>
  struct check_update_static_set
  {
    using type = static_combined_check_t<check_update_set_t<Assignments...>,
                                         static_check_t<sizeof...(Assignments) != 0, assert_update_set_count_args_t>>;
  };

  template <typename... Assignments>
  using check_update_static_set_t = typename check_update_static_set<Assignments...>::type;

  struct no_update_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_where>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t() = default;
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_update_list_t, T>;

      using _consistency_check = assert_update_assignments_t;

      template <typename... Assignments>
      auto set(Assignments... assignments) const
          -> _new_statement_t<check_update_static_set_t<Assignments...>, update_list_t<Assignments...>>
      {
        using Check = check_update_static_set_t<Assignments...>;
        return _set_impl(Check{}, std::make_tuple(assignments...));
      }

      template <typename... Assignments>
      auto set(std::tuple<Assignments...> assignments) const
          -> _new_statement_t<check_update_static_set_t<Assignments...>, update_list_t<Assignments...>>
      {
        using Check = check_update_static_set_t<Assignments...>;
        return _set_impl(Check{}, assignments);
      }

    private:
      template <typename Check, typename... Assignments>
      auto _set_impl(Check, Assignments... assignments) const -> inconsistent<Check>;

      template <typename... Assignments>
      auto _set_impl(consistent_t /*unused*/, std::tuple<Assignments...> assignments) const
          -> _new_statement_t<consistent_t, update_list_t<Assignments...>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                update_list_data_t<Assignments...>{assignments}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename... Assignments>
  auto to_sql_string(Context& context, const update_list_data_t<Assignments...>& t) -> std::string
  {
    return " SET " + interpret_tuple(t._assignments, ",", context);
  }
}  // namespace sqlpp

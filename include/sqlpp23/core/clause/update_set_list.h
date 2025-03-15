#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
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

#include <sqlpp23/core/detail/type_set.h>
#include <sqlpp23/core/query/dynamic.h>
#include <sqlpp23/core/static_assert.h>
#include <sqlpp23/core/query/statement.h>
#include <sqlpp23/core/tuple_to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename... Assignments>
struct update_set_list_t {
  update_set_list_t(std::tuple<Assignments...> assignments)
      : _assignments(std::move(assignments)) {}
  update_set_list_t(const update_set_list_t&) = default;
  update_set_list_t(update_set_list_t&&) = default;
  update_set_list_t& operator=(const update_set_list_t&) = default;
  update_set_list_t& operator=(update_set_list_t&&) = default;
  ~update_set_list_t() = default;

  std::tuple<Assignments...> _assignments;
};

  template <typename Context, typename... Assignments>
  auto to_sql_string(Context& context, const update_set_list_t<Assignments...>& t)
      -> std::string {
    return " SET " + tuple_to_sql_string(context, t._assignments,
                                         tuple_operand_no_dynamic{", "});
  }

SQLPP_WRAPPED_STATIC_ASSERT(assert_no_unknown_tables_in_update_assignments_t,
                            "at least one update assignment requires a table "
                            "which is otherwise not known in the statement");

template <typename... Assignments>
struct is_clause<update_set_list_t<Assignments...>> : public std::true_type {};

template <typename Statement, typename... Assignments>
struct consistency_check<Statement, update_set_list_t<Assignments...>> {
  using type = consistent_t;
};

template <typename Statement, typename... Assignments>
struct prepare_check<Statement, update_set_list_t<Assignments...>> {
  using type = std::conditional_t<
      Statement::template _no_unknown_tables<update_set_list_t<Assignments...>>,
      consistent_t, assert_no_unknown_tables_in_update_assignments_t>;
};

template <typename... Assignments>
struct nodes_of<update_set_list_t<Assignments...>> {
  using type = detail::type_vector<Assignments...>;
};

struct no_update_set_list_t {
  template <typename Statement, DynamicAssignment... Assignments>
  auto set(this Statement &&statement, Assignments... assignments)

  {
    SQLPP_STATIC_ASSERT(sizeof...(Assignments) != 0,
                        "at least one assignment expression required in set()");
    SQLPP_STATIC_ASSERT(not detail::has_duplicates<
                            lhs_t<remove_dynamic_t<Assignments>>...>::value,
                        "at least one duplicate column detected in set()");
    SQLPP_STATIC_ASSERT(
        detail::make_joined_set_t<
            required_tables_of_t<lhs_t<Assignments>>...>::size() == 1,
        "set() contains assignments for columns from more than one table");

    return new_statement<no_update_set_list_t>(
        std::forward<Statement>(statement),
        update_set_list_t<Assignments...>{std::make_tuple(assignments...)});
  }

};

  template <typename Context>
  auto to_sql_string(Context&, const no_update_set_list_t&)
      -> std::string {
    return "";
  }

SQLPP_WRAPPED_STATIC_ASSERT(assert_update_assignments_t,
                            "update assignments required, i.e. set(...)");

template <typename Statement>
struct consistency_check<Statement, no_update_set_list_t> {
  using type = assert_update_assignments_t;
};

template <DynamicAssignment... T> auto update_set(T... t) {
  return statement_t<no_update_set_list_t>().set(std::move(t)...);
}

} // namespace sqlpp

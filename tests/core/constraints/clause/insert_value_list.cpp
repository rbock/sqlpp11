/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
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

#include <sqlpp23/tests/core/constraints_helpers.h>

#include <sqlpp23/tests/core/tables.h>

namespace {
template <typename... Expressions>
concept can_call_insert_set_with_standalone =
    requires(Expressions... expressions) { sqlpp::insert_set(expressions...); };
template <typename... Expressions>
concept can_call_insert_set_with_in_statement =
    requires(Expressions... expressions) {
      sqlpp::statement_t<sqlpp::no_insert_value_list_t>{}.set(expressions...);
    };

template <typename... Expressions>
concept can_call_insert_set_with =
    can_call_insert_set_with_standalone<Expressions...> and
    can_call_insert_set_with_in_statement<Expressions...>;

template <typename... Expressions>
concept cannot_call_insert_set_with =
    not(can_call_insert_set_with_standalone<Expressions...> or
        can_call_insert_set_with_in_statement<Expressions...>);

template <typename... Expressions>
concept can_call_insert_columns_with_standalone = requires(
    Expressions... expressions) { sqlpp::insert_columns(expressions...); };
template <typename... Expressions>
concept can_call_insert_columns_with_in_statement = requires(
    Expressions... expressions) {
  sqlpp::statement_t<sqlpp::no_insert_value_list_t>{}.columns(expressions...);
};

template <typename... Expressions>
concept can_call_insert_columns_with =
    can_call_insert_columns_with_standalone<Expressions...> and
    can_call_insert_columns_with_in_statement<Expressions...>;

template <typename... Expressions>
concept cannot_call_insert_columns_with =
    not(can_call_insert_columns_with_standalone<Expressions...> or
        can_call_insert_columns_with_in_statement<Expressions...>);

} // namespace

int main() {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Confirming the required columns of TabBar.
  static_assert(std::is_same<sqlpp::required_insert_columns_of_t<test::TabBar>,
                             sqlpp::detail::type_set<sqlpp::column_t<
                                 test::TabBar, test::TabBar_::BoolNn>>>::value,
                "");

  // -------------------------
  // insert_into(tab).set(...)
  // -------------------------

  // insert_into(table).set(<non arguments>) is inconsistent and cannot be
  // constructed.
  SQLPP_CHECK_STATIC_ASSERT(
      insert_into(bar).set(),
      "at least one assignment expression required in set()");

  // insert_set(<arguments including non-assignments>) is inconsistent and
  // cannot be constructed.
  static_assert(can_call_insert_set_with<decltype(bar.id = 7)>,
                "assignment is OK");
  static_assert(cannot_call_insert_set_with<decltype(bar.id == 7)>,
                "not an assignment: comparison");
  static_assert(
      cannot_call_insert_set_with<decltype(bar.id = 7), decltype(bar.boolNn)>,
      "not an assignment: boolNn");

  // insert_into(table).set(<arguments including non-assignments>) is
  // inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(
      insert_into(bar).set(bar.boolNn = true, bar.boolNn = false),
      "at least one duplicate column detected in set()");
  SQLPP_CHECK_STATIC_ASSERT(
      insert_into(bar).set(bar.boolNn = true,
                           dynamic(true, bar.boolNn = false)),
      "at least one duplicate column detected in set()");

  // insert_into(table).set(<assignments from more than one table>) is
  // inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(
      insert_into(bar).set(foo.id = sqlpp::default_value, bar.boolNn = true),
      "set() arguments must be assignment for exactly one table");
  SQLPP_CHECK_STATIC_ASSERT(
      insert_into(bar).set(dynamic(true, foo.id = sqlpp::default_value),
                           bar.boolNn = true),
      "set() arguments must be assignment for exactly one table");

  // insert_into(table).set(<not all required columns>) is inconsistent but can
  // be constructed (check can only run later)
  {
    auto i = insert_into(bar).set(bar.id = sqlpp::default_value);
    using I = decltype(i);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::assert_all_required_assignments_t>::value,
                  "");
  }
  {
    auto i = insert_into(bar).set(dynamic(true, bar.id = sqlpp::default_value));
    using I = decltype(i);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::assert_all_required_assignments_t>::value,
                  "");
  }

  // insert_into(table).set(<dynamic required columns>) is also inconsistent but
  // can be constructed (check can only run later)
  {
    auto i = insert_into(bar).set(dynamic(true, bar.boolNn = true));
    using I = decltype(i);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::assert_all_required_assignments_t>::value,
                  "");
  }

  // -------------------------
  // insert_into(tab).columns(...)
  // -------------------------

  // insert_into(table).columns(<non arguments>) is inconsistent and cannot be
  // constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(),
                            "at least one column required in columns()");

  // insert_into(table).columns(<arguments including non-columns>) is
  // inconsistent and cannot be constructed.
  static_assert(can_call_insert_columns_with<decltype(bar.id)>,
                "OK, argument is a column");
  static_assert(cannot_call_insert_columns_with<decltype(bar.id = 7)>,
                "OK, argument is not a column");

  // insert_into(table).columns(<arguments including non-columns>) is
  // inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(
      insert_into(bar).columns(bar.boolNn, bar.id, bar.boolNn),
      "at least one duplicate column detected in columns()");
  SQLPP_CHECK_STATIC_ASSERT(
      insert_into(bar).columns(bar.boolNn, bar.id, dynamic(true, bar.boolNn)),
      "at least one duplicate column detected in columns()");

  // insert_into(table).columns(<columns from more than one table>) is
  // inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(foo.id, bar.boolNn),
                            "columns() contains columns from several tables");
  SQLPP_CHECK_STATIC_ASSERT(
      insert_into(bar).columns(dynamic(true, foo.id), bar.boolNn),
      "columns() contains columns from several tables");
  SQLPP_CHECK_STATIC_ASSERT(
      insert_into(bar).columns(foo.id, dynamic(true, bar.boolNn)),
      "columns() contains columns from several tables");

  // insert_into(table).columns(<not all required columns>) is inconsistent but
  // can be constructed (check can only run later)
  {
    auto i = insert_into(bar).columns(bar.id);
    using I = decltype(i);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::assert_all_required_columns_t>::value,
                  "");
  }
  {
    auto i = insert_into(bar).columns(dynamic(true, bar.id));
    using I = decltype(i);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::assert_all_required_columns_t>::value,
                  "");
  }

  // insert_into(table).columns(<dynamic required columns>) is also inconsistent
  // but can be constructed (check can only run later)
  {
    auto i = insert_into(bar).columns(dynamic(true, bar.boolNn));
    using I = decltype(i);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::assert_all_required_columns_t>::value,
                  "");
  }

  // -------------------------
  // insert_into(tab).columns(...).add_value(...)
  // -------------------------
  {
    auto i = insert_into(bar).columns(bar.id, bar.boolNn);
    i.add_values(bar.id = sqlpp::default_value, bar.boolNn = true); // OK
    i.add_values(bar.id = 7, bar.boolNn = true);                    // OK

    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(bar.boolNn = true),
        "add_values() arguments have to match columns() arguments");
    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(bar.id = sqlpp::default_value),
        "add_values() arguments have to match columns() arguments");

    SQLPP_CHECK_STATIC_ASSERT(i.add_values(bar.id = sqlpp::default_value,
                                           bar.boolNn = not bar.boolNn),
                              "add_values() arguments must not be expressions");

    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(bar.id = parameter(bar.id), bar.boolNn = bar.boolNn),
        "add_values() arguments must not contain parameters");

    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(bar.id = bar.id, bar.boolNn = bar.boolNn),
        "add_values() arguments must not have names");
  }

  {
    auto i = insert_into(bar).columns(dynamic(true, bar.id), bar.boolNn);
    i.add_values(dynamic(true, bar.id = sqlpp::default_value),
                 bar.boolNn = true); // OK

    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(bar.boolNn = true),
        "add_values() arguments have to match columns() arguments");
    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(bar.id = sqlpp::default_value),
        "add_values() arguments have to match columns() arguments");
    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(bar.id = sqlpp::default_value, bar.boolNn = true),
        "add_values() arguments have to match columns() arguments");

    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(dynamic(true, bar.id = bar.id + 1), bar.boolNn = true),
        "add_values() arguments must not be expressions");

    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(dynamic(true, bar.id = sqlpp::default_value),
                     bar.boolNn = parameter(bar.boolNn)),
        "add_values() arguments must not contain parameters");

    SQLPP_CHECK_STATIC_ASSERT(
        i.add_values(dynamic(true, bar.id = bar.id), bar.boolNn = true),
        "add_values() arguments must not have names");
  }
}

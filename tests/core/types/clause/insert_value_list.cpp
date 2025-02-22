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

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/tests/core/types_helpers.h>

void test_no_insert_value_list() {
  const auto foo = test::TabFoo{};
  {
    using I = decltype(insert_into(foo));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::assert_insert_values_t>::value);
  }
}

void test_required_insert_columns_of() {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  using RicoFoo = sqlpp::required_insert_columns_of_t<test::TabFoo>;
  using RicoBar = sqlpp::required_insert_columns_of_t<test::TabBar>;

  static_assert(RicoFoo::empty(), "");
  static_assert(not RicoBar::empty(), "");

  {
    using I = decltype(insert_into(foo));
    using RicoI = sqlpp::required_insert_columns_of_t<I>;

    static_assert(std::is_same<RicoI, RicoFoo>::value);
  }

  {
    using I = decltype(insert_into(bar));
    using RicoI = sqlpp::required_insert_columns_of_t<I>;

    static_assert(std::is_same<RicoI, RicoBar>::value);
  }
}

void test_insert_set() {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Confirming the required columns of TabBar.
  static_assert(std::is_same<sqlpp::required_insert_columns_of_t<test::TabBar>,
                             sqlpp::detail::type_set<sqlpp::column_t<
                                 test::TabBar, test::TabBar_::BoolNn>>>::value,
                "");

  // Test nodes_of
  {
    using I = extract_clause_t<decltype(insert_set(bar.boolNn = true))>;
    using A = decltype(bar.boolNn = true);
    static_assert(std::is_same<sqlpp::nodes_of_t<I>,
                               sqlpp::detail::type_vector<A>>::value,
                  "");
  }

  // insert_into(table).set(<all required tables>) is consistent
  {
    using I = decltype(insert_into(bar).set(bar.boolNn = true));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::consistent_t>::value);
  }

  // insert_into(table).set(<all required tables> plus some more) is consistent
  {
    using I = decltype(insert_into(bar).set(bar.id = sqlpp::default_value,
                                            bar.boolNn = true));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::consistent_t>::value);
  }

  // insert_into(tableA).set(assignments for tableB) not inconsistent
  {
    using I = decltype(insert_into(foo).set(bar.id = sqlpp::default_value,
                                            bar.boolNn = true));
    static_assert(
        std::is_same<
            sqlpp::statement_consistency_check_t<I>,
            sqlpp::assert_no_unknown_tables_in_insert_assignments_t>::value);
  }

  // insert_into(tableA).set(missing required assignments) not inconsistent
  {
    using I = decltype(insert_into(bar).set(bar.id = sqlpp::default_value));
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<I>,
                     sqlpp::assert_all_required_assignments_t>::value);
  }
  {
    using I = decltype(insert_into(bar).set(bar.id = sqlpp::default_value,
                                            dynamic(true, bar.boolNn = true)));
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<I>,
                     sqlpp::assert_all_required_assignments_t>::value);
  }
}

void test_insert_columns() {
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Test nodes_of
  {
    using I = extract_clause_t<decltype(insert_columns(bar.id, bar.boolNn))>;
    using A = decltype(bar.id);
    using B = decltype(bar.boolNn);
    static_assert(std::is_same<sqlpp::nodes_of_t<I>,
                               sqlpp::detail::type_vector<A, B>>::value,
                  "");
  }

  // insert_into(tableA).columns(decent set of columns from tableA) is
  // consistent
  {
    using I = decltype(insert_into(bar).columns(bar.id, bar.boolNn));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::consistent_t>::value);
  }
  {
    using I =
        decltype(insert_into(bar).columns(dynamic(true, bar.id), bar.boolNn));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::consistent_t>::value);
  }

  // insert_into(tableA).columns(decent set of columns from tableB) is not
  // consistent
  {
    using I = decltype(insert_into(foo).columns(bar.id, bar.boolNn));
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<I>,
                     sqlpp::assert_no_unknown_tables_in_column_list_t>::value);
  }
  {
    using I =
        decltype(insert_into(foo).columns(dynamic(true, bar.id), bar.boolNn));
    static_assert(
        std::is_same<sqlpp::statement_consistency_check_t<I>,
                     sqlpp::assert_no_unknown_tables_in_column_list_t>::value);
  }

  // insert_into(tableA).columns(missing required static columns) is not
  // consistent
  {
    using I = decltype(insert_into(bar).columns(bar.id));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::assert_all_required_columns_t>::value);
  }
  {
    using I = decltype(insert_into(bar).columns(dynamic(true, bar.boolNn)));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>,
                               sqlpp::assert_all_required_columns_t>::value);
  }
}

int main() {
  void test_no_insert_value_list();
  void test_all_columns_have_default_values();
  void test_insert_set();
  void test_insert_columns();
}

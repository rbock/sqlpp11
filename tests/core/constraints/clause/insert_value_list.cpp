/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define SQLPP_TEST_STATIC_ASSERT

#include "Sample.h"
#include <sqlpp11/sqlpp11.h>
#include "../../../include/test_helpers.h"

#define SQLPP_CHECK_STATIC_ASSERT(expression, message)                             \
  {                                                                                \
    try                                                                            \
    {                                                                              \
      expression;                                                                  \
      std::cerr << __FILE__ << " " << __LINE__ << '\n' << "Expected exception!\n"; \
      return -1;                                                                   \
    }                                                                              \
    catch (const sqlpp::unit_test_exception& e)                                    \
    {                                                                              \
      if (e.what() != sqlpp::string_view(message))                                 \
      {                                                                            \
        std::cerr << __FILE__ << " " << __LINE__ << '\n'                           \
                  << "Expected: -->|" << message << "|<--\n"                       \
                  << "Received: -->|" << e.what() << "|<--\n";                     \
        return -1;                                                                 \
      }                                                                            \
    }                                                                              \
  }

int main()
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Confirming the required columns of TabBar.
  static_assert(std::is_same<test::TabBar::_required_insert_columns,
                             sqlpp::detail::type_set<sqlpp::column_t<test::TabBar, test::TabBar_::BoolNn>>>::value,
                "");

  // -------------------------
  // insert_into(tab).set(...)
  // -------------------------

  // insert_into(table).set(<non arguments>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).set(), "at least one assignment expression required in set()");

#warning: Is there a reasonable way to test this?
  /*
  // insert_into(table).set(<arguments including non-assignments>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).set(bar.id == 7), "at least one argument is not an assignment in set()");
  */

  // insert_into(table).set(<arguments including non-assignments>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).set(bar.boolNn = true, bar.boolNn = false), "at least one duplicate column detected in set()");
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).set(bar.boolNn = true, dynamic(true, bar.boolNn = false)), "at least one duplicate column detected in set()");

  // insert_into(table).set(<assignments from more than one table>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).set(foo.id = sqlpp::default_value, bar.boolNn = true),
                           "set() arguments must be assignment for exactly one table");
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).set(dynamic(true, foo.id = sqlpp::default_value), bar.boolNn = true),
                            "set() arguments must be assignment for exactly one table");

  // insert_into(table).set(<not all required columns>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).set(bar.id = sqlpp::default_value),
                           "at least one required column is missing in set()");
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).set(dynamic(true, bar.id = sqlpp::default_value)),
                           "at least one required column is missing in set()");

  // insert_into(table).set(<dynamic required columns>) is also inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).set(dynamic(true, bar.boolNn = true)),
                           "at least one required column is missing in set()");

  // -------------------------
  // insert_into(tab).columns(...)
  // -------------------------

  // insert_into(table).columns(<non arguments>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(), "at least one column required in columns()");

#warning: Is there a reasonable way to test this?
  /*
  // insert_into(table).columns(<arguments including non-columns>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(bar.id == 7), "at least one argument is not an column in set()");
  */

  // insert_into(table).columns(<arguments including non-columns>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(bar.boolNn, bar.id, bar.boolNn), "at least one duplicate column detected in columns()");
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(bar.boolNn, bar.id, dynamic(true, bar.boolNn)), "at least one duplicate column detected in columns()");

  // insert_into(table).columns(<columns from more than one table>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(foo.id, bar.boolNn),
                           "columns() contains columns from several tables");
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(dynamic(true,foo.id), bar.boolNn),
                           "columns() contains columns from several tables");
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(foo.id, dynamic(true, bar.boolNn)),
                           "columns() contains columns from several tables");

  // insert_into(table).columns(<not all required columns>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(bar.id),
                           "at least one required column is missing in columns()");
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(dynamic(true, bar.id)),
                           "at least one required column is missing in columns()");

  // insert_into(table).columns(<dynamic required columns>) is also inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(insert_into(bar).columns(dynamic(true, bar.boolNn)),
                           "at least one required column is missing in columns()");

#warning: need to add tests for add_values()

}


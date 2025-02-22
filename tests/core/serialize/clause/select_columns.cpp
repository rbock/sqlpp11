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
#include <sqlpp11/tests/core/serialize_helpers.h>
#include <sqlpp11/tests/core/tables.h>

SQLPP_CREATE_NAME_TAG(cheese);
SQLPP_CREATE_NAME_TAG(cake);
SQLPP_CREATE_NAME_TAG(id_count);

int main(int, char *[]) {
  const auto val = sqlpp::value(17);

  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // -----------------------------------------
  // --  SELECT(<columns>)
  // -----------------------------------------
  // Single column
  SQLPP_COMPARE(select(foo.doubleN), "SELECT tab_foo.double_n");

  // Two columns
  SQLPP_COMPARE(select(foo.doubleN, bar.id),
                "SELECT tab_foo.double_n, tab_bar.id");

  // All columns of a table
  SQLPP_COMPARE(
      select(all_of(foo)),
      "SELECT tab_foo.id, tab_foo.text_nn_d, tab_foo.int_n, tab_foo.double_n, "
      "tab_foo.u_int_n, tab_foo.blob_n, tab_foo.bool_n");

  // All columns of a table plus one more
  SQLPP_COMPARE(
      select(all_of(foo), bar.id),
      "SELECT tab_foo.id, tab_foo.text_nn_d, tab_foo.int_n, tab_foo.double_n, "
      "tab_foo.u_int_n, tab_foo.blob_n, tab_foo.bool_n, tab_bar.id");

  // One more, plus all columns of a table
  SQLPP_COMPARE(
      select(bar.id, all_of(foo)),
      "SELECT tab_bar.id, tab_foo.id, tab_foo.text_nn_d, tab_foo.int_n, "
      "tab_foo.double_n, tab_foo.u_int_n, tab_foo.blob_n, tab_foo.bool_n");

  using T = decltype(count(bar.id).as(id_count));
  static_assert(sqlpp::has_value_type_v<
                    sqlpp::remove_as_t<sqlpp::remove_dynamic_t<T>>> and
                sqlpp::has_name_tag_v<sqlpp::remove_dynamic_t<T>>);
  // Column and aggregate function
  SQLPP_COMPARE(select(foo.doubleN, count(bar.id).as(id_count)),
                "SELECT tab_foo.double_n, COUNT(tab_bar.id) AS id_count");

  // Column aliases
  SQLPP_COMPARE(select(foo.doubleN.as(sqlpp::alias::o),
                       count(bar.id).as(sqlpp::alias::a)),
                "SELECT tab_foo.double_n AS o, COUNT(tab_bar.id) AS a");

  // Optional column manually
  SQLPP_COMPARE(select(dynamic(true, bar.id)), "SELECT tab_bar.id");
  SQLPP_COMPARE(select(dynamic(false, bar.id)), "SELECT NULL AS id");

  // -----------------------------------------
  // --  select_columns(<columns>)
  // -----------------------------------------
  // Plain columns.
  SQLPP_COMPARE(select_columns(foo.id), "tab_foo.id");
  SQLPP_COMPARE(select_columns(foo.textNnD), "tab_foo.text_nn_d");
  SQLPP_COMPARE(select_columns(foo.boolN), "tab_foo.bool_n");

  // Multiple plain columns.
  SQLPP_COMPARE(select_columns(foo.id, foo.textNnD, foo.boolN),
                "tab_foo.id, tab_foo.text_nn_d, tab_foo.bool_n");

  // Single expression
  SQLPP_COMPARE(select_columns((foo.id + 17).as(cake)),
                "(tab_foo.id + 17) AS cake");

  // Single dynamic column.
  SQLPP_COMPARE(select_columns(dynamic(true, foo.id)), "tab_foo.id");
  SQLPP_COMPARE(select_columns(dynamic(false, foo.id)), "NULL AS id");
  SQLPP_COMPARE(select_columns(dynamic(false, foo.id.as(cake))),
                "NULL AS cake");

  // Multiple dynamic columns (this is odd if all are dynamic)
  SQLPP_COMPARE(select_columns(dynamic(true, foo.id), foo.textNnD, foo.boolN),
                "tab_foo.id, tab_foo.text_nn_d, tab_foo.bool_n");
  SQLPP_COMPARE(select_columns(foo.id, dynamic(true, foo.textNnD), foo.boolN),
                "tab_foo.id, tab_foo.text_nn_d, tab_foo.bool_n");
  SQLPP_COMPARE(select_columns(foo.id, foo.textNnD, dynamic(true, foo.boolN)),
                "tab_foo.id, tab_foo.text_nn_d, tab_foo.bool_n");

  SQLPP_COMPARE(select_columns(dynamic(false, foo.id), foo.textNnD, foo.boolN),
                "NULL AS id, tab_foo.text_nn_d, tab_foo.bool_n");
  SQLPP_COMPARE(select_columns(foo.id, dynamic(false, foo.textNnD), foo.boolN),
                "tab_foo.id, NULL AS text_nn_d, tab_foo.bool_n");
  SQLPP_COMPARE(select_columns(foo.id, foo.textNnD, dynamic(false, foo.boolN)),
                "tab_foo.id, tab_foo.text_nn_d, NULL AS bool_n");

  SQLPP_COMPARE(select_columns(foo.id, dynamic(false, foo.textNnD),
                               dynamic(false, foo.boolN)),
                "tab_foo.id, NULL AS text_nn_d, NULL AS bool_n");
  SQLPP_COMPARE(select_columns(dynamic(false, foo.id), foo.textNnD,
                               dynamic(false, foo.boolN)),
                "NULL AS id, tab_foo.text_nn_d, NULL AS bool_n");
  SQLPP_COMPARE(select_columns(dynamic(false, foo.id),
                               dynamic(false, foo.textNnD), foo.boolN),
                "NULL AS id, NULL AS text_nn_d, tab_foo.bool_n");

  // Single declared column
  SQLPP_COMPARE(select_columns(declare_group_by_column(val).as(cheese)),
                "17 AS cheese");
  SQLPP_COMPARE(select_columns(declare_group_by_column(foo.id + 17).as(cake)),
                "(tab_foo.id + 17) AS cake");

  // Mixed declared column
  SQLPP_COMPARE(select_columns(foo.id, declare_group_by_column(val).as(cheese)),
                "tab_foo.id, 17 AS cheese");
  SQLPP_COMPARE(select_columns(declare_group_by_column(val).as(cake), foo.id),
                "17 AS cake, tab_foo.id");

  // Mixed dynamic declared column
  SQLPP_COMPARE(
      select_columns(foo.id,
                     dynamic(true, declare_group_by_column(val).as(cheese))),
      "tab_foo.id, 17 AS cheese");
  SQLPP_COMPARE(
      select_columns(dynamic(true, declare_group_by_column(val).as(cake)),
                     foo.id),
      "17 AS cake, tab_foo.id");

  SQLPP_COMPARE(
      select_columns(foo.id,
                     dynamic(false, declare_group_by_column(val).as(cheese))),
      "tab_foo.id, NULL AS cheese");
  SQLPP_COMPARE(
      select_columns(dynamic(false, declare_group_by_column(val).as(cake)),
                     foo.id),
      "NULL AS cake, tab_foo.id");

  return 0;
}

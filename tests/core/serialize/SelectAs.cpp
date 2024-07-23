/*
 * Copyright (c) 2023, Roland Bock
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

#include "compare.h"
#include "Sample.h"
#include <sqlpp11/sqlpp11.h>

#include <iostream>

SQLPP_ALIAS_PROVIDER(id_count)
SQLPP_ALIAS_PROVIDER(cheese)

  using namespace sqlpp;

int SelectAs(int, char*[])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // SELECT...AS as selectable column
  //using S = table_alias_t<cheese_t, select_pseudo_table_t<statement_t<no_with_t, select_t, no_select_flag_list_t, select_column_list_t<as_expression<count_t<noop, column_t<test::TabBar, test::TabBar_::Id> >, id_count_t> >, from_t<test::TabBar>, where_t<unconditional_t>, no_group_by_t, no_having_t, no_order_by_t, no_limit_t, no_offset_t, no_union_t, no_for_update_t>, as_expression<count_t<noop, column_t<test::TabBar, test::TabBar_::Id> >, id_count_t> >, select_column_spec_t<statement_t<no_with_t, select_t, no_select_flag_list_t, select_column_list_t<as_expression<count_t<noop, column_t<test::TabBar, test::TabBar_::Id> >, id_count_t> >, from_t<test::TabBar>, where_t<unconditional_t>, no_group_by_t, no_having_t, no_order_by_t, no_limit_t, no_offset_t, no_union_t, no_for_update_t>, as_expression<count_t<noop, column_t<test::TabBar, test::TabBar_::Id> >, id_count_t> > >;
  using S = select_pseudo_table_t<statement_t<no_with_t, select_t, no_select_flag_list_t, select_column_list_t<as_expression<count_t<noop, column_t<test::TabBar, test::TabBar_::Id> >, id_count_t> >, from_t<test::TabBar>, where_t<unconditional_t>, no_group_by_t, no_having_t, no_order_by_t, no_limit_t, no_offset_t, no_union_t, no_for_update_t>> ;
  static_assert(sqlpp::has_value_type<S>::value, "");
  /*
  sqlpp::name_tag_of_t<decltype(select(count(bar.id).as(id_count)).from(bar).unconditionally())::_result_type_provider>::hansi;
  sqlpp::value_type_of_t<decltype(select(count(bar.id).as(id_count)).from(bar).unconditionally())::_result_type_provider>::hansi;
  static_assert(sqlpp::has_value_type<decltype(select(count(bar.id).as(id_count)).from(bar).unconditionally())>::value, "");
  static_assert(sqlpp::has_name<decltype(select(count(bar.id).as(id_count)).from(bar).unconditionally())>::value, "");
  */
#error: The select itself should not offer an "as" that yields a value.
#error: The id_count should offer the alias that offers the value.
  compare(__LINE__, select(foo.doubleN, select(count(bar.id).as(id_count)).from(bar).unconditionally().as(cheese)),
          "SELECT tab_foo.double_n,(SELECT COUNT(tab_bar.id) AS id_count FROM tab_bar) AS cheese");

  return 0;
}

/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include "Sample.h"
#include "MockDb.h"
#include <sqlpp11/auto_alias.h>
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/clause/select.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/database/connection.h>

#include <iostream>

SQLPP_ALIAS_PROVIDER(kaesekuchen)

int Function(int, char* [])
{
  MockDb db = {};
  const auto f = test::TabFoo{};
  const auto t = test::TabBar{};

  // f.doubleN + 4 *= "";

  // MEMBER FUNCTIONS
  // ----------------

  // Test in
  {
    using TI = decltype(t.id.in(1, 2, 3));
    using TF = decltype(f.doubleN.in(1.0, 2.0, 3.0));
    using TT = decltype(t.textN.in("a", "b", "c"));
    static_assert(sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test in with value list
  {
    using TI = decltype(t.id.in(sqlpp::value_list(std::vector<int>({1, 2, 3}))));
    using TF = decltype(f.doubleN.in(sqlpp::value_list(std::vector<float>({1.0, 2.0, 3.0}))));
    using TT = decltype(t.textN.in(sqlpp::value_list(std::vector<std::string>({"a", "b", "c"}))));
    static_assert(sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test not_in
  {
    using TI = decltype(t.id.not_in(1, 2, 3));
    using TF = decltype(f.doubleN.not_in(1.0, 2.0, 3.0));
    using TT = decltype(t.textN.not_in("a", "b", "c"));
    static_assert(sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test not in with value list
  {
    using TI = decltype(t.id.not_in(sqlpp::value_list(std::vector<int>({1, 2, 3}))));
    using TF = decltype(f.doubleN.not_in(sqlpp::value_list(std::vector<float>({1.0, 2.0, 3.0}))));
    using TT = decltype(t.textN.not_in(sqlpp::value_list(std::vector<std::string>({"a", "b", "c"}))));
    static_assert(sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test like
  {
    using TT = decltype(t.textN.like("%c%"));
    static_assert(sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test is_null
  {
    using TI = decltype(t.id.is_null());
    using TF = decltype(f.doubleN.is_null());
    using TT = decltype(t.textN.is_null());
    using TTI = decltype(is_null(t.id));
    using TTF = decltype(is_null(f.doubleN));
    using TTT = decltype(is_null(t.textN));
    static_assert(std::is_same<TI, TTI>::value, "type requirement");
    static_assert(std::is_same<TF, TTF>::value, "type requirement");
    static_assert(std::is_same<TT, TTT>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test is_not_null
  {
    using TI = decltype(t.id.is_not_null());
    using TF = decltype(f.doubleN.is_not_null());
    using TT = decltype(t.textN.is_not_null());
    using TTI = decltype(is_not_null(t.id));
    using TTF = decltype(is_not_null(f.doubleN));
    using TTT = decltype(is_not_null(t.textN));
    static_assert(std::is_same<TI, TTI>::value, "type requirement");
    static_assert(std::is_same<TF, TTF>::value, "type requirement");
    static_assert(std::is_same<TT, TTT>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // SUB_SELECT_FUNCTIONS
  // --------------------

  // Test exists
  {
    using TI = decltype(exists(select(t.id).from(t)));
    using TT = decltype(exists(select(t.textN).from(t)));
    static_assert(sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");

    if (false and db(select(exists(select(t.id).from(t).unconditionally()))).front().exists)
    { /* do something */
    }
  }

  // Test any
  {
    using S = decltype(select(t.id).from(t));
    static_assert(sqlpp::is_numeric_t<S>::value, "type requirement");

    using TI = decltype(any(select(t.id).from(t)));
    using TT = decltype(any(select(t.textN).from(t)));
    using TF = decltype(any(select(f.doubleN).from(f)));
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_multi_expression_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_multi_expression_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TF>::value, "tFpe requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_multi_expression_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_integral_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test some
  {
    using TI = decltype(some(select(t.id).from(t)));
    using TT = decltype(some(select(t.textN).from(t)));
    using TF = decltype(some(select(f.doubleN).from(f)));
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_multi_expression_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_multi_expression_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_multi_expression_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_integral_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // NUMERIC FUNCTIONS
  // -----------------

  // Test avg
  {
    using TI = decltype(avg(t.id));
    using TF = decltype(avg(f.doubleN));
    static_assert(sqlpp::has_auto_alias_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TI>::value, "type requirement");
    static_assert(sqlpp::has_auto_alias_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_integral_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
  }

  // Test count
  {
    using TI = decltype(count(t.id));
    using TT = decltype(count(t.textN));
    using TF = decltype(count(f.doubleN));
    static_assert(sqlpp::has_auto_alias_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point_t<TI>::value, "type requirement");
    static_assert(sqlpp::has_auto_alias_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(sqlpp::has_auto_alias_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point_t<TT>::value, "type requirement");

    if (false and db(select(count(t.id)).from(t).unconditionally()).front().count > 0)
    { /* do something */
    }
  }

  // Test max
  {
    using TI = decltype(max(t.id));
    using TF = decltype(max(f.doubleN));
    using TT = decltype(max(t.textN));
    static_assert(sqlpp::has_auto_alias_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point_t<TI>::value, "type requirement");
    static_assert(sqlpp::has_auto_alias_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_integral_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(sqlpp::has_auto_alias_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test min
  {
    using TI = decltype(min(t.id));
    using TF = decltype(min(f.doubleN));
    using TT = decltype(min(t.textN));
    static_assert(sqlpp::has_auto_alias_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_integral_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test sum
  {
    using TI = decltype(sum(t.id));
    using TF = decltype(sum(f.doubleN));
    static_assert(sqlpp::has_auto_alias_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_integral_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
  }

  // MISC FUNCTIONS
  // --------------

  // test value
  {
    using TB = decltype(sqlpp::value(true));
    using TI = decltype(sqlpp::value(7));
    using TF = decltype(sqlpp::value(1.5));
    using TT = decltype(sqlpp::value("cheesecake"));
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TB>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // test flatten
  {
    using TB = decltype(flatten(t.boolNn, db));
    using TI = decltype(flatten(t.id, db));
    using TF = decltype(flatten(f.doubleN, db));
    using TT = decltype(flatten(t.textN, db));
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TB>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // test value_or_null
  {
    using TB = decltype(sqlpp::value_or_null(true));
    using TI = decltype(sqlpp::value_or_null(7));
    using TF = decltype(sqlpp::value_or_null(5.6));
    using TT = decltype(sqlpp::value_or_null("hallo"));
    using TD = decltype(sqlpp::value_or_null(std::chrono::system_clock::now()));
    using TBN = decltype(sqlpp::value_or_null<sqlpp::boolean>(sqlpp::null));
    using TIN = decltype(sqlpp::value_or_null<sqlpp::integral>(sqlpp::null));
    using TFN = decltype(sqlpp::value_or_null<sqlpp::floating_point>(sqlpp::null));
    using TTN = decltype(sqlpp::value_or_null<sqlpp::text>(sqlpp::null));
    using TDN = decltype(sqlpp::value_or_null<sqlpp::time_point>(sqlpp::null));
    static_assert(std::is_same<TB, TBN>::value, "type_requirement");
    static_assert(std::is_same<TI, TIN>::value, "type_requirement");
    static_assert(std::is_same<TF, TFN>::value, "type_requirement");
    static_assert(std::is_same<TT, TTN>::value, "type_requirement");
    static_assert(std::is_same<TD, TDN>::value, "type_requirement");
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TB>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // test verbatim
  {
    using TB = decltype(sqlpp::verbatim<sqlpp::boolean>("1"));
    using TBS = decltype(sqlpp::verbatim<sqlpp::boolean>("1").as(kaesekuchen));
    using TI = decltype(sqlpp::verbatim<sqlpp::bigint>("42"));
    using TF = decltype(sqlpp::verbatim<sqlpp::floating_point>("1.5"));
    using TT = decltype(sqlpp::verbatim<sqlpp::text>("cheesecake"));
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TBS>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TBS>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // test verbatim_table
  {
    using T = decltype(sqlpp::verbatim_table("cheesecake"));
    static_assert(not sqlpp::is_selectable_t<T>::value, "type requirement");
    static_assert(not sqlpp::is_expression_t<T>::value, "type requirement");
    static_assert(sqlpp::is_table_t<T>::value, "type requirement");
  }

  // test verbatim_table alias
  {
    using T = decltype(sqlpp::verbatim_table("cheesecake").as(kaesekuchen));
    static_assert(not sqlpp::is_selectable_t<T>::value, "type requirement");
    static_assert(not sqlpp::is_expression_t<T>::value, "type requirement");
    static_assert(sqlpp::is_table_t<T>::value, "type requirement");
    static_assert(sqlpp::is_alias_t<T>::value, "type requirement");
  }

  return 0;
}

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
#include <sqlpp11/select.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/connection.h>

#include <iostream>

SQLPP_ALIAS_PROVIDER(kaesekuchen)

int Function(int, char* [])
{
  MockDb db = {};
  const auto f = test::TabFoo{};
  const auto t = test::TabBar{};

  // f.omega + 4 *= "";

  // MEMBER FUNCTIONS
  // ----------------

  // Test in
  {
    using TI = decltype(t.alpha.in(1, 2, 3));
    using TF = decltype(f.omega.in(1.0, 2.0, 3.0));
    using TT = decltype(t.beta.in("a", "b", "c"));
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
    using TI = decltype(t.alpha.in(sqlpp::value_list(std::vector<int>({1, 2, 3}))));
    using TF = decltype(f.omega.in(sqlpp::value_list(std::vector<float>({1.0, 2.0, 3.0}))));
    using TT = decltype(t.beta.in(sqlpp::value_list(std::vector<std::string>({"a", "b", "c"}))));
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
    using TI = decltype(t.alpha.not_in(1, 2, 3));
    using TF = decltype(f.omega.not_in(1.0, 2.0, 3.0));
    using TT = decltype(t.beta.not_in("a", "b", "c"));
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
    using TI = decltype(t.alpha.not_in(sqlpp::value_list(std::vector<int>({1, 2, 3}))));
    using TF = decltype(f.omega.not_in(sqlpp::value_list(std::vector<float>({1.0, 2.0, 3.0}))));
    using TT = decltype(t.beta.not_in(sqlpp::value_list(std::vector<std::string>({"a", "b", "c"}))));
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
    using TT = decltype(t.beta.like("%c%"));
    static_assert(sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // Test is_null
  {
    using TI = decltype(t.alpha.is_null());
    using TF = decltype(f.omega.is_null());
    using TT = decltype(t.beta.is_null());
    using TTI = decltype(is_null(t.alpha));
    using TTF = decltype(is_null(f.omega));
    using TTT = decltype(is_null(t.beta));
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
    using TI = decltype(t.alpha.is_not_null());
    using TF = decltype(f.omega.is_not_null());
    using TT = decltype(t.beta.is_not_null());
    using TTI = decltype(is_not_null(t.alpha));
    using TTF = decltype(is_not_null(f.omega));
    using TTT = decltype(is_not_null(t.beta));
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
    using TI = decltype(exists(select(t.alpha).from(t)));
    using TT = decltype(exists(select(t.beta).from(t)));
    static_assert(sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_numeric_t<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text_t<TT>::value, "type requirement");

    if (false and db(select(exists(select(t.alpha).from(t).unconditionally()))).front().exists)
    { /* do something */
    }
  }

  // Test any
  {
    using S = decltype(select(t.alpha).from(t));
    static_assert(sqlpp::is_numeric_t<S>::value, "type requirement");

    using TI = decltype(any(select(t.alpha).from(t)));
    using TT = decltype(any(select(t.beta).from(t)));
    using TF = decltype(any(select(f.omega).from(f)));
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
    using TI = decltype(some(select(t.alpha).from(t)));
    using TT = decltype(some(select(t.beta).from(t)));
    using TF = decltype(some(select(f.omega).from(f)));
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
    using TI = decltype(avg(t.alpha));
    using TF = decltype(avg(f.omega));
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
    using TI = decltype(count(t.alpha));
    using TT = decltype(count(t.beta));
    using TF = decltype(count(f.omega));
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

    if (false and db(select(count(t.alpha)).from(t).unconditionally()).front().count > 0)
    { /* do something */
    }
  }

  // Test max
  {
    using TI = decltype(max(t.alpha));
    using TF = decltype(max(f.omega));
    using TT = decltype(max(t.beta));
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
    using TI = decltype(min(t.alpha));
    using TF = decltype(min(f.omega));
    using TT = decltype(min(t.beta));
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
    using TI = decltype(sum(t.alpha));
    using TF = decltype(sum(f.omega));
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
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // test flatten
  {
    using TB = decltype(flatten(t.gamma, db));
    using TI = decltype(flatten(t.alpha, db));
    using TF = decltype(flatten(f.omega, db));
    using TT = decltype(flatten(t.beta, db));
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TB>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
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
    using TBN = decltype(sqlpp::value_or_null<sqlpp::boolean>(sqlpp::null));
    using TIN = decltype(sqlpp::value_or_null<sqlpp::integral>(sqlpp::null));
    using TFN = decltype(sqlpp::value_or_null<sqlpp::floating_point>(sqlpp::null));
    using TTN = decltype(sqlpp::value_or_null<sqlpp::text>(sqlpp::null));
    static_assert(std::is_same<TB, TBN>::value, "type_requirement");
    static_assert(std::is_same<TI, TIN>::value, "type_requirement");
    static_assert(std::is_same<TF, TFN>::value, "type_requirement");
    static_assert(std::is_same<TT, TTN>::value, "type_requirement");
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TB>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
    static_assert(sqlpp::is_floating_point_t<TF>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TT>::value, "type requirement");
    static_assert(sqlpp::is_text_t<TT>::value, "type requirement");
  }

  // test verbatim
  {
    using TB = decltype(sqlpp::verbatim<sqlpp::boolean>("1"));
    using TI = decltype(sqlpp::verbatim<sqlpp::bigint>("42"));
    using TF = decltype(sqlpp::verbatim<sqlpp::floating_point>("1.5"));
    using TT = decltype(sqlpp::verbatim<sqlpp::text>("cheesecake"));
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_boolean_t<TB>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TB>::value, "type requirement");
    static_assert(sqlpp::is_integral_t<TI>::value, "type requirement");
    static_assert(not sqlpp::is_selectable_t<TI>::value, "type requirement");
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

  // test tvin
  {
    static_assert(std::is_same<decltype(sqlpp::tvin(1)), sqlpp::tvin_arg_t<sqlpp::integral_operand>>::value,
                  "integral values are accepted and wrapped");
    static_assert(std::is_same<decltype(sqlpp::tvin(false)), sqlpp::tvin_arg_t<sqlpp::boolean_operand>>::value,
                  "bool values are accepted and wrapped");
    static_assert(std::is_same<decltype(sqlpp::tvin(0.17)), sqlpp::tvin_arg_t<sqlpp::floating_point_operand>>::value,
                  "float values are accepted and wrapped");
    static_assert(std::is_same<decltype(sqlpp::tvin("test")), sqlpp::tvin_arg_t<sqlpp::text_operand>>::value,
                  "text values are accepted and wrapped");

    for (const auto& row : db(select(all_of(t)).from(t).unconditionally()))
    {
      static_assert(std::is_same<decltype(sqlpp::tvin(row.alpha)),
                                 sqlpp::tvin_arg_t<typename std::remove_const<decltype(row.alpha)>::type>>::value,
                    "result fields are accepted and not wrapped");
      static_assert(std::is_same<decltype(sqlpp::tvin(row.beta)),
                                 sqlpp::tvin_arg_t<typename std::remove_const<decltype(row.beta)>::type>>::value,
                    "result fields are accepted and not wrapped");
      static_assert(std::is_same<decltype(sqlpp::tvin(row.gamma)),
                                 sqlpp::tvin_arg_t<typename std::remove_const<decltype(row.gamma)>::type>>::value,
                    "result fields are accepted and not wrapped");
    }
  }

  return 0;
}

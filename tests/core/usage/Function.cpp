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

#include <sqlpp11/tests/core/MockDb.h>
#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/tests/core/result_helpers.h>
#include <sqlpp11/sqlpp11.h>

#include <iostream>

SQLPP_CREATE_NAME_TAG(kaesekuchen);
SQLPP_CREATE_NAME_TAG(something);

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
    static_assert(sqlpp::is_boolean<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TT>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text<TT>::value, "type requirement");
  }

  // Test in with value list
  {
    using TI = decltype(t.id.in(std::vector<int>({1, 2, 3})));
    using TF = decltype(f.doubleN.in(std::vector<float>({1.0, 2.0, 3.0})));
    using TT = decltype(t.textN.in(std::vector<std::string>({"a", "b", "c"})));
    static_assert(sqlpp::is_boolean<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TT>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text<TT>::value, "type requirement");
  }

  // Test not_in
  {
    using TI = decltype(t.id.not_in(1, 2, 3));
    using TF = decltype(f.doubleN.not_in(1.0, 2.0, 3.0));
    using TT = decltype(t.textN.not_in("a", "b", "c"));
    static_assert(sqlpp::is_boolean<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TT>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text<TT>::value, "type requirement");
  }

  // Test not in with value list
  {
    using TI = decltype(t.id.not_in(std::vector<int>({1, 2, 3})));
    using TF = decltype(f.doubleN.not_in(std::vector<float>({1.0, 2.0, 3.0})));
    using TT = decltype(t.textN.not_in(std::vector<std::string>({"a", "b", "c"})));
    static_assert(sqlpp::is_boolean<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TT>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text<TT>::value, "type requirement");
  }

  // Test like
  {
    using TT = decltype(t.textN.like("%c%"));
    static_assert(sqlpp::is_boolean<TT>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text<TT>::value, "type requirement");
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
    static_assert(sqlpp::is_boolean<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TT>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text<TT>::value, "type requirement");
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
    static_assert(sqlpp::is_boolean<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text<TF>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TT>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text<TT>::value, "type requirement");
  }

  // SUB_SELECT_FUNCTIONS
  // --------------------

  // Test exists
  {
    using TI = decltype(exists(select(t.id).from(t).unconditionally()));
    using TT = decltype(exists(select(t.textN).from(t).unconditionally()));
    static_assert(sqlpp::is_boolean<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text<TI>::value, "type requirement");
    static_assert(sqlpp::is_boolean<TT>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text<TT>::value, "type requirement");

    if (false and db(select(exists(select(t.id).from(t).unconditionally()).as(something))).front().something)
    { /* do something */
    }
  }

  // Test any
  {
    using S = decltype(select(t.id).from(t).unconditionally());
    static_assert(sqlpp::is_numeric<S>::value, "type requirement");

    using TI = decltype(any(select(t.id).from(t).unconditionally()));
    using TT = decltype(any(select(t.textN).from(t).unconditionally()));
    using TF = decltype(any(select(f.doubleN).from(f).unconditionally()));
    static_assert(not sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(not sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(not sqlpp::is_text<TI>::value, "type requirement");
    static_assert(not sqlpp::is_numeric<TF>::value, "tFpe requirement");
    static_assert(not sqlpp::is_floating_point<TF>::value, "type requirement");
    static_assert(not sqlpp::is_text<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(not sqlpp::is_integral<TT>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<TT>::value, "type requirement");
    static_assert(not sqlpp::is_text<TT>::value, "type requirement");
  }

  // NUMERIC FUNCTIONS
  // -----------------

  // Test avg
  {
    using TI = decltype(avg(t.id));
    using TF = decltype(avg(f.doubleN));
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(not sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_integral<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<TF>::value, "type requirement");
  }

  // Test count
  {
    using TI = decltype(count(t.id));
    using TT = decltype(count(t.textN));
    using TF = decltype(count(f.doubleN));
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(sqlpp::is_integral<TF>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<TF>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(sqlpp::is_integral<TT>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<TT>::value, "type requirement");

    if (false and db(select(count(t.id).as(something)).from(t).unconditionally()).front().something > 0)
    { /* do something */
    }
  }

  // Test max
  {
    using TI = decltype(max(t.id));
    using TF = decltype(max(f.doubleN));
    using TT = decltype(max(t.textN));
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_integral<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(sqlpp::is_text<TT>::value, "type requirement");
  }

  // Test min
  {
    using TI = decltype(min(t.id));
    using TF = decltype(min(f.doubleN));
    using TT = decltype(min(t.textN));
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_integral<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<TF>::value, "type requirement");
    static_assert(not sqlpp::is_numeric<TT>::value, "type requirement");
    static_assert(sqlpp::is_text<TT>::value, "type requirement");
  }

  // Test sum
  {
    using TI = decltype(sum(t.id));
    using TF = decltype(sum(f.doubleN));
    static_assert(sqlpp::is_numeric<TI>::value, "type requirement");
    static_assert(sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<TI>::value, "type requirement");
    static_assert(sqlpp::is_numeric<TF>::value, "type requirement");
    static_assert(not sqlpp::is_integral<TF>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<TF>::value, "type requirement");
  }

  // MISC FUNCTIONS
  // --------------

  // test value
  {
    using TB = decltype(sqlpp::value(true));
    using TI = decltype(sqlpp::value(7));
    using TF = decltype(sqlpp::value(1.5));
    using TT = decltype(sqlpp::value("cheesecake"));
    static_assert(sqlpp::is_boolean<TB>::value, "type requirement");
    static_assert(sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<TF>::value, "type requirement");
    static_assert(sqlpp::is_text<TT>::value, "type requirement");
  }

  // test flatten
  {
    using TB = decltype(flatten(t.boolNn, db));
    using TI = decltype(flatten(t.id, db));
    using TF = decltype(flatten(f.doubleN, db));
    using TT = decltype(flatten(t.textN, db));
    static_assert(sqlpp::is_boolean<TB>::value, "type requirement");
    static_assert(sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<TF>::value, "type requirement");
    static_assert(sqlpp::is_text<TT>::value, "type requirement");
  }

  // test optional
  {
    using TB = decltype(sqlpp::optional<bool>(true));
    using TI = decltype(sqlpp::optional<int>(7));
    using TF = decltype(sqlpp::optional<float>(5.6));
    using TT = decltype(sqlpp::optional<std::string>("hallo"));
    using TD = decltype(sqlpp::optional<std::chrono::time_point<std::chrono::system_clock>>(std::chrono::system_clock::now()));
    using TBN = decltype(sqlpp::optional<bool>(sqlpp::nullopt));
    using TIN = decltype(sqlpp::optional<int>(sqlpp::nullopt));
    using TFN = decltype(sqlpp::optional<float>(sqlpp::nullopt));
    using TTN = decltype(sqlpp::optional<std::string>(sqlpp::nullopt));
    using TDN = decltype(sqlpp::optional<std::chrono::time_point<std::chrono::system_clock>>(sqlpp::nullopt));
    static_assert(std::is_same<TB, TBN>::value, "type_requirement");
    static_assert(std::is_same<TI, TIN>::value, "type_requirement");
    static_assert(std::is_same<TF, TFN>::value, "type_requirement");
    static_assert(std::is_same<TT, TTN>::value, "type_requirement");
    static_assert(std::is_same<TD, TDN>::value, "type_requirement");
    static_assert(sqlpp::is_boolean<TB>::value, "type requirement");
    static_assert(sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<TF>::value, "type requirement");
    static_assert(sqlpp::is_text<TT>::value, "type requirement");
  }

  // test verbatim
  {
    using TB = decltype(sqlpp::verbatim<sqlpp::boolean>("1"));
    using TBS = decltype(sqlpp::verbatim<sqlpp::boolean>("1").as(kaesekuchen));
    using TI = decltype(sqlpp::verbatim<sqlpp::integral>("42"));
    using TF = decltype(sqlpp::verbatim<sqlpp::floating_point>("1.5"));
    using TT = decltype(sqlpp::verbatim<sqlpp::text>("cheesecake"));
    static_assert(sqlpp::is_boolean<TB>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<TBS>::value, "type requirement");
    static_assert(sqlpp::is_integral<TI>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<TF>::value, "type requirement");
    static_assert(sqlpp::is_text<TT>::value, "type requirement");
  }

  // test verbatim_table
  {
    using T = decltype(sqlpp::verbatim_table("cheesecake"));
    static_assert(sqlpp::is_table<T>::value, "type requirement");
  }

  // test verbatim_table alias
  {
    using T = decltype(sqlpp::verbatim_table("cheesecake").as(kaesekuchen));
    static_assert(sqlpp::is_table<T>::value, "type requirement");
  }

  return 0;
}

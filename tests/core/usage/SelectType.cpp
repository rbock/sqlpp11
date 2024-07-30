/*
 * Copyright (c) 2013-2016, Roland Bock, Aaron Bishop
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

#include "MockDb.h"
#include "Sample.h"
#include <iostream>
#include <sqlpp11/core/name/alias_provider.h>
#include <sqlpp11/core/database/connection.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/core/clause/select.h>
#include "../../include/test_helpers.h"

namespace alias
{
  SQLPP_ALIAS_PROVIDER(a)
  SQLPP_ALIAS_PROVIDER(b)
  SQLPP_ALIAS_PROVIDER(left)
  SQLPP_ALIAS_PROVIDER(right)
}  // namespace alias

int SelectType(int, char*[])
{
  MockDb db = {};
  MockDb::_serializer_context_t printer = {};

  auto f = test::TabFoo{};
  auto t = test::TabBar{};

  // Test a table
  {
    using T = typename std::decay<decltype(t)>::type;
    static_assert(not sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_integral<T>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(sqlpp::is_table<T>::value, "type requirement");
  }

  // Test an alias of table
  {
    using T = decltype(t.as(alias::a));
    static_assert(not sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_integral<T>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(sqlpp::is_table<T>::value, "type requirement");
  }

  // Test an integral column of an alias of table
  {
    using T = decltype(t.as(alias::a).id);
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(sqlpp::is_integral<T>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test an integral table column
  {
    using T = decltype(t.id);
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(sqlpp::is_integral<T>::value, "type requirement");
    static_assert(not sqlpp::is_unsigned_integral<T>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test an unsigned integral table column
  {
    using T = decltype(f.uIntN);
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_integral<T>::value, "type requirement");
    static_assert(sqlpp::is_unsigned_integral<T>::value, "type requirement");
    static_assert(not sqlpp::is_floating_point<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test a floating point table column
  {
    using T = decltype(f.doubleN);
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_integral<T>::value, "type requirement");
    static_assert(sqlpp::is_floating_point<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test a an alias of a numeric table column
  {
    using T = decltype(t.id.as(alias::a));
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test a select of a single column without a from
  {
    using T = decltype(select(t.id));
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test a select of a single numeric table column
  {
    using T = decltype(select(t.id).from(t));
    // static_assert(sqlpp::is_select_column_list_t<decltype(T::_column_list)>::value, "Must not be noop");
    // static_assert(sqlpp::is_from_t<decltype(T::_from)>::value, "Must not be noop");
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test a select of an alias of a single numeric table column
  {
    using T = decltype(select(t.id.as(alias::a)).from(t));
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test an alias of a select of a single numeric table column
  {
    using T = decltype(select(t.id).from(t).as(alias::b));
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "red to not be boolean");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(sqlpp::is_table<T>::value, "type requirement");
  }

  // Test the column of an alias of a select of an alias of a single numeric table column
  {
    using T = decltype(select(t.id.as(alias::a)).from(t).as(alias::b));
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(sqlpp::is_table<T>::value, "type requirement");
  }

  // Test the column of an alias of a select of a single numeric table column
  {
    using T = decltype(select(t.id).from(t).as(alias::b).id);
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test an alias of a select of an alias of a single numeric table column
  {
    using T = decltype(select(t.id.as(alias::a)).from(t).as(alias::b).a);
    static_assert(sqlpp::is_numeric<T>::value, "type requirement");
    static_assert(not sqlpp::is_boolean<T>::value, "type requirement");
    static_assert(not sqlpp::is_text<T>::value, "type requirement");
    static_assert(not sqlpp::is_table<T>::value, "type requirement");
  }

  // Test that all_of(tab) is expanded in select
  {
    auto a = select(all_of(t));
    auto b = select(t.id, t.textN, t.boolNn, t.intN);
    // auto c = select(t);
    static_assert(std::is_same<decltype(a), decltype(b)>::value, "all_of(t) has to be expanded by select()");
    // static_assert(std::is_same<decltype(b), decltype(c)>::value, "t has to be expanded by select()");
  }

  // Test that result sets with identical name/value combinations have identical types
#warning: reactivate after renaming concluded
#if 0
  {
    auto a = select(t.id);
    auto b = select(f.intN.as(t.id));
    using A = typename decltype(a)::_result_row_t<MockDb>;
    using B = typename decltype(b)::_result_row_t<MockDb>;
    static_assert(
        std::is_same<sqlpp::value_type_of<decltype(t.id)>, sqlpp::value_type_of<decltype(f.intN)>>::value,
        "Two bigint columns must have identical base_value_type");
    static_assert(std::is_same<A, B>::value,
                  "select with identical columns(name/value_type) need to have identical result_types");
  }
#endif

  for (const auto& row : db(select(all_of(t)).from(t).unconditionally()))
  {
    const auto a = row.id;
    std::cout << a << std::endl;
  }

#warning add tests for dynamic everything.
  /*
  {
    auto s = dynamic_select(db, all_of(t)).dynamic_from(t).dynamic_where().dynamic_limit().dynamic_offset();
    s.from.add(dynamic_join(f).on(f.doubleN > t.id));
    s.where.add(without_table_check(f.doubleN > 7 and t.id == any(select(t.id).from(t).where(t.id < 3))));
    s.limit.set(30u);
    s.limit.set(3u);
    std::cerr << "------------------------\n";
    serialize(printer, s).str();
    std::cerr << "------------------------\n";
    using T = decltype(s);
  }

  // Test that select can be called with zero columns if it is used with dynamic columns.
  {
    auto s = dynamic_select(db).dynamic_columns();
    s.selected_columns.add(without_table_check(t.id));
    serialize(printer, s).str();
  }

  {
    auto find_query = sqlpp::dynamic_select(db).dynamic_columns(t.id.as(alias::a)).dynamic_from(t).unconditionally();
    find_query.from.add(sqlpp::dynamic_join(f).on(t.id == f.doubleN));
    find_query.selected_columns.add(sqlpp::without_table_check(f.doubleN.as(alias::b)));
  }
  */

  // Test that verbatim_table compiles
  {
    auto s = select(t.id).from(sqlpp::verbatim_table("my_unknown_table"));
    serialize(printer, s).str();
  }

  static_assert(sqlpp::is_select_flag_t<decltype(sqlpp::all)>::value, "sqlpp::all has to be a select_flag");
  static_assert(sqlpp::is_numeric<decltype(t.id)>::value, "TabBar.id has to be a numeric");
  ((t.id + 7) + 4).asc();
  static_assert(sqlpp::is_boolean<decltype(t.boolNn != not(t.boolNn))>::value,
                "Comparison expression have to be boolean");
  !t.boolNn;
  serialize(printer, t.textN < "kaesekuchen").str();
  serialize(printer, t.textN + "hallenhalma").str();
  serialize(printer, t.id).str();
  std::cerr << "\n" << sizeof(test::TabBar) << std::endl;

  auto l = t.as(alias::left);
  auto r = select(t.boolNn.as(alias::a)).from(t).where(t.boolNn == true).as(alias::right);
  static_assert(sqlpp::is_boolean<decltype(select(t.boolNn).from(t))>::value, "select(bool) has to be a bool");
  static_assert(sqlpp::is_boolean<decltype(select(r.a).from(r))>::value, "select(bool) has to be a bool");
  auto s1 = sqlpp::select()
                .flags(sqlpp::distinct, sqlpp::straight_join)
                .columns(l.boolNn, r.a)
                .from(r.cross_join(t).cross_join(l))
                .where(t.textN == "hello world" and select(t.boolNn).from(t))  // .as(alias::right))
                .group_by(l.boolNn, r.a)
                .having(r.a != true)
                .order_by(l.textN.asc())
                .limit(17u)
                .offset(3u)
                .as(alias::a);

  return 0;
}

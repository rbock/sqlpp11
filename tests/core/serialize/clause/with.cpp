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

#include "Sample.h"
#include "../compare.h"
#include <sqlpp11/sqlpp11.h>

int main(int, char* [])
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // No expression (not super useful).
  SQLPP_COMPARE(cte(sqlpp::alias::x), "x");

  // WITH Simple CTE: X AS SELECT
  {
    const auto x = cte(sqlpp::alias::x).as(select(foo.id).from(foo).unconditionally());
    SQLPP_COMPARE(with(x), "WITH x AS (SELECT tab_foo.id FROM tab_foo) ");
  }

  // WITH Non-recursive union CTE: X AS SELECT ... UNION ALL SELECT ...
  {
    const auto x =
        cte(sqlpp::alias::x)
            .as(select(foo.id).from(foo).unconditionally().union_all(select(bar.id).from(bar).unconditionally()));
    SQLPP_COMPARE(with(x), "WITH x AS (SELECT tab_foo.id FROM tab_foo UNION ALL SELECT tab_bar.id FROM tab_bar) ");
  }

  // WITH Recursive union CTE: X AS SELECT ... UNION ALL SELECT ... FROM X ...
  {
    const auto x_base = cte(sqlpp::alias::x).as(select(sqlpp::value(0).as(sqlpp::alias::a)));
    const auto x = x_base.union_all(select((x_base.a + 1).as(sqlpp::alias::a)).from(x_base).where(x_base.a < 10));
#warning: Remove debug code.
    /*
    using Lhs = sqlpp::statement_t<sqlpp::no_with_t, sqlpp::select_t, sqlpp::no_select_flag_list_t,
                           sqlpp::select_column_list_t<sqlpp::expression_as<sqlpp::value_t<int>, sqlpp::alias::a_t>>,
                           sqlpp::no_from_t, sqlpp::no_where_t<true>, sqlpp::no_group_by_t, sqlpp::no_having_t,
                           sqlpp::no_order_by_t, sqlpp::no_limit_t, sqlpp::no_offset_t, sqlpp::no_union_t,
                           sqlpp::no_for_update_t>;
    using Rhs = sqlpp::statement_t<
        sqlpp::no_with_t, sqlpp::select_t, sqlpp::no_select_flag_list_t,
        sqlpp::select_column_list_t<sqlpp::expression_as<
            sqlpp::arithmetic_expression<
                sqlpp::column_t<sqlpp::cte_ref_t<sqlpp::alias::x_t>,
                                sqlpp::field_spec_t<sqlpp::alias::a_t::_sqlpp_name_tag, sqlpp::integral>>,
                sqlpp::plus, int>,
            sqlpp::alias::a_t>>,
        sqlpp::from_t<sqlpp::cte_ref_t<sqlpp::alias::x_t>>,
        sqlpp::where_t<sqlpp::comparison_expression<
            sqlpp::column_t<sqlpp::cte_ref_t<sqlpp::alias::x_t>,
                            sqlpp::field_spec_t<sqlpp::alias::a_t::_sqlpp_name_tag, sqlpp::integral>>,
            sqlpp::less, int>>,
        sqlpp::no_group_by_t, sqlpp::no_having_t, sqlpp::no_order_by_t, sqlpp::no_limit_t, sqlpp::no_offset_t,
        sqlpp::no_union_t, sqlpp::no_for_update_t>;
    //sqlpp::required_ctes_of_t<Rhs>::hansi;
    using U = sqlpp::cte_union_t<
        sqlpp::all_t, Lhs, Rhs>;

    using X = typename std::decay<decltype(x)>::type;
    //X::hansi;
    static_assert(X::_is_recursive, "");
    */
#warning: Need to test that recursive CTEs are detected as being recursive.
    SQLPP_COMPARE(with(x), "WITH RECURSIVE x AS (SELECT 0 AS a UNION ALL SELECT (x.a + 1) AS a FROM x WHERE x.a < 10) ");
  }

  return 0;
}

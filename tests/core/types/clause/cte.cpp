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
#include <sqlpp11/sqlpp11.h>

void test_cte()
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

#warning: add more tests, including parameters and dynamic parts
#warning: Can we union dynamically?

  // Simple CTE: X AS SELECT
  {
    auto x = cte(sqlpp::alias::x).as(select(foo.id).from(foo).unconditionally());
    auto a = x.as(sqlpp::alias::a);

    using X = decltype(x);
    using RX = decltype(make_table_ref(x));
    using A = decltype(a);
    using RA = decltype(make_table_ref(a));

    // CTE is used in WITH and in FROM
    static_assert(sqlpp::is_cte<X>::value, "");
    static_assert(not sqlpp::is_recursive_cte<X>::value, "");
    static_assert(sqlpp::is_table<X>::value, "");
    static_assert(sqlpp::required_ctes_of_t<X>::is_empty(), "");
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<X>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(sqlpp::parameters_of_t<X>::empty(), "");

    // CTE reference is what is stored in from_t or join_t.
    // While it refers to a CTE, it cannot be used as a CTE or table, i.e. with(rx) or from(ra) would not compile.
    static_assert(not sqlpp::is_cte<RX>::value, "");
    static_assert(not sqlpp::is_table<RX>::value, "");
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<RX>, sqlpp::detail::type_set<>>::value, "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<RX>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(sqlpp::parameters_of_t<RX>::empty(), "");

    // CTEs can be aliased (e.g. x AS a). This alias can be used as a table in FROM, but not as a CTE in WITH.
    static_assert(not sqlpp::is_cte<A>::value, "");
    static_assert(sqlpp::is_table<A>::value, "");
    static_assert(std::is_same<A, RA>::value, "");
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<RA>, sqlpp::detail::type_set<>>::value, "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<RA>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(sqlpp::parameters_of_t<RA>::empty(), "");
  }

  // Simple CTE with parameter
  {
    auto p = sqlpp::parameter(foo.id);
    auto x = cte(sqlpp::alias::x).as(select(foo.id).from(foo).where(foo.id > p));
    auto a = x.as(sqlpp::alias::a);

    using X = decltype(x);
    using RX = decltype(make_table_ref(x));
    using RA = decltype(make_table_ref(a));
    using P = decltype(p);

    // CTE is used in WITH and in FROM
    static_assert(sqlpp::is_cte<X>::value, "");
    static_assert(not sqlpp::is_recursive_cte<X>::value, "");
    static_assert(sqlpp::is_table<X>::value, "");
    static_assert(sqlpp::required_ctes_of_t<X>::is_empty(), "");
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<X>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(std::is_same<sqlpp::parameters_of_t<X>, sqlpp::detail::type_vector<P>>::value, "");

    // Neither CTE reference nor alias carry the parameter.
    static_assert(sqlpp::parameters_of_t<RX>::empty(), "");
    static_assert(sqlpp::parameters_of_t<RA>::empty(), "");
  }

  // Non-recursive union CTE: X AS SELECT ... UNION ALL SELECT ...
  {
    auto x =
        cte(sqlpp::alias::x)
            .as(select(foo.id).from(foo).unconditionally().union_all(select(bar.id).from(bar).unconditionally()));

    using X = decltype(x);
    using RX = decltype(make_table_ref(x));

    // CTE is used in WITH and in FROM
    static_assert(sqlpp::is_cte<X>::value, "");
    static_assert(not sqlpp::is_recursive_cte<X>::value, "");
    static_assert(sqlpp::is_table<X>::value, "");
    static_assert(sqlpp::required_ctes_of_t<X>::is_empty(), "");
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<X>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(sqlpp::parameters_of_t<X>::empty(), "");
  }

  // Recursive union CTE: X AS SELECT ... UNION ALL SELECT ... FROM X ...
  {
    auto p = sqlpp::parameter(foo.id);
    auto x_base = cte(sqlpp::alias::x).as(select(sqlpp::value(0).as(sqlpp::alias::a)));
    auto x = x_base.union_all(select((x_base.a + 1).as(sqlpp::alias::a)).from(x_base).where(x_base.a < p));

    using X = decltype(x);
    using RX = decltype(make_table_ref(x));
    using P = decltype(p);

    // CTE is used in WITH and in FROM
    static_assert(sqlpp::is_cte<X>::value, "");
    static_assert(sqlpp::is_recursive_cte<X>::value, "");
    static_assert(sqlpp::is_table<X>::value, "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<X>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<X>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(std::is_same<sqlpp::parameters_of_t<X>, sqlpp::detail::type_vector<P>>::value, "");
   }

  // A CTE depending on another CTE
  {
    auto pb = sqlpp::parameter(foo.intN);
    auto p = sqlpp::parameter(foo.id);
    auto b = cte(sqlpp::alias::b).as(select(foo.id).from(foo).where(foo.id != pb));
    auto x = cte(sqlpp::alias::y).as(select(b.id, sqlpp::value(7).as(sqlpp::alias::a)).from(b).where(b.id > p));
    auto a = x.as(sqlpp::alias::a);

    using RB = decltype(make_table_ref(b));
    using X = decltype(x);
    using RX = decltype(make_table_ref(x));
    using RA = decltype(make_table_ref(a));
    using P = decltype(p);

    // CTE is used in WITH and in FROM
    static_assert(sqlpp::is_cte<X>::value, "");
    static_assert(not sqlpp::is_recursive_cte<X>::value, "");
    static_assert(sqlpp::is_table<X>::value, "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<X>, sqlpp::detail::type_set<RB>>::value, "");
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<X>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(std::is_same<sqlpp::parameters_of_t<X>, sqlpp::detail::type_vector<P>>::value, "");

    // Neither CTE reference nor alias carry the dependency.
    static_assert(std::is_same<sqlpp::required_ctes_of_t<RA>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<RX>, sqlpp::detail::type_set<RX>>::value, "");
   }

  // A recursive CTE depending on another CTE
  {
    auto pb = sqlpp::parameter(foo.intN);
    auto p = sqlpp::parameter(foo.id);
    auto b = cte(sqlpp::alias::b).as(select(foo.id.as(sqlpp::alias::a)).from(foo).where(foo.id != pb));
    auto x_base = cte(sqlpp::alias::x).as(select(b.a).from(b).unconditionally());
    auto x = x_base.union_all(select((x_base.a + 1).as(sqlpp::alias::a)).from(x_base).where(x_base.a < p));
    auto a = x.as(sqlpp::alias::a);

    using RB = decltype(make_table_ref(b));
    using X = decltype(x);
    using RX = decltype(make_table_ref(x));
    using RA = decltype(make_table_ref(a));
    using P = decltype(p);

    // CTE is used in WITH and in FROM
    static_assert(sqlpp::is_cte<X>::value, "");
    static_assert(sqlpp::is_recursive_cte<X>::value, "");
    static_assert(sqlpp::is_table<X>::value, "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<X>, sqlpp::detail::type_set<RB, RX>>::value, "");
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<X>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(std::is_same<sqlpp::parameters_of_t<X>, sqlpp::detail::type_vector<P>>::value, "");

    // Neither CTE reference nor alias carry the dependency.
    static_assert(std::is_same<sqlpp::required_ctes_of_t<RA>, sqlpp::detail::type_set<RX>>::value, "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<RX>, sqlpp::detail::type_set<RX>>::value, "");
   }

}

int main()
{
  void test_cte();
}


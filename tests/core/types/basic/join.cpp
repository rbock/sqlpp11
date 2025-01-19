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

#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/sqlpp11.h>

namespace test
{
  SQLPP_CREATE_NAME_TAG(cheese);
  SQLPP_CREATE_NAME_TAG(cake);
  SQLPP_CREATE_NAME_TAG(meme);
  SQLPP_CREATE_NAME_TAG(verb);
  SQLPP_CREATE_NAME_TAG(CTE);
  SQLPP_CREATE_NAME_TAG(sel_as);
}  // namespace test

void test_join()
{
  auto v = sqlpp::value(17);
  auto foo = test::TabFoo{};
  auto bar = test::TabBar{};
  auto cheese = foo.as(test::cheese);
  auto cake = foo.as(test::cake);
  auto meme = schema_qualified_table({"meme"}, foo).as(test::meme);
  auto verb = sqlpp::verbatim_table("verb").as(test::verb);
  auto cte = sqlpp::cte(test::CTE).as(select(foo.id).from(foo).where(true));
  auto sel_as = select(all_of(foo)).from(foo).where(foo.id == sqlpp::parameter(foo.id)).as(test::sel_as);

  using Foo = decltype(foo);
  using Bar = decltype(bar);
  using Cheese = decltype(cheese);
  using Cake = decltype(cake);
  using Meme = decltype(meme);
  using Verb = decltype(verb);
  using CteRef = sqlpp::cte_ref_t<test::CTE_t>;
  using SelAsRef = sqlpp::select_ref_t<test::sel_as_t::_sqlpp_name_tag>;

  // Pre-join
  static_assert(not sqlpp::is_table<decltype(foo.join(bar))>::value, "");

  // Join of tables
  {
    using J = decltype(foo.join(bar).on(foo.id == bar.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Bar>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  {
    using J = decltype(foo.cross_join(bar));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Bar>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  {
    using J = decltype(foo.inner_join(bar).on(foo.id == bar.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Bar>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  {
    using J = decltype(foo.left_outer_join(bar).on(foo.id == bar.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Bar>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<Bar>>::value, "");
  }

  {
    using J = decltype(foo.right_outer_join(bar).on(foo.id == bar.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Bar>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<Foo>>::value, "");
  }

  {
    using J = decltype(foo.full_outer_join(bar).on(foo.id == bar.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Bar>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<Foo, Bar>>::value,
                  "");
  }

  // Join with rhs alias table
  {
    using J = decltype(foo.join(cheese).on(foo.id == cheese.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Cheese>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  // Join with rhs alias table
  {
    using J = decltype(foo.join(dynamic(true, cheese)).on(foo.id == cheese.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Cheese>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::detail::type_set<Foo>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  // Join with two alias tables
  {
    using J = decltype(cheese.join(cake).on(cheese.id == cake.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Cheese, Cake>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  // Join with schema-qualified table
  {
    using J = decltype(meme.join(cake).on(meme.id == cake.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Meme, Cake>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  // Join with verbatim table
  {
    using J = decltype(verb.join(meme).on(sqlpp::verbatim<sqlpp::integral>("verb.id") == meme.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Verb, Meme>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  // Join with select as
  {
    using J = decltype(sel_as.join(foo).on(sel_as.id == foo.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<SelAsRef, Foo>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  // Join with select as and parameters
  {
    using J = decltype(sel_as.join(foo).on(sel_as.id == foo.id + sqlpp::parameter(sqlpp::integral{}, sqlpp::alias::a)));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<SelAsRef, Foo>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");

    // parameters from sub select and condition are being exposed
    using ExpectedParameters =
        sqlpp::detail::type_vector<sqlpp::parameter_t<sqlpp::integral, test::TabFoo_::Id::_sqlpp_name_tag>,
                                   sqlpp::parameter_t<sqlpp::integral, sqlpp::alias::a_t::_sqlpp_name_tag>>;
    static_assert(std::is_same<sqlpp::parameters_of_t<J>, ExpectedParameters>::value, "");
  }

  // Join with cte
  {
    using J = decltype(cte.join(meme).on(cte.id == meme.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<CteRef, Meme>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::provided_tables_of_t<J>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<J>, sqlpp::detail::type_set<CteRef>>::value, "");
  }

  // Join with dynamic table
  {
    using J = decltype(foo.join(dynamic(true, bar)).on(foo.id == bar.id));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Bar>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::detail::type_set<Foo>>::value, "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  // Join with dynamic table and static table
  {
    using J = decltype(foo.cross_join(dynamic(true, bar))
                           .join(cheese)
                           .on(foo.id == cheese.id and dynamic(true, bar.id == cheese.id)));
    static_assert(sqlpp::is_table<J>::value, "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<J>, sqlpp::detail::type_set<Foo, Bar, Cheese>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<J>, sqlpp::detail::type_set<Foo, Cheese>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<J>, sqlpp::detail::type_set<>>::value, "");
  }

  // Examples that need to fail.
  {
    // foo.join(dynamic(true, bar))
    //    .on(foo.id == cheese.id)
    //
    // `cheese` must not be used in the ON condition as it is not part of the join at all.
    static_assert(
        not sqlpp::deep_check_join_on_condition<Foo, sqlpp::dynamic_t<Bar>, decltype(foo.id == cheese.id)>::value, "");

    // foo.cross_join(dynamic(true, bar))
    //    .join(cheese)
    //    .on(bar.id == cheese.id))`
    //
    // `bar` is dynamically joined only. It must not be used statically when joining cheese `statically`.
    using FooBar = decltype(foo.cross_join(dynamic(true, bar)));
    static_assert(
        not sqlpp::deep_check_join_on_condition<FooBar, Cheese, decltype(bar.id == cheese.id)>::value, "");
  }
}

int main()
{
  void test_join();
}


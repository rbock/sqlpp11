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
#include <sqlpp11/tests/core/tables.h>

SQLPP_CREATE_NAME_TAG(cheese);

void test_column() {
  {
    // Column integer with default (auto-increment).
    auto foo = test::TabFoo{};
    using Foo = decltype(foo);
    using Id = decltype(foo.id);
    using Cheese = decltype(foo.id.as(cheese));
    using BarId = decltype(foo.as(test::TabBar{}).id);
    using BarCheese = decltype(foo.as(test::TabBar{}).id.as(cheese));

    static_assert(not sqlpp::is_table<Id>::value, "");
    static_assert(sqlpp::has_default<Id>::value, "");
    static_assert(sqlpp::is_group_by_column<Id>::value, "");

    // Columns are aggregates when in group by, otherwise they are
    // non-aggregates. But they are never neutral.
    static_assert(not sqlpp::is_aggregate_neutral<Id>::value, "");

    static_assert(std::is_same<sqlpp::name_tag_of_t<Id>,
                               test::TabFoo_::Id::_sqlpp_name_tag>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<Id>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<Id>,
                               sqlpp::provided_tables_of_t<Id>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<Id>,
                               sqlpp::provided_tables_of_t<Id>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_tables_of_t<Id>,
                               sqlpp::detail::type_set<Foo>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_static_tables_of_t<Id>,
                               sqlpp::required_tables_of_t<Id>>::value,
                  "");

    static_assert(
        std::is_same<sqlpp::value_type_of_t<Id>, sqlpp::integral>::value, "");

    // tab_foo.id AS cheese
    // This is only useful SELECT. It therefore exposes no value directly.
    // It does require its table, though.
    static_assert(not sqlpp::is_table<Cheese>::value, "");
    static_assert(not sqlpp::has_default<Cheese>::value, "");
    static_assert(not sqlpp::is_group_by_column<Cheese>::value, "");

    static_assert(std::is_same<sqlpp::name_tag_of_t<Cheese>,
                               cheese_t::_sqlpp_name_tag>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<Cheese>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<Cheese>,
                               sqlpp::provided_tables_of_t<Cheese>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<Cheese>,
                               sqlpp::provided_tables_of_t<Cheese>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_tables_of_t<Cheese>,
                               sqlpp::detail::type_set<Foo>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_static_tables_of_t<Cheese>,
                               sqlpp::required_tables_of_t<Cheese>>::value,
                  "");

    static_assert(
        std::is_same<sqlpp::value_type_of_t<Cheese>, sqlpp::no_value_t>::value,
        "");

    // (tab_foo AS bar).id
    static_assert(not sqlpp::is_table<BarId>::value, "");
    static_assert(sqlpp::has_default<BarId>::value, "");
    static_assert(sqlpp::is_group_by_column<BarId>::value, "");

    static_assert(std::is_same<sqlpp::name_tag_of_t<BarId>,
                               test::TabFoo_::Id::_sqlpp_name_tag>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<BarId>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<BarId>,
                               sqlpp::provided_tables_of_t<BarId>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<BarId>,
                               sqlpp::provided_tables_of_t<BarId>>::value,
                  "");
    static_assert(
        std::is_same<
            sqlpp::required_tables_of_t<BarId>,
            sqlpp::detail::type_set<sqlpp::table_as_t<
                test::TabFoo_, test::TabBar_::_sqlpp_name_tag>>>::value,
        "");
    static_assert(std::is_same<sqlpp::required_static_tables_of_t<BarId>,
                               sqlpp::required_tables_of_t<BarId>>::value,
                  "");

    static_assert(
        std::is_same<sqlpp::value_type_of_t<BarId>, sqlpp::integral>::value,
        "");

    // (tab_foo as bar).id.as(cheese)
    static_assert(not sqlpp::is_table<BarCheese>::value, "");
    static_assert(not sqlpp::has_default<BarCheese>::value, "");
    static_assert(not sqlpp::is_group_by_column<BarCheese>::value, "");

    static_assert(std::is_same<sqlpp::name_tag_of_t<BarCheese>,
                               cheese_t::_sqlpp_name_tag>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<BarCheese>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<BarCheese>,
                               sqlpp::provided_tables_of_t<BarCheese>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<BarCheese>,
                               sqlpp::provided_tables_of_t<BarCheese>>::value,
                  "");
    static_assert(
        std::is_same<
            sqlpp::required_tables_of_t<BarCheese>,
            sqlpp::detail::type_set<sqlpp::table_as_t<
                test::TabFoo_, test::TabBar_::_sqlpp_name_tag>>>::value,
        "");
    static_assert(std::is_same<sqlpp::required_static_tables_of_t<BarCheese>,
                               sqlpp::required_tables_of_t<BarCheese>>::value,
                  "");

    static_assert(std::is_same<sqlpp::value_type_of_t<BarCheese>,
                               sqlpp::no_value_t>::value,
                  "");
  }

  {
    // Column optional (can be null) text with default.
    auto bar = test::TabBar{};
    using Bar = decltype(bar);
    using TextN = decltype(bar.textN);

    static_assert(not sqlpp::is_table<TextN>::value, "");
    static_assert(sqlpp::has_default<TextN>::value, "");
    static_assert(sqlpp::is_group_by_column<TextN>::value, "");

    static_assert(std::is_same<sqlpp::name_tag_of_t<TextN>,
                               test::TabBar_::TextN::_sqlpp_name_tag>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<TextN>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<TextN>,
                               sqlpp::provided_tables_of_t<TextN>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<TextN>,
                               sqlpp::provided_tables_of_t<TextN>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_tables_of_t<TextN>,
                               sqlpp::detail::type_set<Bar>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_static_tables_of_t<TextN>,
                               sqlpp::required_tables_of_t<TextN>>::value,
                  "");

    static_assert(std::is_same<sqlpp::value_type_of_t<TextN>,
                               std::optional<sqlpp::text>>::value,
                  "");
  }

  {
    // Column bool without default.
    auto bar = test::TabBar{};
    using Bar = decltype(bar);
    using BoolNn = decltype(bar.boolNn);

    static_assert(not sqlpp::is_table<BoolNn>::value, "");
    static_assert(not sqlpp::has_default<BoolNn>::value, "");
    static_assert(sqlpp::is_group_by_column<BoolNn>::value, "");

    static_assert(std::is_same<sqlpp::name_tag_of_t<BoolNn>,
                               test::TabBar_::BoolNn::_sqlpp_name_tag>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_tables_of_t<BoolNn>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<BoolNn>,
                               sqlpp::provided_tables_of_t<BoolNn>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<BoolNn>,
                               sqlpp::provided_tables_of_t<BoolNn>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_tables_of_t<BoolNn>,
                               sqlpp::detail::type_set<Bar>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_static_tables_of_t<BoolNn>,
                               sqlpp::required_tables_of_t<BoolNn>>::value,
                  "");

    static_assert(
        std::is_same<sqlpp::value_type_of_t<BoolNn>, sqlpp::boolean>::value,
        "");
  }

  {
    // table() function
    auto bar = test::TabBar{};
    using Bar = decltype(bar);
    using Id = decltype(bar.id);

    using IdTable = decltype(Id::table());

    static_assert(std::is_same<Bar, IdTable>::value, "");
  }
}

int main() { void test_column(); }

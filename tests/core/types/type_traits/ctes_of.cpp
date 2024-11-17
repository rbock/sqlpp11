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
#include "../../../include/test_helpers.h"

namespace test
{
  SQLPP_CREATE_NAME_TAG(incomplete);
  SQLPP_CREATE_NAME_TAG(basic);
  SQLPP_CREATE_NAME_TAG(referencing);
  SQLPP_CREATE_NAME_TAG(recursive);
  SQLPP_CREATE_NAME_TAG(alias);
}  // namespace

template<typename BlankWith>
struct extract_with;

template<typename... Ctes>
struct extract_with<sqlpp::blank_with_t<Ctes...>>
{
  using type = sqlpp::with_t<Ctes...>;
};

template<typename BlankWith>
using extract_with_t = typename extract_with<BlankWith>::type;

void test_required_ctes_of()
{
  auto foo = test::TabFoo{};

  // Incomplete ctes are represented as cte_ref_t which require themselves.
  {
    using T = decltype(sqlpp::cte(test::incomplete));
    static_assert(std::is_same<sqlpp::required_ctes_of_t<T>, sqlpp::detail::type_vector<T>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<T>, sqlpp::detail::type_vector<T>>::value, "");
  }

  // Basic (complete) ctes do not require ctes, but the reference to it does require itself.
  {
    auto basic = sqlpp::cte(test::basic).as(select(foo.id).from(foo).unconditionally());
    using T = decltype(basic);
    using TRef = decltype(make_table_ref(basic));

    static_assert(std::is_same<sqlpp::required_ctes_of_t<T>, sqlpp::detail::type_vector<>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<T>, sqlpp::detail::type_vector<>>::value,
                  "");

    static_assert(std::is_same<sqlpp::required_ctes_of_t<TRef>, sqlpp::detail::type_vector<TRef>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<TRef>, sqlpp::detail::type_vector<TRef>>::value,
                  "");
  }

  // ctes referencing other CTEs require such ctes. A reference to them requires itself only, though.
  {
    auto basic = sqlpp::cte(test::basic).as(select(foo.id).from(foo).unconditionally());
    using BRef = decltype(make_table_ref(basic));

    auto referencing = sqlpp::cte(test::referencing).as(select(basic.id).from(basic).unconditionally());

    using T = decltype(referencing);
    using TRef = decltype(make_table_ref(referencing));

    static_assert(std::is_same<sqlpp::required_ctes_of_t<T>, sqlpp::detail::type_vector<BRef>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<T>, sqlpp::detail::type_vector<BRef>>::value,
                  "");

    static_assert(std::is_same<sqlpp::required_ctes_of_t<TRef>, sqlpp::detail::type_vector<TRef>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<TRef>, sqlpp::detail::type_vector<TRef>>::value,
                  "");

    // Note: cte requirements of ctes are evaluated in `with`, which does not expose those requirements.
    using W = extract_with_t<decltype(with(basic, referencing))>;
    static_assert(sqlpp::required_ctes_of_t<W>::empty(), "");
    static_assert(sqlpp::required_static_ctes_of_t<W>::empty(), "");
   }

  // Recursive CTEs require references to themselves. A reference to them requires itself only, though.
  {
    auto base = sqlpp::cte(test::recursive).as(select(sqlpp::value(1).as(foo.id)));
    using BRef = decltype(make_table_ref(base));

    auto recursive = base.union_all(select((base.id + 1).as(foo.id)).from(base).where(base.id < 10));

    using T = decltype(recursive);
    using TRef = decltype(make_table_ref(recursive));

    static_assert(std::is_same<BRef, TRef>::value, "");

    static_assert(std::is_same<sqlpp::required_ctes_of_t<T>, sqlpp::detail::type_vector<BRef>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<T>, sqlpp::detail::type_vector<BRef>>::value,
                  "");

    static_assert(std::is_same<sqlpp::required_ctes_of_t<TRef>, sqlpp::detail::type_vector<BRef>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<TRef>, sqlpp::detail::type_vector<BRef>>::value,
                  "");

    // Note: cte requirements of ctes are evaluated in `with`, which does not expose those requirements.
    using W = extract_with_t<decltype(with(recursive))>;
    static_assert(sqlpp::required_ctes_of_t<W>::empty(), "");
    static_assert(sqlpp::required_static_ctes_of_t<W>::empty(), "");
  }

  // Aliased ctes require the references to the underlying CTE.
  {
    auto basic = sqlpp::cte(test::basic).as(select(foo.id).from(foo).unconditionally());
    using BRef = decltype(make_table_ref(basic));

    using T = decltype(basic.as(test::alias));

    static_assert(std::is_same<sqlpp::required_ctes_of_t<T>, sqlpp::detail::type_vector<BRef>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<T>, sqlpp::detail::type_vector<BRef>>::value,
                  "");
  }

  // `x.join(y)` exposes both CTE requirements.
  {
    auto a = sqlpp::cte(sqlpp::alias::a).as(select(foo.id).from(foo).unconditionally());
    auto b = sqlpp::cte(sqlpp::alias::b).as(select(foo.id).from(foo).unconditionally());
    using ARef = decltype(make_table_ref(a));
    using BRef = decltype(make_table_ref(b));

    using T = decltype(a.join(b).on(a.id == b.id));

    static_assert(std::is_same<sqlpp::required_ctes_of_t<T>, sqlpp::detail::type_vector<ARef, BRef>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<T>, sqlpp::detail::type_vector<ARef, BRef>>::value,
                  "");
  }

  // `x.join(dynamic(condition, y))` exposes both CTE requirements.
  {
    auto a = sqlpp::cte(sqlpp::alias::a).as(select(foo.id).from(foo).unconditionally());
    auto b = sqlpp::cte(sqlpp::alias::b).as(select(foo.id).from(foo).unconditionally());
    using ARef = decltype(make_table_ref(a));
    using BRef = decltype(make_table_ref(b));

    using T = decltype(a.join(dynamic(true, b)).on(a.id == b.id));

    static_assert(std::is_same<sqlpp::required_ctes_of_t<T>, sqlpp::detail::type_vector<ARef, BRef>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<T>, sqlpp::detail::type_vector<ARef>>::value,
                  "");
  }

  // `from x` exposes CTE requirements
  {
    auto basic = sqlpp::cte(test::basic).as(select(foo.id).from(foo).unconditionally());
    using BRef = decltype(make_table_ref(basic));

    using T = decltype(from(basic));

    static_assert(std::is_same<sqlpp::required_ctes_of_t<T>, sqlpp::detail::type_vector<BRef>>::value, "");
    static_assert(std::is_same<sqlpp::required_static_ctes_of_t<T>, sqlpp::detail::type_vector<BRef>>::value,
                  "");
  }
}

void test_provided_ctes_of()
{
  auto foo = test::TabFoo{};

  // Incomplete ctes are represented as cte_ref_t which provide no ctes.
  {
    using T = decltype(sqlpp::cte(test::incomplete));
    static_assert(sqlpp::provided_ctes_of_t<T>::empty(), "");
    static_assert(sqlpp::provided_static_ctes_of_t<T>::empty(), "");
  }

  // Basic (complete) ctes provide ctes, but their references don't.
  {
    auto basic = sqlpp::cte(test::basic).as(select(foo.id).from(foo).unconditionally());
    using T = decltype(basic);
    using TRef = decltype(make_table_ref(basic));

    static_assert(std::is_same<sqlpp::provided_ctes_of_t<T>, sqlpp::detail::type_vector<TRef>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_ctes_of_t<T>, sqlpp::detail::type_vector<TRef>>::value,
                  "");

    static_assert(sqlpp::provided_ctes_of_t<TRef>::empty(), "");
    static_assert(sqlpp::provided_static_ctes_of_t<TRef>::empty(),
                  "");
  }

  // ctes referencing other CTEs provide themselves, not the CTEs they are referencing.
  {
    auto basic = sqlpp::cte(test::basic).as(select(foo.id).from(foo).unconditionally());
    using BRef = decltype(make_table_ref(basic));

    auto referencing = sqlpp::cte(test::referencing).as(select(basic.id).from(basic).unconditionally());

    using T = decltype(referencing);
    using TRef = decltype(make_table_ref(referencing));

    static_assert(std::is_same<sqlpp::provided_ctes_of_t<T>, sqlpp::detail::type_vector<TRef>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_ctes_of_t<T>, sqlpp::detail::type_vector<TRef>>::value,
                  "");

    static_assert(sqlpp::provided_ctes_of_t<TRef>::empty(), "");
    static_assert(sqlpp::provided_static_ctes_of_t<TRef>::empty(),
                  "");

    // Note: ctes are provided by `with`.
    using W = extract_with_t<decltype(with(basic, referencing))>;
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<W>, sqlpp::detail::type_vector<BRef, TRef>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_ctes_of_t<W>, sqlpp::detail::type_vector<BRef, TRef>>::value,
                  "");

    // Note: ctes are provided by `with` and might be dynamic.
    using WDyn = extract_with_t<decltype(with(basic, dynamic(true, referencing)))>;
    static_assert(std::is_same<sqlpp::provided_ctes_of_t<WDyn>, sqlpp::detail::type_vector<BRef, TRef>>::value, "");
    static_assert(std::is_same<sqlpp::provided_static_ctes_of_t<WDyn>, sqlpp::detail::type_vector<BRef>>::value,
                  "");
   }
}

int main()
{
  void test_required_ctes_of();
  void test_provided_ctes_of();
}


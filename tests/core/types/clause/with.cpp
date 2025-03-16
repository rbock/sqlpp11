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

#include <sqlpp23/sqlpp23.h>
#include <sqlpp23/tests/core/tables.h>

namespace test {
SQLPP_CREATE_NAME_TAG(basic);
SQLPP_CREATE_NAME_TAG(referencing);
SQLPP_CREATE_NAME_TAG(recursive);
}  // namespace test

template <typename SelectWith>
struct extract_with;

template <typename... Ctes>
struct extract_with<sqlpp::statement_t<sqlpp::with_t<Ctes...>>> {
  using type = sqlpp::with_t<Ctes...>;
};

template <typename SelectWith>
using extract_with_t = typename extract_with<SelectWith>::type;

void test_with() {
  const auto foo = test::TabFoo{};

  // ctes referencing other CTEs require such ctes. `have_correct_dependencies`
  // checks that.
  {
    auto basic =
        sqlpp::cte(test::basic).as(select(foo.id).from(foo).where(true));
    using Basic = decltype(basic);

    auto referencing = sqlpp::cte(test::referencing)
                           .as(select(basic.id).from(basic).where(true));
    using Referencing = decltype(referencing);

    // Simple good cases.
    static_assert(sqlpp::have_correct_cte_dependencies<Basic>::value, "");
    static_assert(
        sqlpp::have_correct_cte_dependencies<sqlpp::dynamic_t<Basic>>::value,
        "");
    static_assert(
        sqlpp::have_correct_cte_dependencies<Basic, Referencing>::value, "");
    static_assert(sqlpp::have_correct_cte_dependencies<
                      Basic, sqlpp::dynamic_t<Referencing>>::value,
                  "");

    // The library has no way of knowing if `Basic` and `Referencing` are
    // dynamically added in the correct combinations
    // (`Basic` has to be present if `Referencing` is added). It has to assume
    // that the library user knows what they are doing.
    static_assert(
        sqlpp::have_correct_cte_dependencies<
            sqlpp::dynamic_t<Basic>, sqlpp::dynamic_t<Referencing>>::value,
        "");

    // `Referencing` requires the cte it references.
    static_assert(not sqlpp::have_correct_cte_dependencies<Referencing>::value,
                  "");
    static_assert(not sqlpp::have_correct_cte_dependencies<
                      sqlpp::dynamic_t<Referencing>>::value,
                  "");

    // `Referencing` has to mentioned after the cte it references.
    static_assert(
        not sqlpp::have_correct_cte_dependencies<Referencing, Basic>::value,
        "");
    static_assert(
        not sqlpp::have_correct_cte_dependencies<sqlpp::dynamic_t<Referencing>,
                                                 Basic>::value,
        "");
    static_assert(
        not sqlpp::have_correct_cte_dependencies<
            sqlpp::dynamic_t<Referencing>, sqlpp::dynamic_t<Basic>>::value,
        "");

    // `Referencing` statically requires the cte it references. It is not
    // sufficient to have a dynamic `Basic` cte.
    static_assert(sqlpp::have_correct_cte_dependencies<sqlpp::dynamic_t<Basic>,
                                                       Referencing>::value,
                  "");
    static_assert(
        not sqlpp::have_correct_static_cte_dependencies<sqlpp::dynamic_t<Basic>,
                                                        Referencing>::value,
        "");
  }

  // ctes dynamically referencing other CTEs require such ctes dynamically.
  // `have_correct_dependencies` checks that.
  {
    auto basic =
        sqlpp::cte(test::basic).as(select(foo.id).from(foo).where(true));
    using Basic = decltype(basic);

    auto referencing =
        sqlpp::cte(test::referencing)
            .as(select(dynamic(true, basic.id))
                    .from(foo.join(dynamic(true, basic)).on(foo.id == basic.id))
                    .where(true));
    using Referencing = decltype(referencing);

    // Simple good cases.
    static_assert(sqlpp::have_correct_cte_dependencies<Basic>::value, "");
    static_assert(
        sqlpp::have_correct_cte_dependencies<sqlpp::dynamic_t<Basic>>::value,
        "");
    static_assert(
        sqlpp::have_correct_cte_dependencies<Basic, Referencing>::value, "");
    static_assert(sqlpp::have_correct_cte_dependencies<
                      Basic, sqlpp::dynamic_t<Referencing>>::value,
                  "");

    // The library has no way of knowing if `Basic` and `Referencing` are
    // dynamically added in the correct combinations
    // (`Basic` has to be present if `Referencing` is added). It has to assume
    // that the library user knows what they are doing.
    static_assert(
        sqlpp::have_correct_cte_dependencies<
            sqlpp::dynamic_t<Basic>, sqlpp::dynamic_t<Referencing>>::value,
        "");

    // `Referencing` requires the cte it references.
    static_assert(not sqlpp::have_correct_cte_dependencies<Referencing>::value,
                  "");
    static_assert(not sqlpp::have_correct_cte_dependencies<
                      sqlpp::dynamic_t<Referencing>>::value,
                  "");

    // `Referencing` has to mentioned after the cte it references.
    static_assert(
        not sqlpp::have_correct_cte_dependencies<Referencing, Basic>::value,
        "");
    static_assert(
        not sqlpp::have_correct_cte_dependencies<sqlpp::dynamic_t<Referencing>,
                                                 Basic>::value,
        "");
    static_assert(
        not sqlpp::have_correct_cte_dependencies<
            sqlpp::dynamic_t<Referencing>, sqlpp::dynamic_t<Basic>>::value,
        "");

    // `Referencing` dynamically requires the cte it references. It is
    // sufficient to have a dynamic `Basic` cte.
    static_assert(sqlpp::have_correct_cte_dependencies<sqlpp::dynamic_t<Basic>,
                                                       Referencing>::value,
                  "");
    static_assert(
        sqlpp::have_correct_static_cte_dependencies<sqlpp::dynamic_t<Basic>,
                                                    Referencing>::value,
        "");
  }

  // Self-referencing CTEs do not necessarily require other ctes.
  // `have_correct_dependencies` checks that.
  {
    auto basic =
        sqlpp::cte(test::basic).as(select(foo.id).from(foo).where(true));
    using Basic = decltype(basic);

    auto recursive_base = sqlpp::cte(test::recursive)
                              .as(select(sqlpp::value(1).as(sqlpp::alias::a)));
    auto recursive = recursive_base.union_all(
        select((recursive_base.a + 1).as(sqlpp::alias::a))
            .from(recursive_base)
            .where(recursive_base.a <= 10));
    using Recursive = decltype(recursive);

    // Simple good cases.
    static_assert(sqlpp::have_correct_cte_dependencies<Basic>::value, "");
    static_assert(
        sqlpp::have_correct_cte_dependencies<sqlpp::dynamic_t<Basic>>::value,
        "");
    static_assert(sqlpp::have_correct_cte_dependencies<Basic, Recursive>::value,
                  "");
    static_assert(sqlpp::have_correct_cte_dependencies<
                      Basic, sqlpp::dynamic_t<Recursive>>::value,
                  "");

    // Since `Recursive` does not reference `Basic`, they can be combined in any
    // order.
    static_assert(sqlpp::have_correct_cte_dependencies<Recursive, Basic>::value,
                  "");
    static_assert(sqlpp::have_correct_cte_dependencies<Basic, Recursive>::value,
                  "");
    static_assert(
        sqlpp::have_correct_cte_dependencies<sqlpp::dynamic_t<Recursive>,
                                             sqlpp::dynamic_t<Basic>>::value,
        "");
    static_assert(
        sqlpp::have_correct_cte_dependencies<
            sqlpp::dynamic_t<Basic>, sqlpp::dynamic_t<Recursive>>::value,
        "");
  }

  // Self-referencing CTEs can require other ctes. `have_correct_dependencies`
  // checks that, too.
  {
    auto basic =
        sqlpp::cte(test::basic).as(select(foo.id).from(foo).where(true));
    using Basic = decltype(basic);

    auto recursive_base =
        sqlpp::cte(test::recursive)
            .as(select(basic.id.as(sqlpp::alias::a)).from(basic).where(true));
    auto recursive = recursive_base.union_all(
        select((recursive_base.a + 1).as(sqlpp::alias::a))
            .from(recursive_base)
            .where(recursive_base.a <= 10));
    using Recursive = decltype(recursive);

    // Simple good cases.
    static_assert(sqlpp::have_correct_cte_dependencies<Basic>::value, "");
    static_assert(
        sqlpp::have_correct_cte_dependencies<sqlpp::dynamic_t<Basic>>::value,
        "");
    static_assert(sqlpp::have_correct_cte_dependencies<Basic, Recursive>::value,
                  "");
    static_assert(sqlpp::have_correct_cte_dependencies<
                      Basic, sqlpp::dynamic_t<Recursive>>::value,
                  "");
    static_assert(
        sqlpp::have_correct_cte_dependencies<
            sqlpp::dynamic_t<Basic>, sqlpp::dynamic_t<Recursive>>::value,
        "");

    // Since `Recursive` references `Basic`, the order matters
    static_assert(
        not sqlpp::have_correct_cte_dependencies<Recursive, Basic>::value, "");
    static_assert(
        not sqlpp::have_correct_cte_dependencies<
            sqlpp::dynamic_t<Recursive>, sqlpp::dynamic_t<Basic>>::value,
        "");
  }

  // `with` exposes parameters from it's CTEs
  {
    auto a = sqlpp::parameter(sqlpp::boolean{}, sqlpp::alias::a);
    using A = decltype(a);
    auto b = sqlpp::parameter(sqlpp::boolean{}, sqlpp::alias::b);
    using B = decltype(b);

    auto basic = sqlpp::cte(test::basic).as(select(foo.id).from(foo).where(a));
    auto referencing =
        sqlpp::cte(test::referencing).as(select(basic.id).from(basic).where(b));

    {
      using W = extract_with_t<decltype(with(basic))>;
      static_assert(std::is_same<sqlpp::parameters_of_t<W>,
                                 sqlpp::detail::type_vector<A>>::value,
                    "");
    }
    {
      using W = extract_with_t<decltype(with(basic, referencing))>;
      static_assert(std::is_same<sqlpp::parameters_of_t<W>,
                                 sqlpp::detail::type_vector<A, B>>::value,
                    "");
    }
    {
      using W = extract_with_t<decltype(with(dynamic(true, basic),
                                             dynamic(false, referencing)))>;
      static_assert(std::is_same<sqlpp::parameters_of_t<W>,
                                 sqlpp::detail::type_vector<A, B>>::value,
                    "");
    }
  }
}

int main() {
  void test_with();
}

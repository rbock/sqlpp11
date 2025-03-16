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
#include <sqlpp23/tests/core/types_helpers.h>

void test_from() {
  const auto maybe = true;
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // FROM table
  {
    auto statement = from(foo);

    using F = typename std::decay<decltype(foo)>::type;
    using S = decltype(statement);
    using FROM = extract_clause_t<S>;

    static_assert(std::is_same<sqlpp::provided_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // FROM dynamic table
  {
    auto statement = from(dynamic(maybe, foo));

    using F = typename std::decay<decltype(foo)>::type;
    using S = decltype(statement);
    using FROM = extract_clause_t<S>;

    static_assert(std::is_same<sqlpp::provided_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // FROM table join table
  {
    auto statement = from(foo.cross_join(bar));

    using F = typename std::decay<decltype(foo)>::type;
    using B = typename std::decay<decltype(bar)>::type;
    using S = decltype(statement);
    using FROM = extract_clause_t<S>;

    static_assert(std::is_same<sqlpp::provided_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // FROM table join dynamic table
  {
    auto statement = from(foo.cross_join(dynamic(maybe, bar)));

    using F = typename std::decay<decltype(foo)>::type;
    using B = typename std::decay<decltype(bar)>::type;
    using S = decltype(statement);
    using FROM = extract_clause_t<S>;

    static_assert(std::is_same<sqlpp::provided_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // FROM table left_outer_join table
  {
    auto statement = from(foo.left_outer_join(bar).on(foo.id == bar.id));

    using F = typename std::decay<decltype(foo)>::type;
    using B = typename std::decay<decltype(bar)>::type;
    using S = decltype(statement);
    using FROM = extract_clause_t<S>;

    static_assert(std::is_same<sqlpp::provided_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<FROM>,
                               sqlpp::detail::type_set<B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // FROM table right_outer_join dynamic table
  {
    auto statement =
        from(foo.right_outer_join(dynamic(maybe, bar)).on(foo.id == bar.id));

    using F = typename std::decay<decltype(foo)>::type;
    using B = typename std::decay<decltype(bar)>::type;
    using S = decltype(statement);
    using FROM = extract_clause_t<S>;

    static_assert(std::is_same<sqlpp::provided_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<FROM>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // FROM CTE
  {
    auto x = cte(sqlpp::alias::x).as(select(foo.id).from(foo).where(true));
    auto statement = from(x);

    using R = decltype(make_table_ref(x));
    using S = decltype(statement);
    using FROM = extract_clause_t<S>;

    static_assert(std::is_same<sqlpp::provided_tables_of_t<FROM>,
                               sqlpp::detail::type_set<R>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<FROM>,
                               sqlpp::detail::type_set<R>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<FROM>,
                               sqlpp::detail::type_set<>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_ctes_of_t<FROM>,
                               sqlpp::detail::type_set<R>>::value,
                  "");
  }
}

int main() {
  void test_from();
}

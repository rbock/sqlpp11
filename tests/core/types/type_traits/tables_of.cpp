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

SQLPP_CREATE_NAME_TAG(cheese);

void test_required_tables_of() {
  // Columns require tables.
  {
    using T = decltype(test::TabFoo{}.id);
    static_assert(std::is_same<sqlpp::required_tables_of_t<T>,
                               sqlpp::detail::type_set<test::TabFoo>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_static_tables_of_t<T>,
                               sqlpp::detail::type_set<test::TabFoo>>::value,
                  "");
  }

  // Tables do not require tables.
  {
    using T = decltype(test::TabFoo{});
    static_assert(sqlpp::required_tables_of_t<T>::empty(), "");
    static_assert(sqlpp::required_static_tables_of_t<T>::empty(), "");
  }

  // Static expressions require collective tables.
  {
    using TF = test::TabFoo;
    using TB = test::TabBar;
    using TC = decltype(test::TabFoo{}.as(cheese));
    using T = decltype(TF{}.id + TB{}.id + TC{}.id);
    static_assert(std::is_same<sqlpp::required_tables_of_t<T>,
                               sqlpp::detail::type_set<TF, TB, TC>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_static_tables_of_t<T>,
                               sqlpp::detail::type_set<TF, TB, TC>>::value,
                  "");
  }

  // Dynamic expressions require all tables, but on the the static parts
  // contribute to the statically required tables.
  {
    using TF = test::TabFoo;
    using TB = test::TabBar;
    using TC = decltype(test::TabFoo{}.as(cheese));
    using T =
        decltype(TF{}.id < 17 and dynamic(true, TB{}.id < 17) and TC{}.id < 17);
    static_assert(std::is_same<sqlpp::required_tables_of_t<T>,
                               sqlpp::detail::type_set<TF, TB, TC>>::value,
                  "");
    static_assert(std::is_same<sqlpp::required_static_tables_of_t<T>,
                               sqlpp::detail::type_set<TF, TC>>::value,
                  "");
  }
}

void test_provided_tables_of() {
  // Columns do not provide tables.
  {
    using T = decltype(test::TabFoo{}.id);
    static_assert(sqlpp::provided_tables_of_t<T>::empty(), "");
    static_assert(sqlpp::provided_static_tables_of_t<T>::empty(), "");
    static_assert(sqlpp::provided_optional_tables_of_t<T>::empty(), "");
  }

  // Tables provide tables.
  {
    using T = test::TabFoo;
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<T>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<T>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // Schema-qualified tables provide tables.
  {
    using T =
        decltype(schema_qualified_table({"meme"}, test::TabFoo{}).as(cheese));
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<T>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<T>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // Tables AS provide tables.
  {
    using T = decltype(test::TabFoo{}.as(cheese));
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<T>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<T>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // SELECT AS provides tables.
  {
    using T = decltype(select(test::TabFoo{}.id)
                           .from(test::TabFoo{})
                           .where(true)
                           .as(cheese));
    using Ref = sqlpp::select_ref_t<cheese_t::_sqlpp_name_tag>;
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<Ref>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<Ref>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // JOIN provides tables.
  {
    using F = test::TabFoo;
    using B = test::TabBar;
    using T = decltype(F{}.cross_join(B{}));
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // Dynamic JOIN provides non-static tables.
  {
    using F = test::TabFoo;
    using B = test::TabBar;
    using T = decltype(F{}.cross_join(dynamic(true, B{})));
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<>>::value,
                  "");
  }

  // Left outer join makes right-hand-side table "optional", i.e. columns can by
  // NULL.
  {
    using F = test::TabFoo;
    using B = test::TabBar;
    using T =
        decltype(F{}.left_outer_join(dynamic(true, B{})).on(F{}.id == B{}.id));
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<B>>::value,
                  "");
  }

  // Right outer join makes left-hand-side table "optional", i.e. columns can by
  // NULL.
  {
    using F = test::TabFoo;
    using B = test::TabBar;
    using T =
        decltype(F{}.right_outer_join(dynamic(true, B{})).on(F{}.id == B{}.id));
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
  }

  // Full outer join makes left-hand-side table "optional", i.e. columns can by
  // NULL.
  {
    using F = test::TabFoo;
    using B = test::TabBar;
    using T =
        decltype(F{}.full_outer_join(dynamic(true, B{})).on(F{}.id == B{}.id));
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
  }

  // Nested joins propagate their provided tables.
  {
    using F = test::TabFoo;
    using B = test::TabBar;
    using C = decltype(test::TabFoo{}.as(cheese));
    using T = decltype(C{}.cross_join(
        F{}.full_outer_join(dynamic(true, B{})).on(F{}.id == B{}.id)));
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<C, F, B>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<C, F>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
  }

  // Nested joins propagate their provided tables.
  {
    using F = test::TabFoo;
    using B = test::TabBar;
    using C = decltype(test::TabFoo{}.as(cheese));
    using T = decltype(F{}.full_outer_join(dynamic(true, B{}))
                           .on(F{}.id == B{}.id)
                           .cross_join(C{}));
    static_assert(std::is_same<sqlpp::provided_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B, C>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_static_tables_of_t<T>,
                               sqlpp::detail::type_set<F, C>>::value,
                  "");
    static_assert(std::is_same<sqlpp::provided_optional_tables_of_t<T>,
                               sqlpp::detail::type_set<F, B>>::value,
                  "");
  }
}

int main() {
  void test_required_tables_of();
  void test_provided_tables_of();
}

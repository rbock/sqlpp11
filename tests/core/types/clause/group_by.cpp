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

namespace {
template <typename S>
auto known_aggregate_columns_of(S)
    -> sqlpp::known_aggregate_columns_of_t<extract_clause_t<S>>;

template <typename S>
auto known_static_aggregate_columns_of(S)
    -> sqlpp::known_static_aggregate_columns_of_t<extract_clause_t<S>>;

}  // namespace

void test_group_by() {
  using sqlpp::detail::type_set;

  auto v = sqlpp::value(17);
  auto id = test::TabFoo{}.id;
  auto textNnD = test::TabFoo{}.textNnD;

  using V = decltype(v);
  using Id = decltype(id);
  using TextNnD = decltype(textNnD);

  // Test that declared group by columns are considered group by columns
  static_assert(
      sqlpp::is_group_by_column<decltype(declare_group_by_column(v))>::value,
      "");

  // Static columns are listed as such in known_aggregate_columns_of_t.
  {
    const auto g = group_by(id);
    using AC = decltype(known_aggregate_columns_of(g));
    using SAC = decltype(known_static_aggregate_columns_of(g));

    static_assert(std::is_same<AC, type_set<Id>>::value, "");
    static_assert(std::is_same<SAC, type_set<Id>>::value, "");
  }
  {
    const auto g = group_by(id, textNnD);
    using AC = decltype(known_aggregate_columns_of(g));
    using SAC = decltype(known_static_aggregate_columns_of(g));

    static_assert(std::is_same<AC, type_set<Id, TextNnD>>::value, "");
    static_assert(std::is_same<SAC, type_set<Id, TextNnD>>::value, "");
  }

  // Dynamic columns are are not listed in known_static_aggregate_columns_of_t.
  {
    const auto g = group_by(dynamic(true, id));
    using AC = decltype(known_aggregate_columns_of(g));
    using SAC = decltype(known_static_aggregate_columns_of(g));

    static_assert(std::is_same<AC, type_set<Id>>::value, "");
    static_assert(std::is_same<SAC, type_set<>>::value, "");
  }
  {
    const auto g = group_by(id, dynamic(true, textNnD));
    using AC = decltype(known_aggregate_columns_of(g));
    using SAC = decltype(known_static_aggregate_columns_of(g));

    static_assert(std::is_same<AC, type_set<Id, TextNnD>>::value, "");
    static_assert(std::is_same<SAC, type_set<Id>>::value, "");
  }

  // Declared columns are listed similar to regular columns.
  {
    const auto g = group_by(declare_group_by_column(v));
    using AC = decltype(known_aggregate_columns_of(g));
    using SAC = decltype(known_static_aggregate_columns_of(g));

    static_assert(std::is_same<AC, type_set<V>>::value, "");
    static_assert(std::is_same<SAC, type_set<V>>::value, "");
  }
  {
    const auto g = group_by(dynamic(true, declare_group_by_column(v)));
    using AC = decltype(known_aggregate_columns_of(g));
    using SAC = decltype(known_static_aggregate_columns_of(g));

    static_assert(std::is_same<AC, type_set<V>>::value, "");
    static_assert(std::is_same<SAC, type_set<>>::value, "");
  }
}

int main() {
  void test_group_by();
}

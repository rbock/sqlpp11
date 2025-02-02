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

#include <sqlpp11/tests/core/constraints_helpers.h>
#include <sqlpp11/tests/postgresql/tables.h>
#include <sqlpp11/core/compat/type_traits.h>

#include <sqlpp11/postgresql/postgresql.h>

namespace
{
  SQLPP_CREATE_NAME_TAG(something);

  // Returns true if `returning_columns(declval<Expressions>()...)` is a valid function call.
  template <typename TypeVector, typename = void>
  struct can_call_returning_columns_with_impl : public std::false_type
  {
  };

  template <typename... Expressions>
  struct can_call_returning_columns_with_impl<sqlpp::detail::type_vector<Expressions...>,
                                  sqlpp::void_t<decltype(sqlpp::postgresql::returning_columns(std::declval<Expressions>()...))>>
      : public std::true_type
  {
  };

  template <typename... Expressions>
  struct can_call_returning_columns_with : public can_call_returning_columns_with_impl<sqlpp::detail::type_vector<Expressions...>>
  {
  };

}  // namespace

namespace test {
  SQLPP_CREATE_NAME_TAG(max_id);
}

int main()
{
  const auto maybe = true;
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  using sqlpp::postgresql::returning_columns;

  // OK
  returning_columns(foo.id);
  returning_columns(foo.id, foo.textNnD);
  returning_columns(all_of(foo));
  returning_columns(foo.id, bar.id);
  returning_columns(all_of(foo), bar.id.as(something));

  // -------------------------
  // returning_columns() can be constructed, but is inconsistent since no columns are selected.
  // -------------------------
  {
    static_assert(can_call_returning_columns_with<>::value, "");
    SQLPP_CHECK_STATIC_ASSERT(sqlpp::postgresql::returning_columns(), "at least one return column required");
  }

  // -------------------------
  // returning_columns(<unnamed>) can be constructed, but is inconsistent columns require a name.
  // -------------------------
  {
    static_assert(can_call_returning_columns_with<decltype(7)>::value, "");
    SQLPP_CHECK_STATIC_ASSERT(sqlpp::postgresql::returning_columns(7), "each return column must have a name");

    static_assert(can_call_returning_columns_with<decltype(sqlpp::dynamic(maybe, 7))>::value, "");
    SQLPP_CHECK_STATIC_ASSERT(sqlpp::postgresql::returning_columns(7), "each return column must have a name");

    static_assert(can_call_returning_columns_with<decltype(all_of(bar)), decltype(7)>::value, "");
    SQLPP_CHECK_STATIC_ASSERT((sqlpp::postgresql::returning_columns(all_of(bar), 7)), "each return column must have a name");
  }

  // -------------------------
  // returning_columns is not required
  // -------------------------
  {
    using I = sqlpp::statement_t<sqlpp::postgresql::no_returning_column_list_t>;

    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<I>, sqlpp::consistent_t>::value, "");
  }

  // -------------------------
  // insert(...).returning(<aggregate functions>)
  // -------------------------
  {
    auto i = sqlpp::postgresql::insert_into(foo).default_values().returning(max(foo.id).as(something));
    using I = decltype(i);

    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>, sqlpp::postgresql::assert_returning_columns_contain_no_aggregates_t>::value, "");
  }

  // -------------------------
  // insert(...).returning(<unknown_tables>)
  // -------------------------
  {
    auto i = sqlpp::postgresql::insert_into(foo).default_values().returning(bar.id);
    using I = decltype(i);

    static_assert(std::is_same<sqlpp::statement_consistency_check_t<I>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<I>, sqlpp::postgresql::assert_no_unknown_tables_in_returning_columns_t>::value, "");
  }

}


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

namespace
{
  SQLPP_CREATE_NAME_TAG(cheese);

  template <typename T>
  struct clause_of;

  template <typename T>
  struct clause_of<sqlpp::statement_t<T>>
  {
    using type = T;
  };
  template <typename T>
  using clause_of_t = typename clause_of<T>::type;
}

void test_select_columns()
{
  auto v = sqlpp::value("text");
  auto col_int = test::TabFoo{}.id;
  auto col_txt = test::TabFoo{}.textNnD;
  auto col_bool = test::TabFoo{}.boolN;

  using unknown = sqlpp::detail::type_set<>;
  using knownInt = sqlpp::detail::type_set<decltype(col_int)>;
  using knownTxt = sqlpp::detail::type_set<decltype(col_txt)>;


  // Single column.
  {
    using T = clause_of_t<decltype(select_columns(col_int))>;
    static_assert(not sqlpp::has_name_tag<T>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::integral>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
  }

  // Single dynamic column.
  {
    auto t = select_columns(dynamic(maybe, col_int));
    using T = clause_of_t<decltype(t)>;
    static_assert(not sqlpp::has_name_tag<T>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::optional<sqlpp::integral>>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
  }

  // Single aggregate function.
  {
    auto t = select_columns(avg(col_int).as(cheese));
    using T = clause_of_t<decltype(t)>;
    static_assert(not sqlpp::has_name_tag<T>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::optional<sqlpp::floating_point>>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
  }

  // Single dynamic aggregate function.
  {
    auto t = select_columns(dynamic(maybe, avg(col_int)).as(cheese));
    using T = clause_of_t<decltype(t)>;
    static_assert(not sqlpp::has_name_tag<T>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::optional<sqlpp::floating_point>>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
  }

  // Single declared group by column.
  {
    using T = clause_of_t<decltype(select_columns(declare_group_by_column(v).as(cheese)))>;
    static_assert(not sqlpp::has_name_tag<T>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::text>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
  }

  // Single dynamic declared group by column.
  {
    using T = clause_of_t<decltype(select_columns(dynamic(maybe, declare_group_by_column(v)).as(cheese)))>;
    static_assert(not sqlpp::has_name_tag<T>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::optional<sqlpp::text>>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
  }

  // Multiple columns.
  {
    using T = clause_of_t<decltype(select_columns(col_int, col_txt, col_bool))>;
    static_assert(not sqlpp::has_name_tag<T>::value, "");
    static_assert(not sqlpp::has_value_type<T>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
  }

  // Mixed columns.
  {
    using T = clause_of_t<decltype(select_columns(col_int, max(col_txt), declare_group_by_column(v).as(cheese)))>;
    static_assert(not sqlpp::has_name_tag<T>::value, "");
    static_assert(not sqlpp::has_value_type<T>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
  }

}

int main()
{
  void test_group_by();
}


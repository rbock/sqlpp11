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

namespace
{
  SQLPP_ALIAS_PROVIDER(cheese);

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

  using unknown = sqlpp::detail::type_vector<>;
  using knownInt = sqlpp::detail::type_vector<decltype(col_int)>;
  using knownTxt = sqlpp::detail::type_vector<decltype(col_txt)>;


#warning test that dynamic join tables are only used by dynamic columns

  // Single column.
  {
    using T = clause_of_t<decltype(select_columns(col_int))>;
    static_assert(std::is_same<sqlpp::name_tag_of_t<T>, test::TabFoo_::Id::_sqlpp_name_tag>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::integral>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<unknown, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownInt, T>::value, "");
    static_assert(not sqlpp::has_correct_aggregates<knownTxt, T>::value, "");
  }

  // Single dynamic column.
  {
    using T = clause_of_t<decltype(select_columns(dynamic(true, col_int)))>;
    static_assert(std::is_same<sqlpp::name_tag_of_t<T>, test::TabFoo_::Id::_sqlpp_name_tag>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::optional<sqlpp::integral>>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<unknown, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownInt, T>::value, "");
    static_assert(not sqlpp::has_correct_aggregates<knownTxt, T>::value, "");
  }

  // Single aggregate function.
  {
    using T = clause_of_t<decltype(select_columns(avg(col_int)))>;
    static_assert(std::is_same<sqlpp::name_tag_of_t<T>, sqlpp::alias::_avg_t::_sqlpp_name_tag>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::optional<sqlpp::floating_point>>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<unknown, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownInt, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownTxt, T>::value, "");
  }

  // Single dynamic aggregate function.
  {
    using T = clause_of_t<decltype(select_columns(dynamic(true, avg(col_int))))>;
    static_assert(std::is_same<sqlpp::name_tag_of_t<T>, sqlpp::alias::_avg_t::_sqlpp_name_tag>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::optional<sqlpp::floating_point>>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<unknown, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownInt, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownTxt, T>::value, "");
  }

  // Single declared group by column.
  {
    using T = clause_of_t<decltype(select_columns(declare_group_by_column(v).as(cheese)))>;
    static_assert(std::is_same<sqlpp::name_tag_of_t<T>, cheese_t::_sqlpp_name_tag>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::text>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<unknown, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownInt, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownTxt, T>::value, "");
  }

  // Single dynamic declared group by column.
  {
    using T = clause_of_t<decltype(select_columns(dynamic(true, declare_group_by_column(v)).as(cheese)))>;
    static_assert(std::is_same<sqlpp::name_tag_of_t<T>, cheese_t::_sqlpp_name_tag>::value, "");
    static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::optional<sqlpp::text>>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<unknown, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownInt, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownTxt, T>::value, "");
  }

  // Mixed columns.
  // The columns in group_by determine if aggregates are correct or not.
  {
    using T = clause_of_t<decltype(select_columns(col_int, col_txt, col_bool))>;
    static_assert(not sqlpp::has_name<T>::value, "");
    static_assert(not sqlpp::has_value_type<T>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<unknown, T>::value, "");
    static_assert(not sqlpp::has_correct_aggregates<knownInt, T>::value, ""); // col_int is a known aggregate here.
    static_assert(not sqlpp::has_correct_aggregates<knownTxt, T>::value, "");
  }

  // Mixed columns.
  // The columns in group_by determine if aggregates are correct or not.
  {
    using T = clause_of_t<decltype(select_columns(col_int, max(col_txt), declare_group_by_column(v).as(cheese)))>;
    static_assert(not sqlpp::has_name<T>::value, "");
    static_assert(not sqlpp::has_value_type<T>::value, "");
    static_assert(sqlpp::is_result_clause<T>::value, "");
    static_assert(not sqlpp::has_correct_aggregates<unknown, T>::value, "");
    static_assert(sqlpp::has_correct_aggregates<knownInt, T>::value, ""); // col_int is a known aggregate here.
    static_assert(not sqlpp::has_correct_aggregates<knownTxt, T>::value, "");
  }

}

int main()
{
  void test_group_by();
}

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
#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/core/compat/type_traits.h>

namespace
{
  SQLPP_CREATE_NAME_TAG(something);

  // Returns true if `select_columns(declval<Expressions>()...)` is a valid function call.
  template <typename TypeVector, typename = void>
  struct can_call_select_columns_with_impl : public std::false_type
  {
  };

  template <typename... Expressions>
  struct can_call_select_columns_with_impl<sqlpp::detail::type_vector<Expressions...>,
                                  sqlpp::void_t<decltype(sqlpp::select_columns(std::declval<Expressions>()...))>>
      : public std::true_type
  {
  };

  template <typename... Expressions>
  struct can_call_select_columns_with : public can_call_select_columns_with_impl<sqlpp::detail::type_vector<Expressions...>>
  {
  };

}  // namespace

namespace test {
  SQLPP_CREATE_NAME_TAG(max_id);
}

int main()
{
  const auto foo = test::TabFoo{};
  const auto bar = test::TabBar{};

  // Confirming the required columns of TabBar.
  static_assert(std::is_same<sqlpp::required_insert_columns_of_t<test::TabBar>,
                             sqlpp::detail::type_set<sqlpp::column_t<test::TabBar, test::TabBar_::BoolNn>>>::value,
                "");

  // -------------------------
  // select() can be constructed, but is inconsistent since not columns are selected.
  // -------------------------
  {
    auto s = sqlpp::select();
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_columns_selected_t>::value, "");
  }

  // -------------------------
  // select().columns(...)
  // -------------------------

  // select_columns(<non arguments>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::select_columns(), "at least one selected column required");

  // select_columns(<arguments with no value>) cannot be called.
  static_assert(can_call_select_columns_with<decltype(bar.boolNn)>::value, "OK, argument a column");
  static_assert(can_call_select_columns_with<decltype(bar.id == 7)>::value, "OK to call, but will fail static_assert later");
  static_assert(not can_call_select_columns_with<decltype(bar.id = 7), decltype(bar.boolNn)>::value, "not value: assignment");


  // select_columns(<at least one unnamed column>) is inconsistent and cannot be constructed.
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::select_columns(sqlpp::value(7)), "each selected column must have a name");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::select_columns(bar.id, max(foo.id)), "each selected column must have a name");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::select_columns(all_of(bar), max(foo.id)), "each selected column must have a name");

  SQLPP_CHECK_STATIC_ASSERT(sqlpp::select_columns(dynamic(true, sqlpp::value(7))), "each selected column must have a name");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::select_columns(bar.id, dynamic(true, max(foo.id))), "each selected column must have a name");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::select_columns(dynamic(true, bar.id), max(foo.id)), "each selected column must have a name");
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::select_columns(all_of(bar), dynamic(true, max(foo.id))), "each selected column must have a name");
  // Note: There is no `dynamic(condition, all_of(table))`

  // select_columns(<selecting table columns without `from`>) can be constructed but is inconsistent.
  {
    auto s = sqlpp::select_columns(bar.id);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_no_unknown_tables_in_selected_columns_t>::value, "");
  }

  {
    auto s = sqlpp::select_columns(dynamic(true, bar.id));
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::assert_no_unknown_tables_in_selected_columns_t>::value, "");
  }

  // ----------------------------
  // ------- Aggregates ---------
  // ----------------------------
  // select_columns(<mix of aggregate and non-aggregate columns>) can be constructed but is inconsistent.
  {
    auto s = select(foo.id, max(foo.id).as(test::max_id)).from(foo);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_select_columns_all_aggregates_t>::value, "");
  }

  {
    auto s = select(foo.id, (max(foo.id) + 7).as(test::max_id)).from(foo);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_select_columns_all_aggregates_t>::value, "");
  }

  {
    auto s = select(foo.id, foo.intN).from(foo).group_by(foo.intN);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_select_columns_with_group_by_are_aggregates_t>::value, "");
  }

  {
    auto s = select(foo.id, dynamic(true, foo.intN)).from(foo).group_by(foo.intN);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_select_columns_with_group_by_are_aggregates_t>::value, "");
  }

  {
    auto s = select(foo.id, dynamic(true, (foo.intN + 7)).as(test::max_id)).from(foo).group_by(foo.intN);
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_select_columns_with_group_by_are_aggregates_t>::value, "");
  }

  // Dynamic group by column
  {
    auto s = select(foo.id, dynamic(true, foo.intN).as(test::max_id))
                 .from(foo)
                 .unconditionally()
                 .group_by(foo.id, dynamic(true, foo.intN));
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }
  {
    auto s = select(foo.id, foo.intN).from(foo).group_by(foo.id, dynamic(true, foo.intN));
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_select_columns_with_group_by_match_static_aggregates_t>::value, "");
  }
  {
    auto s = select(foo.id, (foo.intN + 7).as(test::max_id)).from(foo).group_by(foo.id, dynamic(true, foo.intN));
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_select_columns_with_group_by_match_static_aggregates_t>::value, "");
  }

  // ----------------------------
  // ------- Join  --------------
  // ----------------------------
  {
    auto s = select(foo.id).from(bar.cross_join(foo)).unconditionally();
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }

  {
    // Fail: Statically required table, but provided dynamically only
    auto s = select(foo.id).from(bar.cross_join(dynamic(true, foo))).unconditionally();
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>,
                               sqlpp::assert_no_unknown_static_tables_in_selected_columns_t>::value,
                  "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>,
                               sqlpp::assert_no_unknown_static_tables_in_selected_columns_t>::value,
                  "");
  }
  {
    // Fail: Statically required table, but provided dynamically only
    auto s = select(foo.id).from(dynamic(true, foo)).unconditionally();
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_no_unknown_static_tables_in_selected_columns_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>,
                               sqlpp::assert_no_unknown_static_tables_in_selected_columns_t>::value,
                  "");
  }
  {
    // Fail: This is a sub select that statically requires `bar` but provides it dynamically only.
    auto s = select(foo.id, bar.intN).from(dynamic(true, foo)).unconditionally();
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_no_unknown_static_tables_in_selected_columns_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>,
                               sqlpp::assert_no_unknown_static_tables_in_selected_columns_t>::value,
                  "");
  }
  {
    // Fail to prepare sub select: `bar` is required from the enclosing query. This would not be allowed for use as a
    // table (and also could not be prepared/executed).
    // Note that we do not detect that foo is required statically but provided dynamically only (we are not running a
    // deep analysis of expressions that have unknown tables to begin with).
#warning: Make the check recursive. That is easier than explaining why this is not detected :-)
    auto s = select((foo.id + bar.intN).as(something)).from(dynamic(true, foo)).unconditionally();
    using S = decltype(s);
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
    static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>,
                               sqlpp::assert_no_unknown_tables_in_selected_columns_t>::value,
                  "");
  }
}


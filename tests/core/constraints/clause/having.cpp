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

  // Returns true if `having(declval<Expression>())` is a valid function call.
  template <typename Expression, typename = void>
  struct can_call_having_with : public std::false_type
  {
  };

  template <typename Expression>
  struct can_call_having_with<Expression, sqlpp::void_t<decltype(sqlpp::having(std::declval<Expression>()))>>
      : public std::true_type
  {
  };
}  // namespace

int main()
{
  const auto maybe = true;
  const auto bar = test::TabBar{};
  const auto foo = test::TabFoo{};

    // OK
  static_assert(can_call_having_with<decltype(bar.boolNn)>::value, "");
  static_assert(can_call_having_with<decltype(bar.boolNn == true)>::value, "");

  // Try assignment as condition
  static_assert(not can_call_having_with<decltype(bar.boolNn = true)>::value, "");

  // Try non-boolean expression
  static_assert(not can_call_having_with<decltype(bar.id)>::value, "");

  // Try some other types as expressions
  static_assert(not can_call_having_with<decltype("true")>::value, "");
  static_assert(not can_call_having_with<decltype(17)>::value, "");
  static_assert(not can_call_having_with<decltype('c')>::value, "");
  static_assert(not can_call_having_with<decltype(nullptr)>::value, "");

  // Try alias bool column (can only be used as select column, but not as a value in `having`).
  static_assert(not can_call_having_with<decltype(bar.boolNn.as(something))>::value, "");

  // --------------------------------
  // consistency checks
  // --------------------------------
  const auto select_without_group_by = select(all_of(bar)).from(bar).unconditionally();
  const auto select_with_group_by = select(bar.id).from(bar).unconditionally().group_by(bar.id);
  const auto select_with_dynamic_group_by = select(bar.id).from(bar).unconditionally().group_by(bar.id, dynamic(maybe, bar.textN));

  // OK
  {
    using S = decltype(select_without_group_by.having(avg(bar.id) > 17));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }
  {
    using S = decltype(select_without_group_by.having(avg(bar.id) > parameter(bar.id)));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }
  {
    using S = decltype(select_with_group_by.having(avg(bar.id) > 17));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }
  {
    using S = decltype(select_with_group_by.having(avg(bar.id) > parameter(bar.id)));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }
  {
    using S = decltype(select_with_group_by.having(bar.id > parameter(bar.id)));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }

  // Try non aggregate
  {
    using S = decltype(select_without_group_by.having(bar.id > 17));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_having_all_aggregates_t>::value, "");
  }
  {
    using S = decltype(select_without_group_by.having(count(bar.id) > 3 and bar.id > 17));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_having_all_aggregates_t>::value, "");
  }
  {
    using S = decltype(select_with_group_by.having(bar.textN > "17"));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_having_all_aggregates_t>::value, "");
  }
  {
    using S = decltype(select_with_group_by.having(count(bar.textN) > 3 and bar.textN > "17"));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_having_all_aggregates_t>::value, "");
  }
  {
    using S = decltype(select_with_group_by.having(count(bar.textN) > 3 and dynamic(maybe, bar.textN > "17")));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_having_all_aggregates_t>::value, "");
  }

  // Try foreign table
  {
    using S = decltype(select_without_group_by.having(foo.doubleN > 17));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_having_all_aggregates_t>::value, "");
  }

  {
    using S = decltype(select_with_group_by.having(foo.doubleN > 17));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::assert_having_all_aggregates_t>::value, "");
  }

  // Use in sub queries: Allow foreign expressions in aggregate functions
  {
    using S = decltype(select_without_group_by.having(avg(foo.doubleN) > 17));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }

  // Use dynamic group-by expressions
  // id is statically and textN is dynamically grouped by.
  {
    using S = decltype(select_with_dynamic_group_by.having(bar.id > 3));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>, sqlpp::consistent_t>::value, "");
  }
  {
    using S = decltype(select_with_dynamic_group_by.having(bar.textN != "cheesecake"));
    static_assert(std::is_same<sqlpp::statement_consistency_check_t<S>::hansi,
                               sqlpp::assert_having_all_static_aggregates_t>::value,
                  "");
  }
}


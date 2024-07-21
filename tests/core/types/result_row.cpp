/*
 * Copyright (c) 2016-2016, Roland Bock
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

#include "MockDb.h"
#include "Sample.h"
#include <sqlpp11/sqlpp11.h>

#include "../../include/test_helpers.h"

namespace
{
  template<typename A, typename B>
  constexpr bool is_same_type()
  {
    return std::is_same<A, B>::value;
  }

  constexpr auto bar = test::TabBar{};
  constexpr auto foo = test::TabFoo{};
  auto db = MockDb{};
}

SQLPP_ALIAS_PROVIDER(r_not_null);
SQLPP_ALIAS_PROVIDER(r_maybe_null);
SQLPP_ALIAS_PROVIDER(r_opt_not_null);
SQLPP_ALIAS_PROVIDER(r_opt_maybe_null);

template<typename ResultType, typename Value>
void test_result_row(Value v)
{
  using OptResultType = sqlpp::compat::optional<ResultType>;

  // Selectable values.
  auto v_not_null = sqlpp::value(v).as(r_not_null);
  const auto v_maybe_null = sqlpp::value(sqlpp::compat::make_optional(v)).as(r_maybe_null);

  // Optional selectable values.
  const auto v_opt_not_null = dynamic(true, sqlpp::value(v).as(r_opt_not_null));
  const auto v_opt_maybe_null = dynamic(true, sqlpp::value(sqlpp::compat::make_optional(v)).as(r_opt_maybe_null));

  for (const auto& row : db(select(v_not_null, v_maybe_null, v_opt_not_null, v_opt_maybe_null)))
  {
    static_assert(std::is_same<decltype(row.r_not_null), ResultType>::value, "");
    static_assert(std::is_same<decltype(row.r_maybe_null), OptResultType>::value, "");
    static_assert(std::is_same<decltype(row.r_opt_not_null), OptResultType>::value, "");
    static_assert(std::is_same<decltype(row.r_opt_maybe_null), OptResultType>::value, "");
  }
}

int main()
{
  // boolean
  test_result_row<bool>(bool{true});

  // integral
  test_result_row<int64_t>(int8_t{7});
  test_result_row<int64_t>(int16_t{7});
  test_result_row<int64_t>(int32_t{7});
  test_result_row<int64_t>(int64_t{7});

  // unsigned integral
  test_result_row<uint64_t>(uint8_t{7});
  test_result_row<uint64_t>(uint16_t{7});
  test_result_row<uint64_t>(uint32_t{7});
  test_result_row<uint64_t>(uint64_t{7});

  // floating point
  test_result_row<double>(float{7.7});
  test_result_row<double>(double{7.7});

  // text
  test_result_row<sqlpp::compat::string_view>('7');
  test_result_row<sqlpp::compat::string_view>("seven");
  test_result_row<sqlpp::compat::string_view>(std::string("seven"));
  test_result_row<sqlpp::compat::string_view>(sqlpp::compat::string_view("seven"));

  // blob
  test_result_row<sqlpp::compat::span<uint8_t>>(std::vector<uint8_t>{});

  // date
  test_result_row<::sqlpp::chrono::day_point>(::sqlpp::chrono::day_point{});

  // timestamp
  test_result_row<::sqlpp::chrono::microsecond_point>(::sqlpp::chrono::microsecond_point{});
  using minute_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_result_row<::sqlpp::chrono::microsecond_point>(minute_point{});

  // time_of_day
  test_result_row<std::chrono::microseconds>(std::chrono::microseconds{});

  // Bit shifting combining optional value with non-optional value yields optional boolean.
  static_assert(std::is_same<sqlpp::value_type_of_t<decltype(sqlpp::value(sqlpp::compat::make_optional(7)) << 8)>,
                             sqlpp::compat::optional<sqlpp::integral>>::value,
                "");

  static_assert(std::is_same<sqlpp::value_type_of_t<decltype(sqlpp::value(8) << sqlpp::compat::make_optional(7))>,
                             sqlpp::compat::optional<sqlpp::integral>>::value,
                "");

  // assignment is no value
  static_assert(std::is_same<sqlpp::value_type_of_t<decltype(assign(bar.intN, sqlpp::compat::nullopt))>, sqlpp::no_value_t>::value, "");
  static_assert(std::is_same<sqlpp::value_type_of_t<decltype(assign(bar.intN, sqlpp::default_value))>, sqlpp::no_value_t>::value, "");

  // as expressions retain the value type of the real thing
  static_assert(sqlpp::has_name<decltype(bar.intN)>::value, "");
  sqlpp::as(bar.intN, bar.textN);
  static_assert(std::is_same<sqlpp::value_type_of_t<decltype(bar.intN.as(bar.textN))>, sqlpp::value_type_of_t<decltype(bar.intN)>>::value, "");

#if 0

  void join()
  {
    // Join
    {
      const auto rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(foo.join(bar).on(foo.doubleN > bar.id))
                             .unconditionally());
      auto& x = rows.front();
      static_assert(not is_optional<decltype(x.id)>::value, "nullable value can always be null");
      static_assert(not is_optional<decltype(x.textNnD)>::value, "left side of (inner) join cannot be null");
      static_assert(not is_optional<decltype(x.boolNn)>::value, "right side of (inner) join cannot be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
	  const auto& rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(bar.join(foo).on(foo.doubleN > bar.id))
                             .unconditionally());
          auto& x = rows.front();
      static_assert(not is_optional<decltype(x.id)>::value, "nullable value can always be null");
      static_assert(not is_optional<decltype(x.boolNn)>::value, "left side of (inner) join cannot be null");
      static_assert(not is_optional<decltype(x.textNnD)>::value, "right side of (inner) join cannot be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      MockSizeDb db2;
      auto&& result = db2(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(bar.join(foo).on(foo.doubleN > bar.id))
                             .unconditionally());
      result.size();
      static_assert(std::is_same<size_t, decltype(result.size())>::value, "MockSizeDb size() isn't size_t");
    }

    // Inner join
    {
      const auto rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(foo.inner_join(bar).on(foo.doubleN > bar.id))
                             .unconditionally());
      auto& x = rows.front();
      static_assert(not is_optional<decltype(x.id)>::value, "nullable value can always be null");
      static_assert(not is_optional<decltype(x.textNnD)>::value, "left side of inner join cannot be null");
      static_assert(not is_optional<decltype(x.boolNn)>::value, "right side of inner join cannot be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(bar.inner_join(foo).on(foo.doubleN > bar.id))
                             .unconditionally());
      auto& x = rows.front();
      static_assert(not is_optional<decltype(x.id)>::value, "primary key cannot be null");
      static_assert(not is_optional<decltype(x.boolNn)>::value, "left side of inner join cannot be null");
      static_assert(not is_optional<decltype(x.textNnD)>::value, "right side of inner join cannot be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }

    // Left outer join
    {
      const auto rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(foo.left_outer_join(bar).on(foo.doubleN > bar.id))
                             .unconditionally());
      auto& x = rows.front();
      static_assert(is_optional<decltype(x.id)>::value, "outer primary key can be null");
      static_assert(not is_optional<decltype(x.textNnD)>::value, "left side of left outer join cannot be null");
      static_assert(is_optional<decltype(x.boolNn)>::value, "right side of left outer join can be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(bar.left_outer_join(foo).on(foo.doubleN > bar.id))
                             .unconditionally());
      auto& x = rows.front();
      static_assert(not is_optional<decltype(x.id)>::value, "primary key cannot be null");
      static_assert(not is_optional<decltype(x.boolNn)>::value, "left side of left outer join cannot be null");
      static_assert(is_optional<decltype(x.textNnD)>::value, "right side of left outer join can be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }

    // Right outer join
    {
      const auto rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(foo.right_outer_join(bar).on(foo.doubleN > bar.id))
                             .unconditionally());
      auto& x = rows.front();
      static_assert(not is_optional<decltype(x.id)>::value, "primary key cannot be null");
      static_assert(is_optional<decltype(x.textNnD)>::value, "left side of right outer join can be null");
      static_assert(not is_optional<decltype(x.boolNn)>::value,
                    "right side of right outer join cannot be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(bar.right_outer_join(foo).on(foo.doubleN > bar.id))
                             .unconditionally());
      auto& x = rows.front();
      static_assert(is_optional<decltype(x.id)>::value, "left side of right outer join can be null");
      static_assert(is_optional<decltype(x.boolNn)>::value, "left side of right outer join can be null");
      static_assert(not is_optional<decltype(x.textNnD)>::value,
                    "right side of right outer join cannot be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }

    // Outer join
    {
      const auto rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(foo.outer_join(bar).on(foo.doubleN > bar.id))
                             .unconditionally());
      auto& x = rows.front();
      static_assert(is_optional<decltype(x.id)>::value, "left side of outer join can be null");
      static_assert(is_optional<decltype(x.textNnD)>::value, "left side of outer join can be null");
      static_assert(is_optional<decltype(x.boolNn)>::value, "right side of outer join can be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto rows = db(select(bar.id, foo.textNnD, bar.boolNn, seven)
                             .from(bar.outer_join(foo).on(foo.doubleN > bar.id))
                             .unconditionally());
      auto& x = rows.front();
      static_assert(is_optional<decltype(x.id)>::value, "left side of outer join can be null");
      static_assert(is_optional<decltype(x.boolNn)>::value, "left side of outer join can be null");
      static_assert(is_optional<decltype(x.textNnD)>::value, "right side of outer join can be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }

    // Cross join
    {
      const auto rows =
          db(select(bar.id, foo.textNnD, bar.boolNn, seven).from(foo.cross_join(bar)).unconditionally());
      auto& x = rows.front();
      static_assert(not is_optional<decltype(x.id)>::value, "primary key cannot be null");
      static_assert(not is_optional<decltype(x.textNnD)>::value, "left side of cross join cannot be null");
      static_assert(not is_optional<decltype(x.boolNn)>::value, "right side of cross join cannot be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto rows =
          db(select(bar.id, foo.textNnD, bar.boolNn, seven).from(bar.cross_join(foo)).unconditionally());
      auto& x = rows.front();
      static_assert(not is_optional<decltype(x.id)>::value, "primary key cannot be null");
      static_assert(not is_optional<decltype(x.boolNn)>::value, "left side of cross join cannot be null");
      static_assert(not is_optional<decltype(x.textNnD)>::value, "right side of cross join cannot be null");
      static_assert(not is_optional<decltype(x.s)>::value, "constant non-null value can not be null");
    }
  }

  void aggregates()
  {
    {
      // aggregates of nullable values
#warning use a nullable value
      const auto a = bar.id;
      static_assert(not sqlpp::can_be_null<decltype(bar.id)>::value, "");
      static_assert(not sqlpp::can_be_null<decltype(a)>::value, "");
      const auto rows = db(select(count(a), avg(a), max(a), min(a), sum(a)).from(bar).unconditionally());
      auto& x = rows.front();
      static_assert(not is_optional<decltype(x.count)>::value, "");
      static_assert(is_optional<decltype(x.avg)>::value, "");
      static_assert(is_optional<decltype(x.sum)>::value, "");
      static_assert(is_optional<decltype(x.max)>::value, "");
      static_assert(is_optional<decltype(x.min)>::value, "");
    }
    {
      // aggregates of nullable values
      const auto o = foo.doubleN;
      static_assert(sqlpp::can_be_null<decltype(foo.doubleN)>::value, "");
      static_assert(sqlpp::can_be_null<decltype(o)>::value, "");
      const auto rows = db(select(count(o), avg(o), max(o), min(o), sum(o)).from(foo).unconditionally());
      auto& x = rows.front();
      static_assert(not is_optional<decltype(x.count)>::value, "");
      static_assert(is_optional<decltype(x.avg)>::value, "");
      static_assert(is_optional<decltype(x.sum)>::value, "");
      static_assert(is_optional<decltype(x.max)>::value, "");
      static_assert(is_optional<decltype(x.min)>::value, "");
    }
  }
}

int main(int, char* [])
{
  single_table();
  optional_columns();
  join();
  aggregates();
#endif
}

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

namespace
{
  constexpr auto bar = test::TabBar{};
  constexpr auto foo = test::TabFoo{};

  static_assert(sqlpp::can_be_null_t<decltype(bar.alpha)>::value, "");
  static_assert(sqlpp::can_be_null_t<decltype(foo.omega)>::value, "");
  static_assert(not sqlpp::can_be_null_t<decltype(foo.delta)>::value, "");
  static_assert(not sqlpp::can_be_null_t<decltype(bar.gamma)>::value, "");
  const auto seven = sqlpp::value(7).as(sqlpp::alias::s);
  static_assert(not sqlpp::can_be_null_t<decltype(seven)>::value, "");

  auto db = MockDb{};

  void single_table()
  {
    {
      // result fields are as nullable as the expressions they represent
      const auto& x = db(select(bar.alpha, bar.gamma, seven).from(bar).unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "");
      static_assert(not sqlpp::can_be_null_t<decltype(x.gamma)>::value, "");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "");
    }
  }

  void join()
  {
    // Join
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(foo.join(bar).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.delta)>::value, "left side of (inner) join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.gamma)>::value, "right side of (inner) join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(bar.join(foo).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.gamma)>::value, "left side of (inner) join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.delta)>::value, "right side of (inner) join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }

    // Inner join
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(foo.inner_join(bar).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.delta)>::value, "left side of inner join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.gamma)>::value, "right side of inner join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(bar.inner_join(foo).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.gamma)>::value, "left side of inner join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.delta)>::value, "right side of inner join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }

    // Left outer join
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(foo.left_outer_join(bar).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.delta)>::value, "left side of left outer join cannot be null");
      static_assert(sqlpp::can_be_null_t<decltype(x.gamma)>::value, "right side of left outer join can be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(bar.left_outer_join(foo).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.gamma)>::value, "left side of left outer join cannot be null");
      static_assert(sqlpp::can_be_null_t<decltype(x.delta)>::value, "right side of left outer join can be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }

    // Right outer join
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(foo.right_outer_join(bar).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(sqlpp::can_be_null_t<decltype(x.delta)>::value, "left side of right outer join can be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.gamma)>::value,
                    "right side of right outer join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(bar.right_outer_join(foo).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(sqlpp::can_be_null_t<decltype(x.gamma)>::value, "left side of right outer join can be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.delta)>::value,
                    "right side of right outer join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }

    // Outer join
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(foo.outer_join(bar).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(sqlpp::can_be_null_t<decltype(x.delta)>::value, "left side of outer join can be null");
      static_assert(sqlpp::can_be_null_t<decltype(x.gamma)>::value, "right side of outer join can be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto& x = db(select(bar.alpha, foo.delta, bar.gamma, seven)
                             .from(bar.outer_join(foo).on(foo.omega > bar.alpha))
                             .unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(sqlpp::can_be_null_t<decltype(x.gamma)>::value, "left side of outer join can be null");
      static_assert(sqlpp::can_be_null_t<decltype(x.delta)>::value, "right side of outer join can be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }

    // Cross join
    {
      const auto& x =
          db(select(bar.alpha, foo.delta, bar.gamma, seven).from(foo.cross_join(bar)).unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.delta)>::value, "left side of cross join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.gamma)>::value, "right side of cross join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }
    {
      const auto& x =
          db(select(bar.alpha, foo.delta, bar.gamma, seven).from(bar.cross_join(foo)).unconditionally()).front();
      static_assert(sqlpp::can_be_null_t<decltype(x.alpha)>::value, "nullable value can always be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.gamma)>::value, "left side of cross join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.delta)>::value, "right side of cross join cannot be null");
      static_assert(not sqlpp::can_be_null_t<decltype(x.s)>::value, "constant non-null value can not be null");
    }
  }

  void aggregates()
  {
    {
      // aggregates of nullable values
      const auto a = bar.alpha;
      static_assert(sqlpp::can_be_null_t<decltype(bar.alpha)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(a)>::value, "");
      const auto& x = db(select(count(a), avg(a), max(a), min(a), sum(a)).from(bar).unconditionally()).front();
      static_assert(not sqlpp::can_be_null_t<decltype(x.count)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(x.avg)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(x.sum)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(x.max)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(x.min)>::value, "");
    }
    {
      // aggregates of nullable values
      const auto o = foo.omega;
      static_assert(sqlpp::can_be_null_t<decltype(foo.omega)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(o)>::value, "");
      const auto& x = db(select(count(o), avg(o), max(o), min(o), sum(o)).from(foo).unconditionally()).front();
      static_assert(not sqlpp::can_be_null_t<decltype(x.count)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(x.avg)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(x.sum)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(x.max)>::value, "");
      static_assert(sqlpp::can_be_null_t<decltype(x.min)>::value, "");
    }
  }
}

int main(int, char* [])
{
  single_table();
  join();
  aggregates();
}

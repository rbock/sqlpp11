/**
 * Copyright © 2023 Vesselin Atanasov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
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

#include <vector>

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include "make_test_connection.h"
#include "Tables.h"

namespace
{
  void save_regular (sqlpp::postgresql::connection& db, sqlpp::chrono::microsecond_point tp, std::chrono::microseconds tod, sqlpp::chrono::day_point dp)
  {
    test::TabDateTime tab {};
    db(
      update(tab)
      .set(
        tab.timePointNTz = tp,
        tab.timeOfDayNTz = tod,
        tab.dayPointN = dp
      )
      .unconditionally()
    );
  }

  void save_prepared (sqlpp::postgresql::connection& db, sqlpp::chrono::microsecond_point tp, std::chrono::microseconds tod, sqlpp::chrono::day_point dp)
  {
    test::TabDateTime tab {};
    auto prepared_update = db.prepare(
      update(tab)
      .set(
        tab.timePointNTz = parameter(tab.timePointNTz),
        tab.timeOfDayNTz = parameter(tab.timeOfDayNTz),
        tab.dayPointN = parameter(tab.dayPointN)
      )
      .unconditionally()
    );
    prepared_update.params.timePointNTz = tp;
    prepared_update.params.timeOfDayNTz = tod;
    prepared_update.params.dayPointN = dp;
    db(prepared_update);
  }

  template <typename L, typename R>
  void require_equal(int line, const L& l, const R& r)
  {
    if (l != r)
    {
      std::cerr << line << ": ";
      to_sql_string(::sqlpp::wrap_operand_t<L>{l}, std::cerr);
      std::cerr << " != ";
      to_sql_string(::sqlpp::wrap_operand_t<R>{r}, std::cerr);
      throw std::runtime_error("Unexpected result");
    }
  }

  void check_saved_values(sqlpp::postgresql::connection& db, sqlpp::chrono::microsecond_point tp, std::chrono::microseconds tod, sqlpp::chrono::day_point dp)
  {
    test::TabDateTime tab {};

    const auto &rows_1 = db(
      select(
        // timePointNTz as microseconds from the start of the UNIX epoch (1970-01-01 00:00:00 UTC)
        sqlpp::verbatim<sqlpp::integer>("floor(extract(epoch from time_point_n_tz)*1000000)::int8").as(sqlpp::alias::a),
        // timeOfDayNTz as microseconds from the start of the day (00:00:00 UTC)
        sqlpp::verbatim<sqlpp::integer>("floor(extract(epoch from time_of_day_n_tz)*1000000)::int8").as(sqlpp::alias::b),
        // dayPointN as days from 1970-01-01 (timezone is not applicable to date fields)
        sqlpp::verbatim<sqlpp::integer>("floor(extract(epoch from day_point_n)/86400)::int8").as(sqlpp::alias::c)
      )
      .from(tab)
      .unconditionally()
    );
    // Check if the internal values of our C++ time variables match the internal values of the PostgreSQL date/time fields.
    // This tests the conversion of date/time types from C++ to PostgreSQL while skipping the conversion from C++ to PostgreSQL.
    const auto &row_1 = rows_1.front();
    require_equal(__LINE__, row_1.a.value(), tp.time_since_epoch().count());
    require_equal(__LINE__, row_1.b.value(), tod.count());
    require_equal(__LINE__, row_1.c.value(), dp.time_since_epoch().count());

    // Check if saving date/time variables from C++ to PostgreSQL and then reading them back yields the same values.
    // This tests the conversion of date/time types from C++ to PostgreSQL and then back from PostgreSQL to C++.
    const auto rows_2 = db(select(all_of(tab)).from(tab).unconditionally());
    const auto &row_2 = rows_2.front();
    require_equal(__LINE__, row_2.timePointNTz.value(), tp);
    require_equal(__LINE__, row_2.timeOfDayNTz.value(), tod);
    require_equal(__LINE__, row_2.dayPointN.value(), dp);
  }

  void test_time_point(sqlpp::postgresql::connection& db, sqlpp::chrono::microsecond_point tp)
  {
    auto dp = date::floor<sqlpp::chrono::days> (tp);
    auto tod = tp - dp; // Time of day

    // Test time values passed in a regular (non-prepared) statement
    save_regular(db, tp, tod, dp);
    check_saved_values(db, tp, tod, dp);

    // Test time values passed in a prepared statement
    save_prepared(db, tp, tod, dp);
    check_saved_values(db, tp, tod, dp);
  }
};

int TimeZone(int, char*[])
{
  namespace sql = sqlpp::postgresql;

  // We use a time zone with non-zero offset from UTC in order to catch serialization/parsing bugs
  auto db = sql::make_test_connection("+1");

  test::createTabDateTime(db);

  test::TabDateTime tab {};
  try {
    db(insert_into(tab).default_values());

    std::vector<sqlpp::chrono::microsecond_point> tps {
      static_cast<date::sys_days>(date::January/1/1970) + std::chrono::hours{1} + std::chrono::minutes{20} + std::chrono::seconds{14} + std::chrono::microseconds{1},
      static_cast<date::sys_days>(date::June/13/1986) + std::chrono::hours{12} + std::chrono::minutes{0} + std::chrono::seconds{1} + std::chrono::microseconds{123},
      static_cast<date::sys_days>(date::December/31/2022) + std::chrono::hours{0} + std::chrono::minutes{59} + std::chrono::seconds{59} + std::chrono::microseconds{987654}
    };
    for (const auto &tp : tps) {
      test_time_point(db, tp);
    }
  } catch (const sql::failure& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

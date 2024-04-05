/*
 * Copyright (c) 2013 - 2016, Roland Bock
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

#include "make_test_connection.h"
#include "TabSample.h"
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>

#include <cassert>
#include <iostream>
#include <vector>

const auto library_raii = sqlpp::mysql::scoped_library_initializer_t{};

namespace
{
  template <typename L, typename R>
  auto require_equal(int line, const L& l, const R& r) -> void
  {
    if (l != r)
    {
      std::cerr << line << ": ";
      serialize(::sqlpp::wrap_operand_t<L>{l}, std::cerr);
      std::cerr << " != ";
      serialize(::sqlpp::wrap_operand_t<R>{r}, std::cerr);
      std::cerr << "\n" ;
      throw std::runtime_error("Unexpected result");
    }
  }

  template <typename L, typename R>
  auto require_close(int line, const L& l, const R& r) -> void
  {
    if (date::abs(l - r) > std::chrono::seconds{1})
    {
      std::cerr << line << ": abs(";
      serialize(::sqlpp::wrap_operand_t<L>{l}, std::cerr);
      std::cerr << " - ";
      serialize(::sqlpp::wrap_operand_t<R>{r}, std::cerr);
      std::cerr << ") > 1s\n" ;
      throw std::runtime_error("Unexpected result");
    }
  }
}

namespace sql = sqlpp::mysql;
int DateTime(int, char*[])
{
  sql::global_library_init();
  try
  {
    const auto now = ::sqlpp::chrono::floor<::std::chrono::milliseconds>(std::chrono::system_clock::now());
    const auto today = ::sqlpp::chrono::floor<::sqlpp::chrono::days>(now);
    const auto yesterday = today - ::sqlpp::chrono::days{1};
    const auto current = now - today;

    auto db = sql::make_test_connection();
    db.execute(R"(SET time_zone = '+00:00')"); // To force MySQL's CURRENT_TIMESTAMP into the right timezone
    db.execute(R"(DROP TABLE IF EXISTS tab_date_time)");
    db.execute(R"(CREATE TABLE tab_date_time (
                    col_day_point date,
                    col_time_point datetime(3),
                    col_date_time_point datetime DEFAULT CURRENT_TIMESTAMP,
                    col_time_of_day time(3)
                  ))");

    const auto tab = TabDateTime{};
    db(insert_into(tab).default_values());
    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.colDayPoint.is_null(), true);
      require_equal(__LINE__, row.colDayPoint.value(), ::sqlpp::chrono::day_point{});
      require_equal(__LINE__, row.colTimePoint.is_null(), true);
      require_equal(__LINE__, row.colTimePoint.value(), ::sqlpp::chrono::microsecond_point{});
      require_close(__LINE__, row.colDateTimePoint.value(), now);
      require_equal(__LINE__, row.colTimeOfDay.is_null(), true);
      require_equal(__LINE__, row.colTimeOfDay.value(), ::std::chrono::microseconds{});
    }

    db(update(tab).set(tab.colDayPoint = today, tab.colTimePoint = now, tab.colTimeOfDay = current).unconditionally());

    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.colDayPoint.value(), today);
      require_equal(__LINE__, row.colTimePoint.value(), now);
      require_close(__LINE__, row.colTimeOfDay.value(), current);
    }

    auto statement = db.prepare(select(all_of(tab)).from(tab).unconditionally());
    for (const auto& row : db(statement))
    {
      require_equal(__LINE__, row.colDateTimePoint.is_null(), false);
      require_close(__LINE__, row.colDateTimePoint.value(), now);
      require_equal(__LINE__, row.colDayPoint.value(), today);
      require_equal(__LINE__, row.colTimePoint.value(), now);
      require_close(__LINE__, row.colTimeOfDay.value(), current);
    }

    db(update(tab).set(tab.colDayPoint = yesterday, tab.colTimePoint = today).unconditionally());

    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.colDayPoint.value(), yesterday);
      require_equal(__LINE__, row.colTimePoint.value(), today);
    }

    update(tab)
        .set(tab.colDayPoint = parameter(tab.colDayPoint), tab.colTimePoint = parameter(tab.colTimePoint))
        .unconditionally();

    auto prepared_update = db.prepare(update(tab)
                                          .set(tab.colDayPoint = parameter(tab.colDayPoint),
                                               tab.colTimePoint = parameter(tab.colTimePoint),
                                               tab.colTimeOfDay = parameter(tab.colTimeOfDay))
                                          .unconditionally());
    prepared_update.params.colDayPoint = today;
    prepared_update.params.colTimePoint = now;
    prepared_update.params.colTimeOfDay = current;
    std::cout << "---- running prepared update ----" << std::endl;
    db(prepared_update);
    std::cout << "---- finished prepared update ----" << std::endl;
    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.colDayPoint.value(), today);
      require_equal(__LINE__, row.colTimePoint.value(), now);
      require_equal(__LINE__, row.colTimeOfDay.value(), current);
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "Unkown exception" << std::endl;
  }
  return 0;
}

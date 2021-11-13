/**
 * Copyright © 2017 Volker Aßmann
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

#include <iostream>

#include <sqlpp11/custom_query.h>
#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include "TabSample.h"
#include "make_test_connection.h"

namespace
{
  const auto now = ::sqlpp::chrono::floor<::std::chrono::microseconds>(std::chrono::system_clock::now());
  const auto today = ::sqlpp::chrono::floor<::sqlpp::chrono::days>(now);
  const auto yesterday = today - ::sqlpp::chrono::days{1};

  template <typename L, typename R>
  void require_equal(int line, const L& l, const R& r)
  {
    if (l != r)
    {
      std::cerr << line << ": ";
      serialize(::sqlpp::wrap_operand_t<L>{l}, std::cerr);
      std::cerr << " != ";
      serialize(::sqlpp::wrap_operand_t<R>{r}, std::cerr);
      throw std::runtime_error("Unexpected result");
    }
  }

  template <class Db>
  void prepare_table(Db&& db, bool with_tz)
  {
    db.execute("DROP TABLE IF EXISTS tab_date_time");
    if (with_tz)
    {
      // prepare test with timezone
      db.execute("CREATE TABLE tab_date_time (col_day_point DATE, col_time_point TIMESTAMP WITH TIME ZONE)");
    }
    else
    {
      // prepare  test without timezone
      db.execute("CREATE TABLE tab_date_time (col_day_point DATE, col_time_point TIMESTAMP)");
    }
  }

}  // namespace

int Date(int, char*[])
{

  namespace sql = sqlpp::postgresql;

  sql::connection db = sql::make_test_connection();


  try
  {
    for (int i = 0; i <= 1; ++i)
    {
      if (i)
        std::cerr << "Testing date/time WITH timezone\n";
      else
        std::cerr << "Testing date/time WITHOUT timezone\n";
      prepare_table(db, i);

      const auto tab = TabDateTime{};
      db(insert_into(tab).default_values());
      for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
      {
        require_equal(__LINE__, row.colDayPoint.is_null(), true);
        require_equal(__LINE__, row.colDayPoint.value(), ::sqlpp::chrono::day_point{});
        require_equal(__LINE__, row.colTimePoint.is_null(), true);
        require_equal(__LINE__, row.colTimePoint.value(), ::sqlpp::chrono::microsecond_point{});
      }

      db(update(tab).set(tab.colDayPoint = today, tab.colTimePoint = now).unconditionally());

      for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
      {
        require_equal(__LINE__, row.colDayPoint.value(), today);
        require_equal(__LINE__, row.colTimePoint.value(), now);
      }

      db(update(tab).set(tab.colDayPoint = yesterday, tab.colTimePoint = today).unconditionally());

      for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
      {
        require_equal(__LINE__, row.colDayPoint.value(), yesterday);
        require_equal(__LINE__, row.colTimePoint.value(), today);
      }

      auto prepared_update = db.prepare(
          update(tab)
              .set(tab.colDayPoint = parameter(tab.colDayPoint), tab.colTimePoint = parameter(tab.colTimePoint))
              .unconditionally());
      prepared_update.params.colDayPoint = today;
      prepared_update.params.colTimePoint = now;
      std::cout << "---- running prepared update ----" << std::endl;
      db(prepared_update);
      std::cout << "---- finished prepared update ----" << std::endl;

      for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
      {
        require_equal(__LINE__, row.colDayPoint.value(), today);
        require_equal(__LINE__, row.colTimePoint.value(), now);
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "Unknown exception" << std::endl;
    return 1;
  }
  return 0;
}

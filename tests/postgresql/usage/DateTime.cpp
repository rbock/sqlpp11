/*
 * Copyright (c) 2013 - 2015, Roland Bock
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

#include <cassert>
#include <iostream>
#include <vector>

#include <sqlpp11/postgresql/exception.h>
#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include "TabDateTime.h"
#include "make_test_connection.h"

namespace
{
  const auto now = ::date::floor<::std::chrono::microseconds>(std::chrono::system_clock::now());
  const auto today = ::date::floor<::sqlpp::chrono::days>(now);
  const auto yesterday = today - ::sqlpp::chrono::days{1};
  const auto current = now - today;

  template <typename L, typename R>
  auto require_equal(int line, const L& l, const R& r) -> void
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
}  // namespace

int DateTime(int, char*[])
{
  namespace sql = sqlpp::postgresql;

  sql::connection db = sql::make_test_connection();

  db.execute(R"(DROP TABLE IF EXISTS tabdatetime;)");
  db.execute(R"(CREATE TABLE tabdatetime
               (
                 c_timepoint timestamp with time zone,
                 c_time time with time zone,
                 c_day date
               ))");

  model::TabDateTime tab = {};
  try
  {
    db(insert_into(tab).default_values());
    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.c_day.is_null(), true);
      require_equal(__LINE__, row.c_day.value(), ::sqlpp::chrono::day_point{});
      require_equal(__LINE__, row.c_time.is_null(), true);
      require_equal(__LINE__, row.c_time.value(), ::std::chrono::microseconds{});
      require_equal(__LINE__, row.c_timepoint.is_null(), true);
      require_equal(__LINE__, row.c_timepoint.value(), ::sqlpp::chrono::microsecond_point{});
    }

    db(update(tab).set(tab.c_day = today, tab.c_time = current, tab.c_timepoint = now).unconditionally());

    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.c_day.value(), today);
      require_equal(__LINE__, row.c_time.value(), current);
      require_equal(__LINE__, row.c_timepoint.value(), now);
    }

    db(update(tab).set(tab.c_day = yesterday, tab.c_timepoint = today).unconditionally());

    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.c_day.value(), yesterday);
      require_equal(__LINE__, row.c_time.value(), current);
      require_equal(__LINE__, row.c_timepoint.value(), today);
    }

    auto prepared_update = db.prepare(update(tab)
                                          .set(tab.c_day = parameter(tab.c_day),
                                               tab.c_time = parameter(tab.c_time),
                                               tab.c_timepoint = parameter(tab.c_timepoint))
                                          .unconditionally());
    prepared_update.params.c_day = today;
    prepared_update.params.c_time = current;
    prepared_update.params.c_timepoint = now;
    std::cout << "---- running prepared update ----" << std::endl;
    db(prepared_update);
    std::cout << "---- finished prepared update ----" << std::endl;
    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.c_day.value(), today);
      require_equal(__LINE__, row.c_time.value(), current);
      require_equal(__LINE__, row.c_timepoint.value(), now);
    }
  }
  catch (const sql::failure& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

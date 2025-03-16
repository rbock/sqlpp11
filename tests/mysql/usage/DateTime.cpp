/*
 * Copyright (c) 2013 - 2016, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
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

#include <sqlpp23/mysql/mysql.h>
#include <sqlpp23/sqlpp23.h>
#include "Tables.h"
#include "make_test_connection.h"

#include "sqlpp23/tests/core/result_helpers.h"

#include <cassert>
#include <iostream>
#include <vector>

const auto library_raii = sqlpp::mysql::scoped_library_initializer_t{};

namespace {
template <typename L, typename R>
auto require_close(int line, const L& l, const R& r) -> void {
  if (std::chrono::abs(l - r) > std::chrono::seconds{1}) {
    std::cerr << line << ": abs(";
    std::cerr << sqlpp::to_sql_string(std::cerr, l);
    std::cerr << " - ";
    std::cerr << sqlpp::to_sql_string(std::cerr, r);
    std::cerr << ") > 1s\n";
    throw std::runtime_error("Unexpected result");
  }
}
}  // namespace

namespace sql = sqlpp::mysql;
int DateTime(int, char*[]) {
  sql::global_library_init();
  try {
    const auto now = std::chrono::floor<::std::chrono::milliseconds>(
        std::chrono::system_clock::now());
    const auto today = std::chrono::floor<std::chrono::days>(now);
    const auto yesterday = today - std::chrono::days{1};
    const auto current = now - today;

    auto db = sql::make_test_connection();
    db.execute(
        R"(SET time_zone = '+00:00')");  // To force MySQL's CURRENT_TIMESTAMP
                                         // into the right timezone
    test::createTabDateTime(db);

    const auto tab = test::TabDateTime{};
    db(insert_into(tab).default_values());
    for (const auto& row : db(select(all_of(tab)).from(tab).where(true))) {
      require_equal(__LINE__, row.dayPointN.has_value(), false);
      require_equal(__LINE__, row.timePointN.has_value(), false);
      require_close(__LINE__, row.dateTimePointND.value(), now);
      require_equal(__LINE__, row.timeOfDayN.has_value(), false);
    }

    db(update(tab)
           .set(tab.dayPointN = today, tab.timePointN = now,
                tab.timeOfDayN = current)
           .where(true));

    for (const auto& row : db(select(all_of(tab)).from(tab).where(true))) {
      require_equal(__LINE__, row.dayPointN.value(), today);
      require_equal(__LINE__, row.timePointN.value(), now);
      require_close(__LINE__, row.timeOfDayN.value(), current);
    }

    auto statement = db.prepare(select(all_of(tab)).from(tab).where(true));
    for (const auto& row : db(statement)) {
      require_close(__LINE__, row.dateTimePointND.value(), now);
      require_equal(__LINE__, row.dayPointN.value(), today);
      require_equal(__LINE__, row.timePointN.value(), now);
      require_close(__LINE__, row.timeOfDayN.value(), current);
    }

    db(update(tab)
           .set(tab.dayPointN = yesterday, tab.timePointN = today)
           .where(true));

    for (const auto& row : db(select(all_of(tab)).from(tab).where(true))) {
      require_equal(__LINE__, row.dayPointN.value(), yesterday);
      require_equal(__LINE__, row.timePointN.value(), today);
    }

    auto prepared_update =
        db.prepare(update(tab)
                       .set(tab.dayPointN = parameter(tab.dayPointN),
                            tab.timePointN = parameter(tab.timePointN),
                            tab.timeOfDayN = parameter(tab.timeOfDayN))
                       .where(true));
    prepared_update.params.dayPointN = today;
    prepared_update.params.timePointN = now;
    prepared_update.params.timeOfDayN = current;
    std::cout << "---- running prepared update ----" << std::endl;
    db(prepared_update);
    std::cout << "---- finished prepared update ----" << std::endl;
    for (const auto& row : db(select(all_of(tab)).from(tab).where(true))) {
      require_equal(__LINE__, row.dayPointN.value(), today);
      require_equal(__LINE__, row.timePointN.value(), now);
      require_equal(__LINE__, row.timeOfDayN.value(), current);
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Unkown exception" << std::endl;
  }
  return 0;
}

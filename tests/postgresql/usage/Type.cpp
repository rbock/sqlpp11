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
  template <typename L, typename R>
  void require_equal(int line, const L& l, const R& r)
  {
    if (l != r)
    {
      std::cerr << line << ": --" << l << " != " << r << "--" << std::endl;
      throw std::runtime_error("Unexpected result");
    }
  }

  template <class Db>
  void prepare_table(Db&& db)
  {
    // prepare test with timezone
    db.execute("DROP TABLE IF EXISTS tab_sample");
    db.execute("CREATE TABLE tab_sample (alpha bigint, beta text, gamma bool)");
  }
}

namespace sql = sqlpp::postgresql;

int Type(int, char*[])
{
  sql::connection db = sql::make_test_connection();

  try
  {
    prepare_table(db);

    const auto tab = TabSample{};
    db(insert_into(tab).default_values());
    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.alpha.is_null(), true);
      require_equal(__LINE__, row.alpha.value(), 0);
      require_equal(__LINE__, row.beta.is_null(), true);
      require_equal(__LINE__, row.beta.value(), "");
      require_equal(__LINE__, row.gamma.is_null(), true);
      require_equal(__LINE__, row.gamma.value(), false);
    }

    db(update(tab).set(tab.alpha = 10, tab.beta = "Cookies!", tab.gamma = true).unconditionally());

    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.alpha.is_null(), false);
      require_equal(__LINE__, row.alpha.value(), 10);
      require_equal(__LINE__, row.beta.is_null(), false);
      require_equal(__LINE__, row.beta.value(), "Cookies!");
      require_equal(__LINE__, row.gamma.is_null(), false);
      require_equal(__LINE__, row.gamma.value(), true);
    }

    db(update(tab).set(tab.alpha = 20, tab.beta = "Monster", tab.gamma = false).unconditionally());

    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.alpha.value(), 20);
      require_equal(__LINE__, row.beta.value(), "Monster");
      require_equal(__LINE__, row.gamma.value(), false);
    }

    auto prepared_update = db.prepare(
        update(tab)
            .set(tab.alpha = parameter(tab.alpha), tab.beta = parameter(tab.beta), tab.gamma = parameter(tab.gamma))
            .unconditionally());
    prepared_update.params.alpha = 30;
    prepared_update.params.beta = "IceCream";
    prepared_update.params.gamma = true;
    std::cout << "---- running prepared update ----" << std::endl;
    db(prepared_update);
    std::cout << "---- finished prepared update ----" << std::endl;

    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally()))
    {
      require_equal(__LINE__, row.alpha.value(), 30);
      require_equal(__LINE__, row.beta.value(), "IceCream");
      require_equal(__LINE__, row.gamma.value(), true);
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

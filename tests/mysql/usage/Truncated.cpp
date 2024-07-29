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
#include "Tables.h"
#include <cassert>
#include <sqlpp11/core/alias_provider.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/core/clause/insert.h>
#include <sqlpp11/mysql/database/connection.h>
#include <sqlpp11/core/clause/remove.h>
#include <sqlpp11/core/clause/select.h>
#include <sqlpp11/core/database/transaction.h>
#include <sqlpp11/core/clause/update.h>
#include "../../include/test_helpers.h"

#include <iostream>
#include <vector>

const auto library_raii = sqlpp::mysql::scoped_library_initializer_t{0, nullptr, nullptr};

namespace sql = sqlpp::mysql;
const auto tab = test::TabSample{};

int Truncated(int, char*[])
{
  sql::global_library_init();
  try
  {
    auto db = sql::make_test_connection();
    test::createTabSample(db);

    db(insert_into(tab).set(tab.boolN = true, tab.textN = "cheese"));
    db(insert_into(tab).set(tab.boolN = true, tab.textN = "cheesecake"));

    {
      for (const auto& row : db(db.prepare(sqlpp::select(all_of(tab)).from(tab).unconditionally())))
      {
        std::cerr << ">>> row.intN: " << row.intN << ", row.textN: " << row.textN << ", row.boolN: " << row.boolN << std::endl;
      }
    }

    {
      auto result = db(db.prepare(sqlpp::select(all_of(tab)).from(tab).where(tab.id == 1).limit(1u)));
      auto& row = result.front();

      std::cerr << ">>> row.intN: " << row.intN << ", row.textN: " << row.textN << ", row.boolN: " << row.boolN << std::endl;
      assert(row.textN == "cheese");
    }

    {
      auto result = db(db.prepare(sqlpp::select(all_of(tab)).from(tab).where(tab.id == 2).limit(1u)));
      auto& row = result.front();

      std::cerr << ">>> row.intN: " << row.intN << ", row.textN: " << row.textN << ", row.boolN: " << row.boolN << std::endl;
      assert(row.textN == "cheesecake");
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

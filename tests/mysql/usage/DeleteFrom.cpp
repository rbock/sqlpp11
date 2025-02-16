/*
 * Copyright (c) 2021 - 2021, Roland Bock, ZerQAQ
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
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>

#include <iostream>
#include <vector>

const auto tab = test::TabSample{};

namespace sql = sqlpp::mysql;

int DeleteFrom(int, char*[])
{
  sql::global_library_init();
  try
  {
    auto db = sql::make_test_connection();
    test::createTabSample(db);

    db(insert_into(tab).set(tab.textN = "1", tab.boolN = false));
    db(insert_into(tab).set(tab.textN = "2", tab.boolN = false));
    db(insert_into(tab).set(tab.textN = "3", tab.boolN = false));

    db(sql::delete_from(tab).where(true).order_by(tab.intN.desc()).limit(1u));
    for(const auto &row : db(sqlpp::select(tab.textN).from(tab).where(true).order_by(tab.intN.desc()).limit(1u))){
      if (row.textN != "2")
        throw std::runtime_error("unexpected value for row.textN: " + std::string(row.textN.value_or("NULL")));
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}


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

#include "make_test_connection.h"
#include "TabSample.h"
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/mysql/connection.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/update.h>

#include <cassert>
#include <iostream>
#include <vector>

namespace sql = sqlpp::mysql;
int MoveConstructor(int, char*[])
{
  sql::global_library_init();
  auto config = sql::make_test_config();
  try
  {
    std::vector<sql::connection> connections;
    connections.emplace_back(sql::connection(config));

    connections.at(0).execute(R"(DROP TABLE IF EXISTS tab_sample)");
    connections.at(0).execute(R"(CREATE TABLE tab_sample (
		alpha bigint(20) DEFAULT NULL,
			beta varchar(255) DEFAULT NULL,
			gamma bool DEFAULT NULL
			))");

    assert(connections.at(0).is_transaction_active() == false);
    connections.at(0).start_transaction();
    auto db = std::move(connections.at(0));
    assert(db.is_transaction_active());
    const auto tab = TabSample{};
    db(insert_into(tab).set(tab.gamma = true));
    auto i = insert_into(tab).columns(tab.beta, tab.gamma);
    i.values.add(tab.beta = "rhabarbertorte", tab.gamma = false);
    i.values.add(tab.beta = "cheesecake", tab.gamma = false);
    i.values.add(tab.beta = "kaesekuchen", tab.gamma = true);
    db(i);

    auto s = dynamic_select(db).dynamic_columns(tab.alpha).from(tab).unconditionally();
    s.selected_columns.add(tab.beta);

    for (const auto& row : db(s))
    {
      std::cerr << "row.alpha: " << row.alpha << ", row.beta: " << row.at("beta") << std::endl;
    };
    db.commit_transaction();
    assert(db.is_transaction_active() == false);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

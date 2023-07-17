/*
 * Copyright (c) 2013-2021, Roland Bock
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

#include <iostream>
#include "make_test_connection.h"
#include "TabSample.h"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/custom_query.h>
#include <sqlpp11/mysql/mysql.h>

namespace
{
  struct on_duplicate_key_update
  {
    std::string _serialized;

    template <typename Db, typename Assignment>
    on_duplicate_key_update(Db& db, Assignment assignment)
    {
      typename Db::_serializer_context_t context{db};
      _serialized = " ON DUPLICATE KEY UPDATE " + serialize(assignment, context).str();
    }

    template <typename Db, typename Assignment>
    auto operator()(Db& db, Assignment assignment) -> on_duplicate_key_update&
    {
      typename Db::_serializer_context_t context{db};
      _serialized += ", " + serialize(assignment, context).str();
      return *this;
    }

    auto get() const -> sqlpp::verbatim_t<::sqlpp::no_value_t>
    {
      return ::sqlpp::verbatim(_serialized);
    }
  };
}  // namespace

const auto tab = TabSample{};

namespace sql = sqlpp::mysql;
int CustomQuery(int, char*[])
{
  sql::global_library_init();
  try
  {
    auto db = sql::make_test_connection();
    db.execute(R"(DROP TABLE IF EXISTS tab_sample)");
    db.execute(R"(CREATE TABLE tab_sample (
			alpha bigint(20) AUTO_INCREMENT,
			beta varchar(255) DEFAULT NULL,
			gamma bool DEFAULT NULL,
			PRIMARY KEY (alpha)
			))");
    db.execute(R"(DROP TABLE IF EXISTS tab_foo)");
    db.execute(R"(CREATE TABLE tab_foo (
		omega bigint(20) DEFAULT NULL
			))");

     // Create a MYSQL style custom "insert on duplicate update"
    db(custom_query(sqlpp::insert_into(tab).set(tab.beta = "sample", tab.gamma = true),
                    on_duplicate_key_update(db, tab.beta = "sample")(db, tab.gamma = false).get()));
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

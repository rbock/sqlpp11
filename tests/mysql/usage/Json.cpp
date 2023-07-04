/*
 * Copyright (c) 2019 - 2019, Roland Bock
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

#include <mysql.h>
#include <iostream>

// JSON support only in MYSQL 5.7.8 and later
#if !USE_MARIADB && (LIBMYSQL_VERSION_ID < 50708)
int Json(int, char*[])
{
  std::cerr << "Warning: not testing Json, because the MySQL version id is less than 50708" << std::endl;
  return 0;
}
#else
// JSON support only in MariaDB 10.2.7 and later
#if USE_MARIADB && (MARIADB_VERSION_ID < 100207)
int Json(int, char*[])
{
  std::cerr << "Warning: not testing Json, because the MariaDB version id is less than 100207" << std::endl;
  return 0;
}
#else

#include "TabJson.h"
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>

namespace test
{
  SQLPP_ALIAS_PROVIDER(value)
}

namespace sql = sqlpp::mysql;
int Json(int, char*[])
{
  sql::global_library_init();
  try
  {
    auto db = sql::make_test_connection();
    db.execute(R"(DROP TABLE IF EXISTS tab_json)");
    db.execute(R"(CREATE TABLE tab_json (
			  data JSON NOT NULL
		  ))");

    const auto tab = test::TabJson{};
    db(insert_into(tab).set(tab.data = R"--({"key" : "value"})--"));

    const auto query =
        select(sqlpp::verbatim<sqlpp::text>(R"--(JSON_UNQUOTE(JSON_EXTRACT(data, "$.key")))--").as(test::value))
            .from(tab)
            .unconditionally();

    auto result = db(query);
    if (result.empty())
      throw std::runtime_error{"selection result is empty"};

    const std::string value = result.front().value;

    if (value != "value")
      throw std::runtime_error{std::string{"unexpected value: "} + value};
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
#endif
#endif

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

#include <mysql.h>
#include <iostream>

// JSON support only in MYSQL 5.7.8 and later
#if !USE_MARIADB && (LIBMYSQL_VERSION_ID < 50708)
int main()
{
  std::cerr << "Warning: not testing Json, because the MySQL version id is less than 50708" << std::endl;
}
#else
// JSON support only in MariaDB 10.2.7 and later
#if USE_MARIADB && (MARIADB_VERSION_ID < 100207)
int main()
{
  std::cerr << "Warning: not testing Json, because the MariaDB version id is less than 100207" << std::endl;
}
#else

#include "TabJson.h"
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>

namespace test
{
  SQLPP_ALIAS_PROVIDER(value)
}

namespace mysql = sqlpp::mysql;
int main()
{
  mysql::global_library_init();

  auto config = std::make_shared<mysql::connection_config>();
  config->user = "root";
  config->database = "sqlpp_mysql";
  config->debug = true;
  try
  {
    mysql::connection db(config);
  }
  catch (const sqlpp::exception& e)
  {
    std::cerr << "For testing, you'll need to create a database sqlpp_mysql for user root (no password)" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
  try
  {
    mysql::connection db(config);
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
}
#endif
#endif

#pragma once

/*
 * Copyright (c) 2021, Roland Bock
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

#include <sqlpp11/postgresql/postgresql.h>

namespace sqlpp
{
  namespace postgresql
  {
    // Get configuration for test connection
    inline std::shared_ptr<sqlpp::postgresql::connection_config> make_test_config()
    {
      auto config = std::make_shared<sqlpp::postgresql::connection_config>();

#ifdef WIN32
      config->dbname = "test";
      config->user = "test";
      config->debug = true;
#else
      config->user = getenv("USER");
      config->dbname = "sqlpp_postgresql";
      config->debug = true;
#endif
      return config;
    }

    // Starts a connection and sets the time zone to UTC
    inline ::sqlpp::postgresql::connection make_test_connection(const std::string &tz = "UTC")
    {
      namespace sql = sqlpp::postgresql;

      auto config = make_test_config();

      sql::connection db;
      try
      {
        db.connectUsing(config);
      }
      catch (const sqlpp::exception&)
      {
        std::cerr << "For testing, you'll need to create a database called '" << config->dbname
                  << "', accessible by user '" << config->user << "' without a password." << std::endl;
        throw;
      }

      db.execute("SET TIME ZONE " + tz + ";");

      return db;
    }
  }  // namespace postgresql
}  // namespace sqlpp

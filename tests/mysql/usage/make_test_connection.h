#pragma once

/*
Copyright (c) 2023, Vesselin Atanasov
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


#include <sqlpp11/mysql/mysql.h>

namespace sqlpp
{
  namespace mysql
  {
    // Get configuration for test connection
    inline std::shared_ptr<sqlpp::mysql::connection_config> make_test_config()
    {
      auto config = std::make_shared<sqlpp::mysql::connection_config>();
      config->user = "root";
      config->database = "sqlpp_mysql";
      config->debug = true;
      return config;
    }

    // Starts a connection
    inline ::sqlpp::mysql::connection make_test_connection()
    {
      namespace sql = sqlpp::mysql;

      auto config = make_test_config();
      sql::connection db;
      try
      {
        db.connectUsing(config);
      }
      catch (const sqlpp::exception&)
      {
        std::cerr << "For testing, you'll need to create a database called '" << config->database
                  << "', accessible by user '" << config->user << "' without a password." << std::endl;
        throw;
      }
      return db;
    }
  }  // namespace mysql
}  // namespace sqlpp

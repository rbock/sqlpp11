/*
 * Copyright (c) 2013 - 2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <memory>
#include <sqlpp11/exception.h>
#include <sqlpp11/sqlite3/connection_config.h>
#include "connection_handle.h"

#ifdef SQLPP_DYNAMIC_LOADING
#include <sqlpp11/sqlite3/dynamic_libsqlite3.h>
#endif

namespace sqlpp
{
  namespace sqlite3
  {

#ifdef SQLPP_DYNAMIC_LOADING
  using namespace dynamic;
#endif

    namespace detail
    {
      connection_handle::connection_handle(connection_config conf) : config(conf), sqlite(nullptr)
      {
#ifdef SQLPP_DYNAMIC_LOADING
        init_sqlite("");
#endif
        
        auto rc = sqlite3_open_v2(conf.path_to_database.c_str(), &sqlite, conf.flags,
                                  conf.vfs.empty() ? nullptr : conf.vfs.c_str());
        if (rc != SQLITE_OK)
        {
          const std::string msg = sqlite3_errmsg(sqlite);
          sqlite3_close(sqlite);
          throw sqlpp::exception("Sqlite3 error: Can't open database: " + msg);
        }
#ifdef SQLITE_HAS_CODEC
        if (conf.password.size()>0)
        {
          int ret = sqlite3_key(sqlite, conf.password.data(),conf.password.size());
          if (ret!=SQLITE_OK)
          {
            const std::string msg = sqlite3_errmsg(sqlite);
            sqlite3_close(sqlite);
            throw sqlpp::exception("Sqlite3 error: Can't set password to database: " + msg);
          }
        }
#endif
      }

      connection_handle::~connection_handle()
      {
        auto rc = sqlite3_close(sqlite);
        if (rc != SQLITE_OK)
        {
          std::cerr << "Sqlite3 error: Can't close database: " << sqlite3_errmsg(sqlite) << std::endl;
        }
      }
    }
  }
}

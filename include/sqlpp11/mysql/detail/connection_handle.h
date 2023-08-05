#pragma once

/*
 * Copyright (c) 2013 - 2017, Roland Bock
 * Copyright (c) 2023, Vesselin Atanasov
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

#include <sqlpp11/mysql/connection_config.h>
#include <sqlpp11/mysql/sqlpp_mysql.h>

#include <memory>

namespace sqlpp
{
  namespace mysql
  {
    namespace detail
    {
      inline void connect(MYSQL* mysql, const connection_config& config)
      {
        if (config.connect_timeout_seconds != 0 &&
            mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &config.connect_timeout_seconds))
        {
          throw sqlpp::exception("MySQL: could not set option MYSQL_OPT_CONNECT_TIMEOUT");
        }

        if (!mysql_real_connect(mysql, config.host.empty() ? nullptr : config.host.c_str(),
                                config.user.empty() ? nullptr : config.user.c_str(),
                                config.password.empty() ? nullptr : config.password.c_str(), nullptr, config.port,
                                config.unix_socket.empty() ? nullptr : config.unix_socket.c_str(), config.client_flag))
        {
          throw sqlpp::exception("MySQL: could not connect to server: " + std::string(mysql_error(mysql)));
        }

        if (mysql_set_character_set(mysql, config.charset.c_str()))
        {
          throw sqlpp::exception("MySQL error: can't set character set " + config.charset);
        }

        if (not config.database.empty() and mysql_select_db(mysql, config.database.c_str()))
        {
          throw sqlpp::exception("MySQL error: can't select database '" + config.database + "'");
        }
      }

      struct connection_handle
      {
        std::shared_ptr<const connection_config> config;
        std::unique_ptr<MYSQL, void (*)(MYSQL*)> mysql;

        connection_handle(const std::shared_ptr<const connection_config>& conf) :
          config(conf),
          mysql(mysql_init(nullptr), mysql_close)
        {
          if (not mysql)
          {
            throw sqlpp::exception("MySQL: could not init mysql data structure");
          }

          if (config->auto_reconnect)
          {
            my_bool my_true = true;
            if (mysql_options(native_handle(), MYSQL_OPT_RECONNECT, &my_true))
            {
              throw sqlpp::exception("MySQL: could not set option MYSQL_OPT_RECONNECT");
            }
          }

          connect(native_handle(), *config);
        }

        connection_handle(const connection_handle&) = delete;
        connection_handle(connection_handle&&) = default;
        connection_handle& operator=(const connection_handle&) = delete;
        connection_handle& operator=(connection_handle&&) = default;

        MYSQL* native_handle() const
        {
          return mysql.get();
        }

        bool check_connection() const
        {
          auto nh = native_handle();
          return nh && (mysql_ping(nh) == 0);
        }

        void reconnect()
        {
          connect(native_handle(), *config);
        }
      };
    }  // namespace detail
  }  // namespace mysql
}  // namespace sqlpp

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

#ifndef SQLPP_MYSQL_CONNECTION_CONFIG_H
#define SQLPP_MYSQL_CONNECTION_CONFIG_H

#include <string>

namespace sqlpp
{
  namespace mysql
  {
    struct connection_config
    {
      std::string host = "localhost";
      std::string user;
      std::string password;
      std::string database;
      unsigned int port = 0;
      std::string unix_socket;
      unsigned long client_flag = 0;
      std::string charset = "utf8";
      bool auto_reconnect = true;
      bool debug = false;
      unsigned int connect_timeout_seconds = 0;  // 0 = do not override MySQL library default

      bool operator==(const connection_config& other) const
      {
        return (other.host == host and other.user == user and other.password == password and
                other.database == database and other.charset == charset and other.auto_reconnect == auto_reconnect and
                other.debug == debug and other.connect_timeout_seconds == connect_timeout_seconds);
      }

      bool operator!=(const connection_config& other) const
      {
        return !operator==(other);
      }
    };
  }
}

#endif

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

#ifndef SQLPP_MYSQL_CONNECTION_HANDLE_H
#define SQLPP_MYSQL_CONNECTION_HANDLE_H

#include <memory>
#include "../sqlpp_mysql.h"

namespace sqlpp
{
  namespace mysql
  {
    struct connection_config;

    namespace detail
    {
      void handle_cleanup(MYSQL* mysql);

      struct connection_handle_t
      {
        const std::shared_ptr<connection_config> config;
        std::unique_ptr<MYSQL, void (*)(MYSQL*)> mysql;

        connection_handle_t(const std::shared_ptr<connection_config>& config);
        ~connection_handle_t();
        connection_handle_t(const connection_handle_t&) = delete;
        connection_handle_t(connection_handle_t&&) = delete;
        connection_handle_t& operator=(const connection_handle_t&) = delete;
        connection_handle_t& operator=(connection_handle_t&&) = delete;

        bool is_valid();
        void reconnect();
      };
    }
  }
}

#endif

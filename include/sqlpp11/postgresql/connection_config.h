#pragma once

/**
 * Copyright © 2014-2015, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/postgresql/visibility.h>
#include <string>

namespace sqlpp
{
  namespace postgresql
  {
    struct DLL_PUBLIC connection_config
    {
      enum class sslmode_t
      {
        disable,
        allow,
        prefer,
        require,
        verify_ca,
        verify_full
      };
      std::string host;
      std::string hostaddr;
      uint32_t port{5432};
      std::string dbname;
      std::string user;
      std::string password;
      uint32_t connect_timeout{0};
      std::string client_encoding;
      std::string options;
      std::string application_name;
      std::string fallback_application_name;
      bool keepalives{true};
      uint32_t keepalives_idle{0};
      uint32_t keepalives_interval{0};
      uint32_t keepalives_count{0};
      sslmode_t sslmode{sslmode_t::prefer};
      bool sslcompression{true};
      std::string sslcert;
      std::string sslkey;
      std::string sslrootcert;
      std::string sslcrl;
      std::string requirepeer;
      std::string krbsrvname;
      std::string service;
      // bool auto_reconnect {true};
      bool debug{false};

      bool operator==(const connection_config& other)
      {
        return (other.host == host && other.hostaddr == hostaddr && other.port == port && other.dbname == dbname &&
                other.user == user && other.password == password && other.connect_timeout == connect_timeout &&
                other.client_encoding == client_encoding && other.options == options &&
                other.application_name == application_name && other.keepalives == keepalives &&
                other.keepalives_idle == keepalives_idle && other.keepalives_interval == keepalives_interval &&
                other.keepalives_count == keepalives_count && other.sslmode == sslmode &&
                other.sslcompression == sslcompression && other.sslcert == sslcert && other.sslkey == sslkey &&
                other.sslrootcert == sslrootcert && other.sslcrl == sslcrl && other.requirepeer == requirepeer &&
                other.krbsrvname == krbsrvname && other.service == service && other.debug == debug);
      }
      bool operator!=(const connection_config& other)
      {
        return !operator==(other);
      }
    };
  }
}

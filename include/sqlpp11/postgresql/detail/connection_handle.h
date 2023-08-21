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

#include <memory>
#include <set>
#include <string>

#include <libpq-fe.h>
#include <sqlpp11/postgresql/connection_config.h>
#include <sqlpp11/postgresql/visibility.h>

#ifdef SQLPP_DYNAMIC_LOADING
#include <sqlpp11/postgresql/dynamic_libpq.h>
#endif

namespace sqlpp
{
  namespace postgresql
  {
    // Forward declaration
    struct connection_config;

#ifdef SQLPP_DYNAMIC_LOADING
    using namespace dynamic;
#endif

    namespace detail
    {
      struct DLL_LOCAL connection_handle
      {
        std::shared_ptr<const connection_config> config;
        std::unique_ptr<PGconn, void(*)(PGconn*)>  postgres;
        std::set<std::string> prepared_statement_names;

        connection_handle(const std::shared_ptr<const connection_config>& conf)
            : config{conf}, postgres{nullptr, PQfinish}
        {
#ifdef SQLPP_DYNAMIC_LOADING
          init_pg("");
#endif
          if (config->debug)
          {
            std::cerr << "PostgreSQL debug: connecting to the database server." << std::endl;
          }

          // Open connection
          std::string conninfo = "";
          if (!config->host.empty())
          {
            conninfo.append("host=" + config->host);
          }
          if (!config->hostaddr.empty())
          {
            conninfo.append(" hostaddr=" + config->hostaddr);
          }
          if (config->port != 5432)
          {
            conninfo.append(" port=" + std::to_string(config->port));
          }
          if (!config->dbname.empty())
          {
            conninfo.append(" dbname=" + config->dbname);
          }
          if (!config->user.empty())
          {
            conninfo.append(" user=" + config->user);
          }
          if (!config->password.empty())
          {
            conninfo.append(" password=" + config->password);
          }
          if (config->connect_timeout != 0)
          {
            conninfo.append(" connect_timeout=" + std::to_string(config->connect_timeout));
          }
          if (!config->client_encoding.empty())
          {
            conninfo.append(" client_encoding=" + config->client_encoding);
          }
          if (!config->options.empty())
          {
            conninfo.append(" options=" + config->options);
          }
          if (!config->application_name.empty())
          {
            conninfo.append(" application_name=" + config->application_name);
          }
          if (!config->fallback_application_name.empty())
          {
            conninfo.append(" fallback_application_name=" + config->fallback_application_name);
          }
          if (!config->keepalives)
          {
            conninfo.append(" keepalives=0");
          }
          if (config->keepalives_idle != 0)
          {
            conninfo.append(" keepalives_idle=" + std::to_string(config->keepalives_idle));
          }
          if (config->keepalives_interval != 0)
          {
            conninfo.append(" keepalives_interval=" + std::to_string(config->keepalives_interval));
          }
          if (config->keepalives_count != 0)
          {
            conninfo.append(" keepalives_count=" + std::to_string(config->keepalives_count));
          }
          switch (config->sslmode)
          {
            case connection_config::sslmode_t::disable:
              conninfo.append(" sslmode=disable");
              break;
            case connection_config::sslmode_t::allow:
              conninfo.append(" sslmode=allow");
              break;
            case connection_config::sslmode_t::require:
              conninfo.append(" sslmode=require");
              break;
            case connection_config::sslmode_t::verify_ca:
              conninfo.append(" sslmode=verify-ca");
              break;
            case connection_config::sslmode_t::verify_full:
              conninfo.append(" sslmode=verify-full");
              break;
            case connection_config::sslmode_t::prefer:
              break;
          }
          if (!config->sslcompression)
          {
            conninfo.append(" sslcompression=0");
          }
          if (!config->sslcert.empty())
          {
            conninfo.append(" sslcert=" + config->sslcert);
          }
          if (!config->sslkey.empty())
          {
            conninfo.append(" sslkey=" + config->sslkey);
          }
          if (!config->sslrootcert.empty())
          {
            conninfo.append(" sslrootcert=" + config->sslrootcert);
          }
          if (!config->requirepeer.empty())
          {
            conninfo.append(" requirepeer=" + config->requirepeer);
          }
          if (!config->krbsrvname.empty())
          {
            conninfo.append(" krbsrvname=" + config->krbsrvname);
          }
          if (!config->service.empty())
          {
            conninfo.append(" service=" + config->service);
          }

          postgres.reset(PQconnectdb(conninfo.c_str()));

          if (!postgres)
            throw std::bad_alloc{};

          if (is_connected() == false)
          {
            std::string msg{PQerrorMessage(native_handle())};
            throw broken_connection{std::move(msg)};
          }
        }

        connection_handle(const connection_handle&) = delete;
        connection_handle(connection_handle&&) = default;

        ~connection_handle()
        {
          // Debug
          if (config->debug)
          {
            std::cerr << "PostgreSQL debug: closing database connection." << std::endl;
          }
        }

        connection_handle& operator=(const connection_handle&) = delete;
        connection_handle& operator=(connection_handle&&) = default;

        void deallocate_prepared_statement(const std::string& name)
        {
          std::string cmd = "DEALLOCATE \"" + name + "\"";
          PGresult* result = PQexec(native_handle(), cmd.c_str());
          PQclear(result);
          prepared_statement_names.erase(name);
        }

        PGconn* native_handle() const
        {
          return postgres.get();
        }

        bool is_connected() const
        {
          auto nh = native_handle();
          return nh && (PQstatus(nh) == CONNECTION_OK);
        }

        bool ping_server() const
        {
          // Loosely based on the implementation of PHP's pg_ping()
          if (is_connected() == false)
          {
            return false;
          }
          auto exec_res = PQexec(native_handle(), "SELECT 1");
          auto exec_ok = PQresultStatus(exec_res) == PGRES_TUPLES_OK;
          PQclear(exec_res);
          return exec_ok;
        }
      };
    }
  }
}

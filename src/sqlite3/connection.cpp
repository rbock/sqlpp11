/*
 * Copyright (c) 2013 - 2016, Roland Bock
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

#include "detail/connection_handle.h"
#include "detail/prepared_statement_handle.h"
#include <iostream>
#include <sqlpp11/exception.h>
#include <sqlpp11/sqlite3/connection.h>

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

    namespace
    {
      detail::prepared_statement_handle_t prepare_statement(detail::connection_handle& handle,
                                                            const std::string& statement)
      {
        if (handle.config.debug)
          std::cerr << "Sqlite3 debug: Preparing: '" << statement << "'" << std::endl;

        detail::prepared_statement_handle_t result(nullptr, handle.config.debug);

        auto rc = sqlite3_prepare_v2(handle.sqlite, statement.c_str(), static_cast<int>(statement.size()),
                                     &result.sqlite_statement, nullptr);

        if (rc != SQLITE_OK)
        {
          throw sqlpp::exception("Sqlite3 error: Could not prepare statement: " +
                                 std::string(sqlite3_errmsg(handle.sqlite)) + " (statement was >>" +
                                 (rc == SQLITE_TOOBIG ? statement.substr(0, 128) + "..." : statement) +
                                 "<<\n");
        }

        return result;
      }

      void execute_statement(detail::connection_handle& handle, detail::prepared_statement_handle_t& prepared)
      {
        auto rc = sqlite3_step(prepared.sqlite_statement);
        switch (rc)
        {
          case SQLITE_OK:
          case SQLITE_ROW:  // might occur if execute is called with a select
          case SQLITE_DONE:
            return;
          default:
            if(handle.config.debug)
              std::cerr << "Sqlite3 debug: sqlite3_step return code: " << rc << std::endl;
            throw sqlpp::exception("Sqlite3 error: Could not execute statement: " +
                                   std::string(sqlite3_errmsg(handle.sqlite)));
        }
      }
    }

    connection::connection(connection_config config) : _handle(new detail::connection_handle(std::move(config)))
    {
    }

    connection::connection(connection&&) noexcept = default;
    connection& connection::operator=(connection&&) noexcept = default;

    connection::~connection()
    {
    }

    ::sqlite3* connection::native_handle()
    {
      return _handle->sqlite;
    }

    bind_result_t connection::select_impl(const std::string& statement)
    {
      std::unique_ptr<detail::prepared_statement_handle_t> prepared(
          new detail::prepared_statement_handle_t(prepare_statement(*_handle, statement)));
      if (!prepared)
      {
        throw sqlpp::exception("Sqlite3 error: Could not store result set");
      }

      return {std::move(prepared)};
    }

    bind_result_t connection::run_prepared_select_impl(prepared_statement_t& prepared_statement)
    {
      return {prepared_statement._handle};
    }

    size_t connection::insert_impl(const std::string& statement)
    {
      auto prepared = prepare_statement(*_handle, statement);
      execute_statement(*_handle, prepared);

      return sqlite3_last_insert_rowid(_handle->sqlite);
    }

    prepared_statement_t connection::prepare_impl(const std::string& statement)
    {
      return {std::unique_ptr<detail::prepared_statement_handle_t>(
          new detail::prepared_statement_handle_t(prepare_statement(*_handle, statement)))};
    }

    size_t connection::run_prepared_insert_impl(prepared_statement_t& prepared_statement)
    {
      execute_statement(*_handle, *prepared_statement._handle.get());

      return sqlite3_last_insert_rowid(_handle->sqlite);
    }

    size_t connection::run_prepared_execute_impl(prepared_statement_t& prepared_statement)
    {
      execute_statement(*_handle, *prepared_statement._handle.get());

      return sqlite3_changes(_handle->sqlite);
    }

    size_t connection::execute(const std::string& statement)
    {
      auto prepared = prepare_statement(*_handle, statement);
      execute_statement(*_handle, prepared);
      return sqlite3_changes(_handle->sqlite);
    }

    size_t connection::update_impl(const std::string& statement)
    {
      auto prepared = prepare_statement(*_handle, statement);
      execute_statement(*_handle, prepared);
      return sqlite3_changes(_handle->sqlite);
    }

    size_t connection::run_prepared_update_impl(prepared_statement_t& prepared_statement)
    {
      execute_statement(*_handle, *prepared_statement._handle.get());

      return sqlite3_changes(_handle->sqlite);
    }

    size_t connection::remove_impl(const std::string& statement)
    {
      auto prepared = prepare_statement(*_handle, statement);
      execute_statement(*_handle, prepared);
      return sqlite3_changes(_handle->sqlite);
    }

    size_t connection::run_prepared_remove_impl(prepared_statement_t& prepared_statement)
    {
      execute_statement(*_handle, *prepared_statement._handle.get());

      return sqlite3_changes(_handle->sqlite);
    }

    std::string connection::escape(const std::string& s) const
    {
      std::string t;
      t.reserve(s.size());

      for (const char c : s)
      {
        if (c == '\'')
          t.push_back(c);
        t.push_back(c);
      }

      return t;
    }

    void connection::set_default_isolation_level(isolation_level level)
    {
      if (level == sqlpp::isolation_level::read_uncommitted)
      {
        execute("pragma read_uncommitted = true");
      } else {
        execute("pragma read_uncommitted = false");
      }
    }

    sqlpp::isolation_level connection::get_default_isolation_level()
    {
      auto stmt = prepare_statement(*_handle, "pragma read_uncommitted");
      execute_statement(*_handle, stmt);

      int level = sqlite3_column_int(stmt.sqlite_statement, 0);

      return level == 0 ? sqlpp::isolation_level::serializable :
                          sqlpp::isolation_level::read_uncommitted;
    }

    void connection::start_transaction()
    {
      if (_transaction_status == transaction_status_type::active)
      {
        throw sqlpp::exception("Sqlite3 error: Cannot have more than one open transaction per connection");
      }

      _transaction_status = transaction_status_type::maybe;
      auto prepared = prepare_statement(*_handle, "BEGIN");
      execute_statement(*_handle, prepared);
      _transaction_status = transaction_status_type::active;
    }

    void connection::commit_transaction()
    {
      if (_transaction_status == transaction_status_type::none)
      {
        throw sqlpp::exception("Sqlite3 error: Cannot commit a finished or failed transaction");
      }
      _transaction_status = transaction_status_type::maybe;
      auto prepared = prepare_statement(*_handle, "COMMIT");
      execute_statement(*_handle, prepared);
      _transaction_status = transaction_status_type::none;
    }

    void connection::rollback_transaction(bool report)
    {
      if (_transaction_status == transaction_status_type::none)
      {
        throw sqlpp::exception("Sqlite3 error: Cannot rollback a finished or failed transaction");
      }
      if (report)
      {
        std::cerr << "Sqlite3 warning: Rolling back unfinished transaction" << std::endl;
      }
      _transaction_status = transaction_status_type::maybe;
      auto prepared = prepare_statement(*_handle, "ROLLBACK");
      execute_statement(*_handle, prepared);
      _transaction_status = transaction_status_type::none;
    }

    void connection::report_rollback_failure(const std::string message) noexcept
    {
      std::cerr << "Sqlite3 message:" << message << std::endl;
    }

    uint64_t connection::last_insert_id() noexcept
    {
      return sqlite3_last_insert_rowid(_handle->sqlite);
    }

    auto connection::attach(const connection_config& config, const std::string name) -> schema_t
    {
      auto prepared =
          prepare_statement(*_handle, "ATTACH '" + escape(config.path_to_database) + "' AS " + escape(name));
      execute_statement(*_handle, prepared);

      return {name};
    }
  }
}

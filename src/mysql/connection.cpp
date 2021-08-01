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

#include <ciso646>
#include <iostream>
#include "detail/connection_handle.h"
#include "detail/prepared_statement_handle.h"
#include "detail/result_handle.h"
#include <sqlpp11/exception.h>
#include <sqlpp11/mysql/connection.h>

namespace sqlpp
{
  namespace mysql
  {
    scoped_library_initializer_t::scoped_library_initializer_t(int argc, char** argv, char** groups)
    {
      mysql_library_init(argc, argv, groups);
    }

    scoped_library_initializer_t::~scoped_library_initializer_t()
    {
      mysql_library_end();
    }

    void global_library_init(int argc, char** argv, char** groups)
    {
      static const auto global_init_and_end = scoped_library_initializer_t(argc, argv, groups);
    }

    namespace
    {
      struct MySqlThreadInitializer
      {
        MySqlThreadInitializer()
        {
          if (!mysql_thread_safe())
          {
            throw sqlpp::exception("MySQL error: Operating on a non-threadsafe client");
          }
          mysql_thread_init();
        }

        ~MySqlThreadInitializer()
        {
          mysql_thread_end();
        }
      };

      void thread_init()
      {
        thread_local MySqlThreadInitializer threadInitializer;
      }

      void execute_statement(detail::connection_handle_t& handle, const std::string& statement)
      {
        thread_init();

        if (handle.config->debug)
          std::cerr << "MySQL debug: Executing: '" << statement << "'" << std::endl;

        if (mysql_query(handle.mysql.get(), statement.c_str()))
        {
          throw sqlpp::exception("MySQL error: Could not execute MySQL-statement: " +
                                 std::string(mysql_error(handle.mysql.get())) + " (statement was >>" + statement +
                                 "<<\n");
        }
      }

      void execute_prepared_statement(detail::prepared_statement_handle_t& prepared_statement)
      {
        thread_init();

        if (prepared_statement.debug)
          std::cerr << "MySQL debug: Executing prepared_statement" << std::endl;

        if (mysql_stmt_bind_param(prepared_statement.mysql_stmt, prepared_statement.stmt_params.data()))
        {
          throw sqlpp::exception(std::string("MySQL error: Could not bind parameters to statement") +
                                 mysql_stmt_error(prepared_statement.mysql_stmt));
        }

        if (mysql_stmt_execute(prepared_statement.mysql_stmt))
        {
          throw sqlpp::exception(std::string("MySQL error: Could not execute prepared statement: ") +
                                 mysql_stmt_error(prepared_statement.mysql_stmt));
        }
      }

      std::shared_ptr<detail::prepared_statement_handle_t> prepare_statement(detail::connection_handle_t& handle,
                                                                             const std::string& statement,
                                                                             size_t no_of_parameters,
                                                                             size_t no_of_columns)
      {
        thread_init();

        if (handle.config->debug)
          std::cerr << "MySQL debug: Preparing: '" << statement << "'" << std::endl;

        auto prepared_statement = std::make_shared<detail::prepared_statement_handle_t>(
            mysql_stmt_init(handle.mysql.get()), no_of_parameters, no_of_columns, handle.config->debug);
        if (not prepared_statement)
        {
          throw sqlpp::exception("MySQL error: Could not allocate prepared statement\n");
        }
        if (mysql_stmt_prepare(prepared_statement->mysql_stmt, statement.data(), statement.size()))
        {
          throw sqlpp::exception("MySQL error: Could not prepare statement: " +
                                 std::string(mysql_error(handle.mysql.get())) + " (statement was >>" + statement +
                                 "<<\n");
        }

        return prepared_statement;
      }
    }

    connection::connection(const std::shared_ptr<connection_config>& config)
        : _handle(new detail::connection_handle_t(config))
    {
    }

    connection::~connection()
    {
    }

    connection::connection(connection&& other)
    {
      this->_transaction_active = other._transaction_active;
      this->_handle = std::move(other._handle);
    }

    bool connection::is_valid()
    {
      return _handle->is_valid();
    }
    void connection::reconnect()
    {
      return _handle->reconnect();
    }

    const std::shared_ptr<connection_config> connection::get_config()
    {
      return _handle->config;
    }

    char_result_t connection::select_impl(const std::string& statement)
    {
      execute_statement(*_handle, statement);
      std::unique_ptr<detail::result_handle> result_handle(
          new detail::result_handle(mysql_store_result(_handle->mysql.get()), _handle->config->debug));
      if (!*result_handle)
      {
        throw sqlpp::exception("MySQL error: Could not store result set: " +
                               std::string(mysql_error(_handle->mysql.get())));
      }

      return {std::move(result_handle)};
    }

    bind_result_t connection::run_prepared_select_impl(prepared_statement_t& prepared_statement)
    {
      execute_prepared_statement(*prepared_statement._handle);
      return prepared_statement._handle;
    }

    size_t connection::run_prepared_insert_impl(prepared_statement_t& prepared_statement)
    {
      execute_prepared_statement(*prepared_statement._handle);
      return mysql_stmt_insert_id(prepared_statement._handle->mysql_stmt);
    }

    size_t connection::run_prepared_update_impl(prepared_statement_t& prepared_statement)
    {
      execute_prepared_statement(*prepared_statement._handle);
      return mysql_stmt_affected_rows(prepared_statement._handle->mysql_stmt);
    }

    size_t connection::run_prepared_remove_impl(prepared_statement_t& prepared_statement)
    {
      execute_prepared_statement(*prepared_statement._handle);
      return mysql_stmt_affected_rows(prepared_statement._handle->mysql_stmt);
    }

    prepared_statement_t connection::prepare_impl(const std::string& statement,
                                                  size_t no_of_parameters,
                                                  size_t no_of_columns)
    {
      return prepare_statement(*_handle, statement, no_of_parameters, no_of_columns);
    }

    size_t connection::insert_impl(const std::string& statement)
    {
      execute_statement(*_handle, statement);

      return mysql_insert_id(_handle->mysql.get());
    }

    void connection::execute(const std::string& command)
    {
      execute_statement(*_handle, command);
    }

    size_t connection::update_impl(const std::string& statement)
    {
      execute_statement(*_handle, statement);
      return mysql_affected_rows(_handle->mysql.get());
    }

    size_t connection::remove_impl(const std::string& statement)
    {
      execute_statement(*_handle, statement);
      return mysql_affected_rows(_handle->mysql.get());
    }

    std::string connection::escape(const std::string& s) const
    {
      std::unique_ptr<char[]> dest(new char[s.size() * 2 + 1]);
      mysql_real_escape_string(_handle->mysql.get(), dest.get(), s.c_str(), s.size());
      return dest.get();
    }

    void connection::start_transaction()
    {
      if (_transaction_active)
      {
        throw sqlpp::exception("MySQL: Cannot have more than one open transaction per connection");
      }
      execute_statement(*_handle, "START TRANSACTION");
      _transaction_active = true;
    }

    void connection::commit_transaction()
    {
      if (not _transaction_active)
      {
        throw sqlpp::exception("MySQL: Cannot commit a finished or failed transaction");
      }
      _transaction_active = false;
      execute_statement(*_handle, "COMMIT");
    }

    void connection::rollback_transaction(bool report)
    {
      if (not _transaction_active)
      {
        throw sqlpp::exception("MySQL: Cannot rollback a finished or failed transaction");
      }
      if (report)
      {
        std::cerr << "MySQL warning: Rolling back unfinished transaction" << std::endl;
      }
      _transaction_active = false;
      execute_statement(*_handle, "ROLLBACK");
    }

    void connection::report_rollback_failure(const std::string message) noexcept
    {
      std::cerr << "MySQL message:" << message << std::endl;
    }

    MYSQL* connection::get_handle(){
      return _handle->mysql.get();
    }
  }
}

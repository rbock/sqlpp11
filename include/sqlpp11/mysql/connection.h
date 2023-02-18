/*
 * Copyright (c) 2013 - 2017, Roland Bock
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

#ifndef SQLPP_MYSQL_CONNECTION_H
#define SQLPP_MYSQL_CONNECTION_H

#include <sqlpp11/connection.h>
#include <sqlpp11/detail/float_safe_ostringstream.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/mysql/bind_result.h>
#include <sqlpp11/mysql/char_result.h>
#include <sqlpp11/mysql/connection_config.h>
#include <sqlpp11/mysql/prepared_statement.h>
#include <sqlpp11/mysql/remove.h>
#include <sqlpp11/mysql/update.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/mysql/sqlpp_mysql.h>
#include <iostream>
#include <sstream>
#include <string>

namespace sqlpp
{
  namespace mysql
  {
    namespace detail
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

      inline void thread_init()
      {
        thread_local MySqlThreadInitializer threadInitializer;
      }

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

      inline void handle_cleanup(MYSQL* mysql)
      {
        mysql_close(mysql);
      }

      struct connection_handle_t
      {
        std::shared_ptr<connection_config> config;
        std::unique_ptr<MYSQL, void (*)(MYSQL*)> mysql;

        connection_handle_t(const std::shared_ptr<connection_config>& conf)
            : config(conf), mysql(mysql_init(nullptr), handle_cleanup)
        {
          if (not mysql)
          {
            throw sqlpp::exception("MySQL: could not init mysql data structure");
          }

          if (config->auto_reconnect)
          {
            my_bool my_true = true;
            if (mysql_options(mysql.get(), MYSQL_OPT_RECONNECT, &my_true))
            {
              throw sqlpp::exception("MySQL: could not set option MYSQL_OPT_RECONNECT");
            }
          }

          connect(mysql.get(), *config);
        }

        ~connection_handle_t() noexcept = default;
        connection_handle_t(const connection_handle_t&) = delete;
        connection_handle_t(connection_handle_t&&) = default;
        connection_handle_t& operator=(const connection_handle_t&) = delete;
        connection_handle_t& operator=(connection_handle_t&&) = default;

        bool is_valid()
        {
          return mysql_ping(mysql.get()) == 0;
        }

        void reconnect()
        {
          connect(mysql.get(), *config);
        }
      };

      inline void execute_statement(detail::connection_handle_t& handle, const std::string& statement)
      {
        thread_init();

        if (handle.config->debug)
          std::cerr << "MySQL debug: Executing: '" << statement << "'" << std::endl;

        if (mysql_query(handle.mysql.get(), statement.c_str()))
        {
          throw sqlpp::exception(
              "MySQL error: Could not execute MySQL-statement: " + std::string(mysql_error(handle.mysql.get())) +
              " (statement was >>" + statement + "<<\n");
        }
      }

      inline void execute_prepared_statement(detail::prepared_statement_handle_t& prepared_statement)
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

      inline std::shared_ptr<detail::prepared_statement_handle_t> prepare_statement(detail::connection_handle_t& handle,
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
          throw sqlpp::exception(
              "MySQL error: Could not prepare statement: " + std::string(mysql_error(handle.mysql.get())) +
              " (statement was >>" + statement + "<<\n");
        }

        return prepared_statement;
      }

    }  // namespace detail

    struct scoped_library_initializer_t
    {
      scoped_library_initializer_t(int argc = 0, char** argv = nullptr, char** groups = nullptr)
      {
        mysql_library_init(argc, argv, groups);
      }

      ~scoped_library_initializer_t()
      {
        mysql_library_end();
      }
    };

    // This will also cleanup when the program shuts down
    inline void global_library_init(int argc = 0, char** argv = nullptr, char** groups = nullptr)
    {
      static const auto global_init_and_end = scoped_library_initializer_t(argc, argv, groups);
    }

    class connection;

    struct serializer_t
    {
      serializer_t(const connection& db) : _db(db)
      {
      }

      template <typename T>
      std::ostream& operator<<(T t)
      {
        return _os << t;
      }

      std::string escape(std::string arg);

      std::string str() const
      {
        return _os.str();
      }

      const connection& _db;
      sqlpp::detail::float_safe_ostringstream _os;
    };

    std::integral_constant<char, '`'> get_quote_left(const serializer_t&);

    std::integral_constant<char, '`'> get_quote_right(const serializer_t&);

    class connection : public sqlpp::connection
    {
      detail::connection_handle_t _handle;
      bool _transaction_active = false;

      // direct execution
      char_result_t select_impl(const std::string& statement)
      {
        execute_statement(_handle, statement);
        std::unique_ptr<detail::result_handle> result_handle(
            new detail::result_handle(mysql_store_result(_handle.mysql.get()), _handle.config->debug));
        if (!*result_handle)
        {
          throw sqlpp::exception("MySQL error: Could not store result set: " +
                                 std::string(mysql_error(_handle.mysql.get())));
        }

        return {std::move(result_handle)};
      }

      size_t insert_impl(const std::string& statement)
      {
        execute_statement(_handle, statement);

        return mysql_insert_id(_handle.mysql.get());
      }

      size_t update_impl(const std::string& statement)
      {
        execute_statement(_handle, statement);
        return mysql_affected_rows(_handle.mysql.get());
      }

      size_t remove_impl(const std::string& statement)
      {
        execute_statement(_handle, statement);
        return mysql_affected_rows(_handle.mysql.get());
      }

      // prepared execution
      prepared_statement_t prepare_impl(const std::string& statement, size_t no_of_parameters, size_t no_of_columns)
      {
        return prepare_statement(_handle, statement, no_of_parameters, no_of_columns);
      }

      bind_result_t run_prepared_select_impl(prepared_statement_t& prepared_statement)
      {
        execute_prepared_statement(*prepared_statement._handle);
        return prepared_statement._handle;
      }

      size_t run_prepared_insert_impl(prepared_statement_t& prepared_statement)
      {
        execute_prepared_statement(*prepared_statement._handle);
        return mysql_stmt_insert_id(prepared_statement._handle->mysql_stmt);
      }

      size_t run_prepared_update_impl(prepared_statement_t& prepared_statement)
      {
        execute_prepared_statement(*prepared_statement._handle);
        return mysql_stmt_affected_rows(prepared_statement._handle->mysql_stmt);
      }

      size_t run_prepared_remove_impl(prepared_statement_t& prepared_statement)
      {
        execute_prepared_statement(*prepared_statement._handle);
        return mysql_stmt_affected_rows(prepared_statement._handle->mysql_stmt);
      }

    public:
      using _prepared_statement_t = ::sqlpp::mysql::prepared_statement_t;
      using _context_t = serializer_t;
      using _serializer_context_t = _context_t;
      using _interpreter_context_t = _context_t;

      struct _tags
      {
        using _null_result_is_trivial_value = std::true_type;
      };

      template <typename T>
      static _context_t& _serialize_interpretable(const T& t, _context_t& context)
      {
        return serialize(t, context);
      }

      template <typename T>
      static _context_t& _interpret_interpretable(const T& t, _context_t& context)
      {
        return serialize(t, context);
      }

      connection(const std::shared_ptr<connection_config>& config) : _handle{config}
      {
      }

      ~connection() = default;

      connection(const connection&) = delete;
      connection& operator=(const connection&) = delete;
      connection& operator=(connection&&) = default;
      connection(connection&& other) = default;

      bool is_valid()
      {
        return _handle.is_valid();
      }

      void reconnect()
      {
        return _handle.reconnect();
      }

      const std::shared_ptr<connection_config>& get_config()
      {
        return _handle.config;
      }

      bool is_transaction_active()
      {
        return _transaction_active;
      }

      template <typename Select>
      char_result_t select(const Select& s)
      {
        _context_t context(*this);
        serialize(s, context);
        return select_impl(context.str());
      }

      template <typename Select>
      _prepared_statement_t prepare_select(Select& s)
      {
        _context_t context(*this);
        serialize(s, context);
        return prepare_impl(context.str(), s._get_no_of_parameters(), s.get_no_of_result_columns());
      }

      template <typename PreparedSelect>
      bind_result_t run_prepared_select(const PreparedSelect& s)
      {
        s._bind_params();
        return run_prepared_select_impl(s._prepared_statement);
      }

      //! insert returns the last auto_incremented id (or zero, if there is none)
      template <typename Insert>
      size_t insert(const Insert& i)
      {
        _context_t context(*this);
        serialize(i, context);
        return insert_impl(context.str());
      }

      template <typename Insert>
      _prepared_statement_t prepare_insert(Insert& i)
      {
        _context_t context(*this);
        serialize(i, context);
        return prepare_impl(context.str(), i._get_no_of_parameters(), 0);
      }

      template <typename PreparedInsert>
      size_t run_prepared_insert(const PreparedInsert& i)
      {
        i._bind_params();
        return run_prepared_insert_impl(i._prepared_statement);
      }

      //! update returns the number of affected rows
      template <typename Update>
      size_t update(const Update& u)
      {
        _context_t context(*this);
        serialize(u, context);
        return update_impl(context.str());
      }

      template <typename Update>
      _prepared_statement_t prepare_update(Update& u)
      {
        _context_t context(*this);
        serialize(u, context);
        return prepare_impl(context.str(), u._get_no_of_parameters(), 0);
      }

      template <typename PreparedUpdate>
      size_t run_prepared_update(const PreparedUpdate& u)
      {
        u._bind_params();
        return run_prepared_update_impl(u._prepared_statement);
      }

      //! remove returns the number of removed rows
      template <typename Remove>
      size_t remove(const Remove& r)
      {
        _context_t context(*this);
        serialize(r, context);
        return remove_impl(context.str());
      }

      template <typename Remove>
      _prepared_statement_t prepare_remove(Remove& r)
      {
        _context_t context(*this);
        serialize(r, context);
        return prepare_impl(context.str(), r._get_no_of_parameters(), 0);
      }

      template <typename PreparedRemove>
      size_t run_prepared_remove(const PreparedRemove& r)
      {
        r._bind_params();
        return run_prepared_remove_impl(r._prepared_statement);
      }

      //! execute arbitrary command (e.g. create a table)
      void execute(const std::string& command)
      {
        execute_statement(_handle, command);
      }

      //! escape given string (does not quote, though)
      std::string escape(const std::string& s) const
      {
        std::unique_ptr<char[]> dest(new char[s.size() * 2 + 1]);
        mysql_real_escape_string(_handle.mysql.get(), dest.get(), s.c_str(), s.size());
        return dest.get();
      }

      //! call run on the argument
      template <typename T>
      auto run(const T& t) -> decltype(t._run(*this))
      {
        return t._run(*this);
      }

      //! call run on the argument
      template <typename T>
      auto _run(const T& t, ::sqlpp::consistent_t) -> decltype(t._run(*this))
      {
        return t._run(*this);
      }

      template <typename Check, typename T>
      auto _run(const T& t, Check) -> Check;

      template <typename T>
      auto operator()(const T& t) -> decltype(this->_run(t, sqlpp::run_check_t<_serializer_context_t, T>{}))
      {
        return _run(t, sqlpp::run_check_t<_serializer_context_t, T>{});
      }

      //! call prepare on the argument
      template <typename T>
      auto _prepare(const T& t, ::sqlpp::consistent_t) -> decltype(t._prepare(*this))
      {
        return t._prepare(*this);
      }

      template <typename Check, typename T>
      auto _prepare(const T& t, Check) -> Check;

      template <typename T>
      auto prepare(const T& t) -> decltype(this->_prepare(t, sqlpp::prepare_check_t<_serializer_context_t, T>{}))
      {
        return _prepare(t, sqlpp::prepare_check_t<_serializer_context_t, T>{});
      }

      //! start transaction
      void start_transaction()
      {
        if (_transaction_active)
        {
          throw sqlpp::exception("MySQL: Cannot have more than one open transaction per connection");
        }
        execute_statement(_handle, "START TRANSACTION");
        _transaction_active = true;
      }

      //! commit transaction (or throw if the transaction has been finished already)
      void commit_transaction()
      {
        if (not _transaction_active)
        {
          throw sqlpp::exception("MySQL: Cannot commit a finished or failed transaction");
        }
        _transaction_active = false;
        execute_statement(_handle, "COMMIT");
      }

      //! rollback transaction (or throw if the transaction has been finished already)
      void rollback_transaction(bool report)
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
        execute_statement(_handle, "ROLLBACK");
      }

      //! report a rollback failure (will be called by transactions in case of a rollback failure in the destructor)
      void report_rollback_failure(const std::string message) noexcept
      {
        std::cerr << "MySQL message:" << message << std::endl;
      }

      MYSQL* get_handle()
      {
        return _handle.mysql.get();
      }
    };

    inline std::string serializer_t::escape(std::string arg)
    {
      return _db.escape(arg);
    }
  }  // namespace mysql
}  // namespace sqlpp

#include <sqlpp11/mysql/serializer.h>

#endif

#pragma once

/*
 * Copyright (c) 2013 - 2017, Roland Bock
 * Copyright (c) 2023, Vesselin Atanasov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
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

#include <iostream>
#include <sqlpp23/core/database/connection.h>
#include <sqlpp23/core/database/exception.h>
#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/mysql/bind_result.h>
#include <sqlpp23/mysql/char_result.h>
#include <sqlpp23/mysql/clause/delete_from.h>
#include <sqlpp23/mysql/clause/update.h>
#include <sqlpp23/mysql/database/connection_config.h>
#include <sqlpp23/mysql/detail/connection_handle.h>
#include <sqlpp23/mysql/prepared_statement.h>
#include <sstream>
#include <string>

namespace sqlpp {
namespace mysql {
namespace detail {
struct mysql_thread_initializer {
  mysql_thread_initializer() {
    if (!mysql_thread_safe()) {
      throw sqlpp::exception{
          "MySQL error: Operating on a non-threadsafe client"};
    }
    mysql_thread_init();
  }

  ~mysql_thread_initializer() { mysql_thread_end(); }
};

inline void thread_init() {
  thread_local mysql_thread_initializer thread_initializer;
}

inline void execute_statement(std::unique_ptr<connection_handle> &handle,
                              const std::string &statement) {
  thread_init();

  if (handle->config->debug)
    std::cerr << "MySQL debug: Executing: '" << statement << "'" << std::endl;

  if (mysql_query(handle->native_handle(), statement.c_str())) {
    throw sqlpp::exception{"MySQL error: Could not execute MySQL-statement: " +
                           std::string{mysql_error(handle->native_handle())} +
                           " (statement was >>" + statement + "<<\n"};
  }
}

inline void execute_prepared_statement(
    detail::prepared_statement_handle_t &prepared_statement) {
  thread_init();

  if (prepared_statement.debug)
    std::cerr << "MySQL debug: Executing prepared_statement" << std::endl;

  if (mysql_stmt_bind_param(prepared_statement.mysql_stmt,
                            prepared_statement.stmt_params.data())) {
    throw sqlpp::exception{
        std::string{"MySQL error: Could not bind parameters to statement"} +
        mysql_stmt_error(prepared_statement.mysql_stmt)};
  }

  if (mysql_stmt_execute(prepared_statement.mysql_stmt)) {
    throw sqlpp::exception{
        std::string{"MySQL error: Could not execute prepared statement: "} +
        mysql_stmt_error(prepared_statement.mysql_stmt)};
  }
}

inline std::shared_ptr<detail::prepared_statement_handle_t>
prepare_statement(std::unique_ptr<connection_handle> &handle,
                  const std::string &statement, size_t no_of_parameters,
                  size_t no_of_columns) {
  thread_init();

  if (handle->config->debug)
    std::cerr << "MySQL debug: Preparing: '" << statement << "'" << std::endl;

  auto prepared_statement =
      std::make_shared<detail::prepared_statement_handle_t>(
          mysql_stmt_init(handle->native_handle()), no_of_parameters,
          no_of_columns, handle->config->debug);
  if (not prepared_statement) {
    throw sqlpp::exception{
        "MySQL error: Could not allocate prepared statement\n"};
  }
  if (mysql_stmt_prepare(prepared_statement->mysql_stmt, statement.data(),
                         statement.size())) {
    throw sqlpp::exception{"MySQL error: Could not prepare statement: " +
                           std::string{mysql_error(handle->native_handle())} +
                           " (statement was >>" + statement + "<<\n"};
  }

  return prepared_statement;
}

} // namespace detail

struct scoped_library_initializer_t {
  scoped_library_initializer_t(int argc = 0, char **argv = nullptr,
                               char **groups = nullptr) {
    mysql_library_init(argc, argv, groups);
  }

  ~scoped_library_initializer_t() { mysql_library_end(); }
};

// This will also cleanup when the program shuts down
inline void global_library_init(int argc = 0, char **argv = nullptr,
                                char **groups = nullptr) {
  static const auto global_init_and_end =
      scoped_library_initializer_t(argc, argv, groups);
}

struct context_t {};

class connection_base : public sqlpp::connection {
private:
  bool _transaction_active{false};

  // direct execution
  char_result_t select_impl(const std::string &statement) {
    execute_statement(_handle, statement);
    std::unique_ptr<detail::result_handle> result_handle(
        new detail::result_handle(mysql_store_result(_handle->native_handle()),
                                  _handle->config->debug));
    if (!*result_handle) {
      throw sqlpp::exception{
          "MySQL error: Could not store result set: " +
          std::string{mysql_error(_handle->native_handle())}};
    }

    return {std::move(result_handle)};
  }

  uint64_t insert_impl(const std::string &statement) {
    execute_statement(_handle, statement);

    return mysql_insert_id(_handle->native_handle());
  }

  uint64_t update_impl(const std::string &statement) {
    execute_statement(_handle, statement);
    return mysql_affected_rows(_handle->native_handle());
  }

  uint64_t remove_impl(const std::string &statement) {
    execute_statement(_handle, statement);
    return mysql_affected_rows(_handle->native_handle());
  }

  // prepared execution
  prepared_statement_t prepare_impl(const std::string &statement,
                                    size_t no_of_parameters,
                                    size_t no_of_columns) {
    return prepare_statement(_handle, statement, no_of_parameters,
                             no_of_columns);
  }

  bind_result_t
  run_prepared_select_impl(prepared_statement_t &prepared_statement) {
    execute_prepared_statement(*prepared_statement._handle);
    return prepared_statement._handle;
  }

  uint64_t run_prepared_insert_impl(prepared_statement_t &prepared_statement) {
    execute_prepared_statement(*prepared_statement._handle);
    return mysql_stmt_insert_id(prepared_statement._handle->mysql_stmt);
  }

  uint64_t run_prepared_update_impl(prepared_statement_t &prepared_statement) {
    execute_prepared_statement(*prepared_statement._handle);
    return mysql_stmt_affected_rows(prepared_statement._handle->mysql_stmt);
  }

  uint64_t run_prepared_delete(prepared_statement_t &prepared_statement) {
    execute_prepared_statement(*prepared_statement._handle);
    return mysql_stmt_affected_rows(prepared_statement._handle->mysql_stmt);
  }

public:
  using _connection_base_t = connection_base;
  using _config_t = connection_config;
  using _config_ptr_t = std::shared_ptr<const _config_t>;
  using _handle_t = detail::connection_handle;
  using _handle_ptr_t = std::unique_ptr<_handle_t>;
  using _context_t = context_t;

  using _prepared_statement_t = ::sqlpp::mysql::prepared_statement_t;

  struct _tags {
    using _null_result_is_trivial_value = std::true_type;
  };

  [[deprecated("Use ping_server() instead")]] bool is_valid() const {
    return _handle->ping_server();
  }

  const std::shared_ptr<const connection_config> &get_config() {
    return _handle->config;
  }

  template <typename Select> char_result_t select(const Select &s) {
    context_t context;
    const auto query = to_sql_string(context, s);
    return select_impl(query);
  }

  template <typename Select> _prepared_statement_t prepare_select(Select &s) {
    context_t context;
    const auto query = to_sql_string(context, s);
    return prepare_impl(query, s._get_no_of_parameters(),
                        s.get_no_of_result_columns());
  }

  template <typename PreparedSelect>
  bind_result_t run_prepared_select(const PreparedSelect &s) {
    s._bind_params();
    return run_prepared_select_impl(s._prepared_statement);
  }

  //! insert returns the last auto_incremented id (or zero, if there is none)
  template <typename Insert> size_t insert(const Insert &i) {
    context_t context;
    const auto query = to_sql_string(context, i);
    return insert_impl(query);
  }

  template <typename Insert> _prepared_statement_t prepare_insert(Insert &i) {
    context_t context;
    const auto query = to_sql_string(context, i);
    return prepare_impl(query, i._get_no_of_parameters(), 0);
  }

  template <typename PreparedInsert>
  size_t run_prepared_insert(const PreparedInsert &i) {
    i._bind_params();
    return run_prepared_insert_impl(i._prepared_statement);
  }

  //! update returns the number of affected rows
  template <typename Update> size_t update(const Update &u) {
    context_t context;
    const auto query = to_sql_string(context, u);
    return update_impl(query);
  }

  template <typename Update> _prepared_statement_t prepare_update(Update &u) {
    context_t context;
    const auto query = to_sql_string(context, u);
    return prepare_impl(query, u._get_no_of_parameters(), 0);
  }

  template <typename PreparedUpdate>
  size_t run_prepared_update(const PreparedUpdate &u) {
    u._bind_params();
    return run_prepared_update_impl(u._prepared_statement);
  }

  //! remove returns the number of removed rows
  template <typename Remove> size_t remove(const Remove &r) {
    context_t context;
    const auto query = to_sql_string(context, r);
    return remove_impl(query);
  }

  template <typename Remove> _prepared_statement_t prepare_remove(Remove &r) {
    context_t context;
    const auto query = to_sql_string(context, r);
    return prepare_impl(query, r._get_no_of_parameters(), 0);
  }

  template <typename PreparedRemove>
  size_t run_prepared_delete(const PreparedRemove &r) {
    r._bind_params();
    return run_prepared_delete(r._prepared_statement);
  }

  //! Execute arbitrary statement (e.g. create a table).
  //! Essentially this calls mysql_query, see
  //! https://dev.mysql.com/doc/c-api/8.0/en/mysql-query.html Note:
  //!  * This usually only allows a single statement (unless configured
  //!  otherwise for the connection).
  //!  * If you are passing a statement with results, like a SELECT, you will
  //!  need to fetch results before issuing
  //!    the next statement on the same connection.
  void execute(const std::string &statement) {
    execute_statement(_handle, statement);
  }

  //! call run on the argument
  template <typename T> auto run(const T &t) -> decltype(t._run(*this)) {
    return t._run(*this);
  }

  //! call run on the argument
  template <typename T>
  auto _run(const T &t, ::sqlpp::consistent_t) -> decltype(t._run(*this)) {
    return t._run(*this);
  }

  template <typename Check, typename T> auto _run(const T &t, Check) -> Check;

  template <typename T>
  auto operator()(const T &t)
      -> decltype(this->_run(t, sqlpp::statement_run_check_t<T>{})) {
    return _run(t, sqlpp::statement_run_check_t<T>{});
  }

  //! call prepare on the argument
  template <typename T>
  auto _prepare(const T &t, ::sqlpp::consistent_t)
      -> decltype(t._prepare(*this)) {
    return t._prepare(*this);
  }

  template <typename Check, typename T>
  auto _prepare(const T &t, Check) -> Check;

  template <typename T>
  auto prepare(const T &t)
      -> decltype(this->_prepare(t, sqlpp::statement_prepare_check_t<T>{})) {
    return _prepare(t, sqlpp::statement_prepare_check_t<T>{});
  }

  //! start transaction
  void start_transaction() {
    if (_transaction_active) {
      throw sqlpp::exception{
          "MySQL: Cannot have more than one open transaction per connection"};
    }
    execute_statement(_handle, "START TRANSACTION");
    _transaction_active = true;
  }

  //! commit transaction (or throw if the transaction has been finished already)
  void commit_transaction() {
    if (not _transaction_active) {
      throw sqlpp::exception{
          "MySQL: Cannot commit a finished or failed transaction"};
    }
    execute_statement(_handle, "COMMIT");
    _transaction_active = false;
  }

  //! rollback transaction (or throw if the transaction has been finished
  //! already)
  void rollback_transaction(bool report) {
    if (not _transaction_active) {
      throw sqlpp::exception{
          "MySQL: Cannot rollback a finished or failed transaction"};
    }
    if (report) {
      std::cerr << "MySQL warning: Rolling back unfinished transaction"
                << std::endl;
    }
    execute_statement(_handle, "ROLLBACK");
    _transaction_active = false;
  }

  //! report a rollback failure (will be called by transactions in case of a
  //! rollback failure in the destructor)
  void report_rollback_failure(const std::string &message) noexcept {
    std::cerr << "MySQL message:" << message << std::endl;
  }

  //! check if transaction is active
  bool is_transaction_active() { return _transaction_active; }

  MYSQL *native_handle() { return _handle->native_handle(); }

  // Kept for compatibility with old code
  MYSQL *get_handle() { return native_handle(); }

protected:
  _handle_ptr_t _handle;

  // Constructors
  connection_base() = default;
  connection_base(_handle_ptr_t &&handle) : _handle{std::move(handle)} {}
};

using connection = sqlpp::normal_connection<connection_base>;
using pooled_connection = sqlpp::pooled_connection<connection_base>;
} // namespace mysql
} // namespace sqlpp

#include <sqlpp23/mysql/serializer.h>

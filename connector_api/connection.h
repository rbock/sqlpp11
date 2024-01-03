#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include <string>
#include <sqlpp11/connection.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/database/char_result.h>  // You may use char result or bind result or both
#include <sqlpp11/database/bind_result.h>  // to represent results of select and prepared select

namespace sqlpp
{
  namespace database
  {
    // Connection configuration that is used to create new database connections
    struct connection_config
    {
      // Put the configuration properties here, e.g.
      //
      // std::string host;
      // unsigned port;
      // std::string username;
      // std::string password;
      // std::string db_name;
    };

    // The connection handle is a low-level representation of a database connection.
    // Pre-connected handles are stored in the connection pool and are used to create
    // full connection objects on request.
    class connection_handle
    {
    public:
      // Connection handles can be created from connection configurations
      connection_handle(const std::shared_ptr<const connection_config>& config);

      // Connection handles cannot be copied
      connection_handle(const connection_handle&) = delete;
      connection_handle& operator=(const connection_handle&) = delete;

      // Connection handles can be moved
      connection_handle(connection_handle&&);
      connection_handle& operator=(connection_handle&&);

      // Used by the connection pool to check if the connection handle is still
      // connected to the database server.
      bool is_connected();

      // Send a dummy request to the server to check if the connection is still alive
      bool ping_server();

      // Optional method that returns a native (low-level) database handle.
      // Used by the test code to test the connection pool
      native_db_handle native_handle();
    };

    // The context is not a requirement, but if the database requires
    // any deviations from the SQL standard, you should use your own
    // context in order to specialize the behaviour, see also interpreter.h
    struct context_t
    {
      template <typename T>
      std::ostream& operator<<(T t);

      std::string escape(std::string arg);
    };

    // The base database-specific connection class. Non-pooled and pooled connection classes derive from it
    class connection_base : public sqlpp::connection  // this inheritance helps with ADL for dynamic_select, for instance
    {
    public:
      // Base configuration
      using _connection_base_t = connection_base;

      // Type of configuration instances
      using _config_t = connection_config;

      // Shared pointer wrapping a configuration instance
      using _config_ptr_t = std::shared_ptr<const _config_t>;

      // Type of connection handles
      using _handle_t = connection_handle;

      // Unique pointer wrapping a connection handle
      using _handle_ptr_t = std::unique_ptr<_handle_t>;

      using _traits = ::sqlpp::make_traits<
          ::sqlpp::no_value_t,
          ::sqlpp::tag::enforce_null_result_treatment  // If that is what you really want, leave it out otherwise
          >;

      using _prepared_statement_t = << handle to a prepared statement of the database >> ;
      using _serializer_context_t = << This context is used to serialize a statement >> using _interpreter_context_t =
                                    << This context is used interpret a statement >>
          ;
      // serializer and interpreter are typically the same for string based connectors
      // the types are required for dynamic statement components, see sqlpp11/interpretable.h

      //! "direct" select
      template <typename Select>
              << bind_result_t >>
          select(const Select& s);

      //! prepared select
      template <typename Select>
      _prepared_statement_t prepare_select(Select& s);

      template <typename PreparedSelect>
              << bind_result_t >>
          run_prepared_select(const PreparedSelect& s);  // call s._bind_params()

      //! "direct insert
      template <typename Insert>
      size_t insert(const Insert& i);

      //! prepared insert
      template <typename Insert>
      _prepared_statement_t prepare_insert(Insert& i);

      template <typename PreparedInsert>
      size_t run_prepared_insert(const PreparedInsert& i);  // call i._bind_params()

      //! "direct" update
      template <typename Update>
      size_t update(const Update& u);

      //! "prepared" update
      template <typename Update>
      _prepared_statement_t prepare_update(Update& u);

      template <typename PreparedUpdate>
      size_t run_prepared_update(const PreparedUpdate& u);  // call u._bind_params()

      //! "direct" remove
      template <typename Remove>
      size_t remove(const Remove& r)

          //! prepared remove
          template <typename Remove>
          _prepared_statement_t prepare_remove(Remove& r);

      template <typename PreparedRemove>
      size_t run_prepared_remove(const PreparedRemove& r);  // call r._bind_params()

      //! call run on the argument
      template <typename T>
      auto operator()(const T& t) -> decltype(t._run(*this))
      {
        return t._run(*this);
      }

      //! call prepare on the argument
      template <typename T>
      auto prepare(const T& t) -> decltype(t._prepare(*this))
      {
        return t._prepare(*this);
      }

      //! set the transaction isolation level for the current connection
      /// time of effect is connector-specific, for most is will only affect new transactions
      void set_default_isolation_level(sqlpp::isolation_level);

      //! read the default transaction isolation level for the current connection
      sqlpp::isolation_level get_default_isolation_level();

      //! start transaction
      void start_transaction();

      //! start transaction with defined isolation level (optional only for connectors that support it)
      void start_transaction(isolation_level isolation /* = isolation_level::undefined */);

      //! commit transaction (or throw transaction if the transaction has been finished already)
      void commit_transaction();

      //! rollback transaction with or without reporting the rollback (or throw if the transaction has been finished
      // already)
      void rollback_transaction(bool report);

      //! report a rollback failure (will be called by transactions in case of a rollback failure in the destructor)
      void report_rollback_failure(const std::string message) noexcept;

      //! check if transaction is active
      bool is_transaction_active();

    protected:
      // Low-level connection handle
      _handle_ptr_t _handle;

      // The constructors are private because the base class instances are never created directly,
      // The constructors are called from the constructors of the derived classes
      connection_base() = default;
      connection_base(_handle_ptr_t&& handle) : _handle{std::move(handle)}
      {
      }
    };

    // Normal non-pooled connections.
    using connection = sqlpp::normal_connection<connection_base>;

    // Pooled connections that are created by the thread pool
    using pooled_connection = sqlpp::pooled_connection<connection_base>;
  }  // namespace database
}  // namespace sqlpp

#include <sqlpp11/database/interpreter.h>

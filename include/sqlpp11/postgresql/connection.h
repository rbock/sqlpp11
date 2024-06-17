#pragma once

/**
 * Copyright © 2014-2015, Matthijs Möhlmann
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

#include <algorithm>
#include <iostream>
#include <sstream>

#include <sqlpp11/compat/make_unique.h>
#include <sqlpp11/connection.h>
#include <sqlpp11/detail/float_safe_ostringstream.h>
#include <sqlpp11/postgresql/bind_result.h>
#include <sqlpp11/postgresql/result_field.h>
#include <sqlpp11/postgresql/connection_config.h>
#include <sqlpp11/postgresql/prepared_statement.h>
#include <sqlpp11/postgresql/exception.h>
#include <sqlpp11/postgresql/result.h>
#include <sqlpp11/postgresql/detail/connection_handle.h>
#include <sqlpp11/postgresql/detail/prepared_statement_handle.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/transaction.h>

#ifdef SQLPP_DYNAMIC_LOADING
#include <sqlpp11/postgresql/dynamic_libpq.h>
#endif

struct pg_conn;
typedef struct pg_conn PGconn;

namespace sqlpp
{
  namespace postgresql
  {
#ifdef SQLPP_DYNAMIC_LOADING
    using namespace dynamic;
#endif

    namespace detail
    {
      inline std::unique_ptr<detail::prepared_statement_handle_t> prepare_statement(std::unique_ptr<connection_handle>& handle,
                                                                             const std::string& stmt,
                                                                             const size_t& param_count)
      {
        if (handle->config->debug)
        {
          std::cerr << "PostgreSQL debug: preparing: " << stmt << std::endl;
        }

        return sqlpp::compat::make_unique<detail::prepared_statement_handle_t>(*handle, stmt, param_count);
      }

      inline void execute_prepared_statement(std::unique_ptr<connection_handle>& handle, std::shared_ptr<detail::prepared_statement_handle_t>& prepared)
      {
        if (handle->config->debug)
        {
          std::cerr << "PostgreSQL debug: executing: " << prepared->name() << std::endl;
        }
        prepared->execute();
      }
    }

    // Forward declaration
    class connection_base;

    // Context
    struct context_t
    {
      context_t(const connection_base& db) : _db(db)
      {
      }
      context_t(const connection_base&&) = delete;

      template <typename T>
      std::ostream& operator<<(T t)
      {
        return _os << t;
      }

      std::ostream& operator<<(bool t)
      {
        return _os << (t ? "TRUE" : "FALSE");
      }

      std::string escape(const std::string& arg) const;

      std::string str() const
      {
        return _os.str();
      }

      size_t count() const
      {
        return _count;
      }

      void pop_count()
      {
        ++_count;
      }

      const connection_base& _db;
      sqlpp::detail::float_safe_ostringstream _os;
      size_t _count{1};
    };

    // Base connection class
    class connection_base : public sqlpp::connection
    {
    private:
      bool _transaction_active{false};

      void validate_connection_handle() const
      {
        if (!_handle) {
          throw std::logic_error{"connection handle used, but not initialized"};
        }
      }

      // direct execution
      bind_result_t select_impl(const std::string& stmt)
      {
        return execute(stmt);
      }

      size_t insert_impl(const std::string& stmt)
      {
        return static_cast<size_t>(execute(stmt)->result.affected_rows());
      }

      size_t update_impl(const std::string& stmt)
      {
        return static_cast<size_t>(execute(stmt)->result.affected_rows());
      }

      size_t remove_impl(const std::string& stmt)
      {
        return static_cast<size_t>(execute(stmt)->result.affected_rows());
      }

      // prepared execution
      prepared_statement_t prepare_impl(const std::string& stmt, const size_t& param_count)
      {
        validate_connection_handle();
        return {prepare_statement(_handle, stmt, param_count)};
      }

      bind_result_t run_prepared_select_impl(prepared_statement_t& prep)
      {
        validate_connection_handle();
        execute_prepared_statement(_handle, prep._handle);
        return {prep._handle};
      }

      size_t run_prepared_execute_impl(prepared_statement_t& prep)
      {
        validate_connection_handle();
        execute_prepared_statement(_handle, prep._handle);
        return static_cast<size_t>(prep._handle->result.affected_rows());
      }

      size_t run_prepared_insert_impl(prepared_statement_t& prep)
      {
        validate_connection_handle();
        execute_prepared_statement(_handle, prep._handle);
        return static_cast<size_t>(prep._handle->result.affected_rows());
      }

      size_t run_prepared_update_impl(prepared_statement_t& prep)
      {
        validate_connection_handle();
        execute_prepared_statement(_handle, prep._handle);
        return static_cast<size_t>(prep._handle->result.affected_rows());
      }

      size_t run_prepared_remove_impl(prepared_statement_t& prep)
      {
        validate_connection_handle();
        execute_prepared_statement(_handle, prep._handle);
        return static_cast<size_t>(prep._handle->result.affected_rows());
      }

    public:
      using _connection_base_t = connection_base;
      using _config_t = connection_config;
      using _config_ptr_t = std::shared_ptr<const _config_t>;
      using _handle_t = detail::connection_handle;
      using _handle_ptr_t = std::unique_ptr<_handle_t>;

      using _prepared_statement_t = prepared_statement_t;
      using _context_t = context_t;
      using _serializer_context_t = _context_t;
      using _interpreter_context_t = _context_t;

      struct _tags
      {
        using _null_result_is_trivial_value = std::true_type;
      };

      template <typename T>
      static _context_t& _serialize_interpretable(const T& t, _context_t& context)
      {
        return ::sqlpp::serialize(t, context);
      }

      template <typename T>
      static _context_t& _interpret_interpretable(const T& t, _context_t& context)
      {
        return ::sqlpp::serialize(t, context);
      }

      // Select stmt (returns a result)
      template <typename Select>
      bind_result_t select(const Select& s)
      {
        _context_t ctx{*this};
        serialize(s, ctx);
        return select_impl(ctx.str());
      }

      // Prepared select
      template <typename Select>
      _prepared_statement_t prepare_select(Select& s)
      {
        _context_t ctx{*this};
        serialize(s, ctx);
        return prepare_impl(ctx.str(), ctx.count() - 1);
      }

      template <typename PreparedSelect>
      bind_result_t run_prepared_select(const PreparedSelect& s)
      {
        s._bind_params();
        return run_prepared_select_impl(s._prepared_statement);
      }

      // Insert
      template <typename Insert>
      size_t insert(const Insert& i)
      {
        _context_t ctx{*this};
        serialize(i, ctx);
        return insert_impl(ctx.str());
      }

      template <typename Insert>
      prepared_statement_t prepare_insert(Insert& i)
      {
        _context_t ctx{*this};
        serialize(i, ctx);
        return prepare_impl(ctx.str(), ctx.count() - 1);
      }

      template <typename PreparedInsert>
      size_t run_prepared_insert(const PreparedInsert& i)
      {
        i._bind_params();
        return run_prepared_insert_impl(i._prepared_statement);
      }

      // Update
      template <typename Update>
      size_t update(const Update& u)
      {
        _context_t ctx{*this};
        serialize(u, ctx);
        return update_impl(ctx.str());
      }

      template <typename Update>
      prepared_statement_t prepare_update(Update& u)
      {
        _context_t ctx{*this};
        serialize(u, ctx);
        return prepare_impl(ctx.str(), ctx.count() - 1);
      }

      template <typename PreparedUpdate>
      size_t run_prepared_update(const PreparedUpdate& u)
      {
        u._bind_params();
        return run_prepared_update_impl(u._prepared_statement);
      }

      // Remove
      template <typename Remove>
      size_t remove(const Remove& r)
      {
        _context_t ctx{*this};
        serialize(r, ctx);
        return remove_impl(ctx.str());
      }

      template <typename Remove>
      prepared_statement_t prepare_remove(Remove& r)
      {
        _context_t ctx{*this};
        serialize(r, ctx);
        return prepare_impl(ctx.str(), ctx.count() - 1);
      }

      template <typename PreparedRemove>
      size_t run_prepared_remove(const PreparedRemove& r)
      {
        r._bind_params();
        return run_prepared_remove_impl(r._prepared_statement);
      }

      //! Execute a single statement (like creating a table).
      //! Note that technically, this supports executing multiple statements today, but this is likely to change to
      //! align with other connectors.
      std::shared_ptr<detail::statement_handle_t> execute(const std::string& stmt)
      {
        validate_connection_handle();
        if (_handle->config->debug)
        {
          std::cerr << "PostgreSQL debug: executing: " << stmt << std::endl;
        }

        auto result = std::make_shared<detail::statement_handle_t>(*_handle);
        result->result = PQexec(native_handle(), stmt.c_str());
        result->valid = true;

        return result;
      }

      template <
          typename Execute,
          typename Enable = typename std::enable_if<not std::is_convertible<Execute, std::string>::value, void>::type>
      std::shared_ptr<detail::statement_handle_t> execute(const Execute& x)
      {
        _context_t ctx{*this};
        serialize(x, ctx);
        return execute(ctx.str());
      }

      template <typename Execute>
      _prepared_statement_t prepare_execute(Execute& x)
      {
        _context_t ctx{*this};
        serialize(x, ctx);
        return prepare_impl(ctx.str(), ctx.count() - 1);
      }

      template <typename PreparedExecute>
      size_t run_prepared_execute(const PreparedExecute& x)
      {
        x._prepared_statement._reset();
        x._bind_params();
        return run_prepared_execute_impl(x._prepared_statement);
      }

      // escape argument
      // TODO: Fix escaping.
      std::string escape(const std::string& s) const
      {
        validate_connection_handle();
        // Escape strings
        std::string result;
        result.resize((s.size() * 2) + 1);

        int err;
        size_t length = PQescapeStringConn(native_handle(), &result[0], s.c_str(), s.size(), &err);
        result.resize(length);
        return result;
      }

      //! call run on the argument
      template <typename T>
      auto _run(const T& t, sqlpp::consistent_t) -> decltype(t._run(*this))
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

      //! set the default transaction isolation level to use for new transactions
      void set_default_isolation_level(isolation_level level)
      {
        std::string level_str = "read uncommmitted";
        switch (level)
        {
          /// @todo what about undefined ?
          case isolation_level::read_committed:
            level_str = "read committed";
            break;
          case isolation_level::read_uncommitted:
            level_str = "read uncommitted";
            break;
          case isolation_level::repeatable_read:
            level_str = "repeatable read";
            break;
          case isolation_level::serializable:
            level_str = "serializable";
            break;
          default:
            throw sqlpp::exception{"Invalid isolation level"};
        }
        std::string cmd = "SET default_transaction_isolation to '" + level_str + "'";
        execute(cmd);
      }

      //! get the currently set default transaction isolation level
      isolation_level get_default_isolation_level()
      {
        auto res = execute("SHOW default_transaction_isolation;");
        auto status = res->result.status();
        if ((status != PGRES_TUPLES_OK) && (status != PGRES_COMMAND_OK))
        {
          throw sqlpp::exception{"PostgreSQL error: could not read default_transaction_isolation"};
        }

        auto in = res->result.get_string_value(0, 0);
        if (in == "read committed")
        {
          return isolation_level::read_committed;
        }
        else if (in == "read uncommitted")
        {
          return isolation_level::read_uncommitted;
        }
        else if (in == "repeatable read")
        {
          return isolation_level::repeatable_read;
        }
        else if (in == "serializable")
        {
          return isolation_level::serializable;
        }
        return isolation_level::undefined;
      }

      //! create savepoint
      void savepoint(const std::string& name)
      {
        /// NOTE prevent from sql injection?
        execute("SAVEPOINT " + name);
      }

      //! ROLLBACK TO SAVEPOINT
      void rollback_to_savepoint(const std::string& name)
      {
        /// NOTE prevent from sql injection?
        execute("ROLLBACK TO SAVEPOINT " + name);
      }

      //! release_savepoint
      void release_savepoint(const std::string& name)
      {
        /// NOTE prevent from sql injection?
        execute("RELEASE SAVEPOINT " + name);
      }

      //! start transaction
      void start_transaction(isolation_level level = isolation_level::undefined)
      {
        if (_transaction_active)
        {
          throw sqlpp::exception{"PostgreSQL error: transaction already open"};
        }
        switch (level)
        {
          case isolation_level::serializable:
          {
            execute("BEGIN ISOLATION LEVEL SERIALIZABLE");
            break;
          }
          case isolation_level::repeatable_read:
          {
            execute("BEGIN ISOLATION LEVEL REPEATABLE READ");
            break;
          }
          case isolation_level::read_committed:
          {
            execute("BEGIN ISOLATION LEVEL READ COMMITTED");
            break;
          }
          case isolation_level::read_uncommitted:
          {
            execute("BEGIN ISOLATION LEVEL READ UNCOMMITTED");
            break;
          }
          case isolation_level::undefined:
          {
            execute("BEGIN");
            break;
          }
        }
        _transaction_active = true;
      }

      //! commit transaction (or throw transaction if transaction has finished already)
      void commit_transaction()
      {
        if (!_transaction_active)
        {
          throw sqlpp::exception{"PostgreSQL error: transaction failed or finished."};
        }
        execute("COMMIT");
        _transaction_active = false;
      }

      //! rollback transaction
      void rollback_transaction(bool report)
      {
        if (!_transaction_active)
        {
          throw sqlpp::exception{"PostgreSQL error: transaction failed or finished."};
        }
        if (report)
        {
          std::cerr << "PostgreSQL warning: rolling back unfinished transaction" << std::endl;
        }
        execute("ROLLBACK");
        _transaction_active = false;
      }

      //! report rollback failure
      void report_rollback_failure(const std::string& message) noexcept
      {
        std::cerr << "PostgreSQL error: " << message << std::endl;
      }

      //! check if transaction is active
      bool is_transaction_active()
      {
        return _transaction_active;
      }

      //! get the last inserted id for a certain table
      uint64_t last_insert_id(const std::string& table, const std::string& fieldname)
      {
        std::string sql = "SELECT currval('" + table + "_" + fieldname + "_seq')";
        PGresult* res = PQexec(native_handle(), sql.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK)
        {
          std::string err{PQresultErrorMessage(res)};
          PQclear(res);
          throw sqlpp::postgresql::undefined_table{err, sql};
        }

        // Parse the number and return.
        std::string in{PQgetvalue(res, 0, 0)};
        PQclear(res);
        return std::stoul(in);
      }

      ::PGconn* native_handle() const
      {
        return _handle->native_handle();
      }

    protected:
      _handle_ptr_t _handle;

      // Constructors
      connection_base() = default;
      connection_base(_handle_ptr_t&& handle) : _handle{std::move(handle)}
      {
      }
    };

    // Method definition moved outside of class because it needs connection_base
    inline std::string context_t::escape(const std::string& arg) const
    {
      return _db.escape(arg);
    }

    using connection = sqlpp::normal_connection<connection_base>;
    using pooled_connection = sqlpp::pooled_connection<connection_base>;
  }  // namespace postgresql
}  // namespace sqlpp

#include <sqlpp11/postgresql/serializer.h>

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

#ifndef SQLPP_POSTGRESQL_CONNECTION_H
#define SQLPP_POSTGRESQL_CONNECTION_H

#include <algorithm>
#include <iostream>
#include <sstream>

#include <sqlpp11/connection.h>
#include <sqlpp11/postgresql/bind_result.h>
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
      // Forward declaration
      inline std::unique_ptr<detail::prepared_statement_handle_t> prepare_statement(detail::connection_handle& handle,
                                                                             const std::string& stmt,
                                                                             const size_t& paramCount)
      {
        if (handle.config->debug)
        {
          std::cerr << "PostgreSQL debug: preparing: " << stmt << std::endl;
        }

        return std::unique_ptr<detail::prepared_statement_handle_t>(new detail::prepared_statement_handle_t 
                                                                    (handle, stmt, paramCount));
      }

      inline void execute_prepared_statement(detail::connection_handle& handle, detail::prepared_statement_handle_t& prepared)
      {
        if (handle.config->debug)
        {
          std::cerr << "PostgreSQL debug: executing: " << prepared.name() << std::endl;
        }
        prepared.execute();
      }
    }

    // Forward declaration
    class connection;

    // Context
    struct context_t
    {
      context_t(const connection& db) : _db(db)
      {
      }
      context_t(const connection&&) = delete;

      template <typename T>
      std::ostream& operator<<(T t)
      {
        return _os << t;
      }

      std::ostream& operator<<(bool t)
      {
        return _os << (t ? "TRUE" : "FALSE");
      }

      std::string escape(const std::string& arg);

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

      const connection& _db;
      std::ostringstream _os;
      size_t _count{1};
    };

    // Connection
    class connection : public sqlpp::connection
    {
    private:
      std::unique_ptr<detail::connection_handle> _handle;
      bool _transaction_active{false};

      void validate_connection_handle() const
      {
        if (!_handle)
          throw std::logic_error("connection handle used, but not initialized");
      }

      // direct execution
      bind_result_t select_impl(const std::string& stmt);
      size_t insert_impl(const std::string& stmt);
      size_t update_impl(const std::string& stmt);
      size_t remove_impl(const std::string& stmt);

      // prepared execution
      prepared_statement_t prepare_impl(const std::string& stmt, const size_t& paramCount);
      bind_result_t run_prepared_select_impl(prepared_statement_t& prep);
      size_t run_prepared_execute_impl(prepared_statement_t& prep);
      size_t run_prepared_insert_impl(prepared_statement_t& prep);
      size_t run_prepared_update_impl(prepared_statement_t& prep);
      size_t run_prepared_remove_impl(prepared_statement_t& prep);

    public:
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

      // ctor / dtor
      connection();
      connection(const std::shared_ptr<connection_config>& config);
      ~connection();
      connection(const connection&) = delete;
      connection(connection&&);
      connection& operator=(const connection&) = delete;
      connection& operator=(connection&&);

      // creates a connection handle and connects to database
      void connectUsing(const std::shared_ptr<connection_config>& config) noexcept(false);

      // Select stmt (returns a result)
      template <typename Select>
      bind_result_t select(const Select& s)
      {
        _context_t ctx(*this);
        serialize(s, ctx);
        return select_impl(ctx.str());
      }

      // Prepared select
      template <typename Select>
      _prepared_statement_t prepare_select(Select& s)
      {
        _context_t ctx(*this);
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
        _context_t ctx(*this);
        serialize(i, ctx);
        return insert_impl(ctx.str());
      }

      template <typename Insert>
      prepared_statement_t prepare_insert(Insert& i)
      {
        _context_t ctx(*this);
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
        _context_t ctx(*this);
        serialize(u, ctx);
        return update_impl(ctx.str());
      }

      template <typename Update>
      prepared_statement_t prepare_update(Update& u)
      {
        _context_t ctx(*this);
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
        _context_t ctx(*this);
        serialize(r, ctx);
        return remove_impl(ctx.str());
      }

      template <typename Remove>
      prepared_statement_t prepare_remove(Remove& r)
      {
        _context_t ctx(*this);
        serialize(r, ctx);
        return prepare_impl(ctx.str(), ctx.count() - 1);
      }

      template <typename PreparedRemove>
      size_t run_prepared_remove(const PreparedRemove& r)
      {
        r._bind_params();
        return run_prepared_remove_impl(r._prepared_statement);
      }

      // Execute
      std::shared_ptr<detail::statement_handle_t> execute(const std::string& command);

      template <
          typename Execute,
          typename Enable = typename std::enable_if<not std::is_convertible<Execute, std::string>::value, void>::type>
      std::shared_ptr<detail::statement_handle_t> execute(const Execute& x)
      {
        _context_t ctx(*this);
        serialize(x, ctx);
        return execute(ctx.str());
      }

      template <typename Execute>
      _prepared_statement_t prepare_execute(Execute& x)
      {
        _context_t ctx(*this);
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
      std::string escape(const std::string& s) const;

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
      void set_default_isolation_level(isolation_level level);

      //! get the currently set default transaction isolation level
      isolation_level get_default_isolation_level();

      //! create savepoint
      void savepoint(const std::string& name);

      //! ROLLBACK TO SAVEPOINT
      void rollback_to_savepoint(const std::string& name);

      //! release_savepoint
      void release_savepoint(const std::string& name);

      //! start transaction
      void start_transaction(isolation_level level = isolation_level::undefined);

      //! commit transaction (or throw transaction if transaction has
      // finished already)
      void commit_transaction();

      //! rollback transaction
      void rollback_transaction(bool report);

      //! report rollback failure
      void report_rollback_failure(const std::string& message) noexcept;

      //! get the last inserted id for a certain table
      uint64_t last_insert_id(const std::string& table, const std::string& fieldname);

      ::PGconn* native_handle();
    };

    inline connection::connection() : _handle()
    {
    }

    inline connection::connection(const std::shared_ptr<connection_config>& config)
        : _handle(new detail::connection_handle(config))
    {
    }

    inline connection::~connection()
    {
    }

    inline connection::connection(connection&& other)
    {
      this->_transaction_active = other._transaction_active;
      this->_handle = std::move(other._handle);
    }

    inline connection& connection::operator=(connection&& other)
    {
      if (this != &other)
      {
        // TODO: check this logic
        this->_transaction_active = other._transaction_active;
        this->_handle = std::move(other._handle);
      }
      return *this;
    }

    inline void connection::connectUsing(const std::shared_ptr<connection_config>& config) noexcept(false)
    {
      this->_handle.reset(new detail::connection_handle(config));
    }

    inline std::shared_ptr<detail::statement_handle_t> connection::execute(const std::string& stmt)
    {
      validate_connection_handle();
      if (_handle->config->debug)
      {
        std::cerr << "PostgreSQL debug: executing: " << stmt << std::endl;
      }

      auto result = std::make_shared<detail::statement_handle_t>(*_handle);
      result->result = PQexec(_handle->native(), stmt.c_str());
      result->valid = true;

      return result;
    }
    // direct execution
    inline bind_result_t connection::select_impl(const std::string& stmt)
    {
      return execute(stmt);
    }

    inline size_t connection::insert_impl(const std::string& stmt)
    {
      return execute(stmt)->result.affected_rows();
    }

    inline size_t connection::update_impl(const std::string& stmt)
    {
      return execute(stmt)->result.affected_rows();
    }

    inline size_t connection::remove_impl(const std::string& stmt)
    {
      return execute(stmt)->result.affected_rows();
    }

    // prepared execution
    inline prepared_statement_t connection::prepare_impl(const std::string& stmt, const size_t& paramCount)
    {
      validate_connection_handle();
      return {prepare_statement(*_handle, stmt, paramCount)};
    }

    inline bind_result_t connection::run_prepared_select_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return {prep._handle};
    }

    inline size_t connection::run_prepared_execute_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    inline size_t connection::run_prepared_insert_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    inline size_t connection::run_prepared_update_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    inline size_t connection::run_prepared_remove_impl(prepared_statement_t& prep)
    {
      validate_connection_handle();
      execute_prepared_statement(*_handle, *prep._handle.get());
      return prep._handle->result.affected_rows();
    }

    inline void connection::set_default_isolation_level(isolation_level level)
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
          throw sqlpp::exception("Invalid isolation level");
      }
      std::string cmd = "SET default_transaction_isolation to '" + level_str + "'";
      execute(cmd);
    }

    inline isolation_level connection::get_default_isolation_level()
    {
      auto res = execute("SHOW default_transaction_isolation;");
      auto status = res->result.status();
      if ((status != PGRES_TUPLES_OK) && (status != PGRES_COMMAND_OK))
      {
        throw sqlpp::exception("PostgreSQL error: could not read default_transaction_isolation");
      }

      auto in = res->result.getValue<std::string>(0, 0);
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

    // TODO: Fix escaping.
    inline std::string connection::escape(const std::string& s) const
    {
      validate_connection_handle();
      // Escape strings
      std::string result;
      result.resize((s.size() * 2) + 1);

      int err;
      size_t length = PQescapeStringConn(_handle->postgres, &result[0], s.c_str(), s.size(), &err);
      result.resize(length);
      return result;
    }

    //! start transaction
    inline void connection::start_transaction(sqlpp::isolation_level level)
    {
      if (_transaction_active)
      {
        throw sqlpp::exception("PostgreSQL error: transaction already open");
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

    //! create savepoint
    inline void connection::savepoint(const std::string& name)
    {
      /// NOTE prevent from sql injection?
      execute("SAVEPOINT " + name);
    }

    //! ROLLBACK TO SAVEPOINT
    inline void connection::rollback_to_savepoint(const std::string& name)
    {
      /// NOTE prevent from sql injection?
      execute("ROLLBACK TO SAVEPOINT " + name);
    }

    //! release_savepoint
    inline void connection::release_savepoint(const std::string& name)
    {
      /// NOTE prevent from sql injection?
      execute("RELEASE SAVEPOINT " + name);
    }

    //! commit transaction (or throw transaction if transaction has finished already)
    inline void connection::commit_transaction()
    {
      if (!_transaction_active)
      {
        throw sqlpp::exception("PostgreSQL error: transaction failed or finished.");
      }

      _transaction_active = false;
      execute("COMMIT");
    }

    //! rollback transaction
    inline void connection::rollback_transaction(bool report)
    {
      if (!_transaction_active)
      {
        throw sqlpp::exception("PostgreSQL error: transaction failed or finished.");
      }
      execute("ROLLBACK");
      if (report)
      {
        std::cerr << "PostgreSQL warning: rolling back unfinished transaction" << std::endl;
      }

      _transaction_active = false;
    }

    //! report rollback failure
    inline void connection::report_rollback_failure(const std::string& message) noexcept
    {
      std::cerr << "PostgreSQL error: " << message << std::endl;
    }

    inline uint64_t connection::last_insert_id(const std::string& table, const std::string& fieldname)
    {
      std::string sql = "SELECT currval('" + table + "_" + fieldname + "_seq')";
      PGresult* res = PQexec(_handle->postgres, sql.c_str());
      if (PQresultStatus(res) != PGRES_TUPLES_OK)
      {
        std::string err{PQresultErrorMessage(res)};
        PQclear(res);
        throw sqlpp::postgresql::undefined_table(err, sql);
      }

      // Parse the number and return.
      std::string in{PQgetvalue(res, 0, 0)};
      PQclear(res);
      return std::stoi(in);
    }

    inline ::PGconn* connection::native_handle()
    {
      return _handle->postgres;
    }

    inline std::string context_t::escape(const std::string& arg)
    {
      return _db.escape(arg);
    }
  }
}

#include <sqlpp11/postgresql/serializer.h>

#endif

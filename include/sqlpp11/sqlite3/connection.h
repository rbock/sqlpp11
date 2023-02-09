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

#ifndef SQLPP_SQLITE3_CONNECTION_H
#define SQLPP_SQLITE3_CONNECTION_H

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <sqlpp11/connection.h>
#include <sqlpp11/detail/float_safe_ostringstream.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/schema.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/sqlite3/bind_result.h>
#include <sqlpp11/sqlite3/connection_config.h>
#include <sqlpp11/sqlite3/prepared_statement.h>
#include <sqlpp11/sqlite3/export.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/type_traits.h>
#include <sstream>
#include <string>

#ifdef SQLPP_DYNAMIC_LOADING
#include <sqlpp11/sqlite3/dynamic_libsqlite3.h>
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace sqlpp
{
  namespace sqlite3
  {
#ifdef SQLPP_DYNAMIC_LOADING
    using namespace dynamic;
#endif

    namespace detail
    {
      inline void handle_cleanup(::sqlite3* sqlite)
      {
        sqlite3_close(sqlite);
      }

      struct connection_handle
      {
        connection_config config;
        std::unique_ptr<::sqlite3, void (*)(::sqlite3*)> sqlite;

        connection_handle(connection_config conf) : config(conf), sqlite(nullptr, handle_cleanup)
        {
#ifdef SQLPP_DYNAMIC_LOADING
          init_sqlite("");
#endif

          ::sqlite3* sqlite_ptr;
          const auto rc = sqlite3_open_v2(conf.path_to_database.c_str(), &sqlite_ptr, conf.flags,
                                    conf.vfs.empty() ? nullptr : conf.vfs.c_str());
          if (rc != SQLITE_OK)
          {
            const std::string msg = sqlite3_errmsg(sqlite_ptr);
            sqlite3_close(sqlite_ptr);
            throw sqlpp::exception("Sqlite3 error: Can't open database: " + msg);
          }

          sqlite.reset(sqlite_ptr);

#ifdef SQLITE_HAS_CODEC
          if (conf.password.size() > 0)
          {
            int ret = sqlite3_key(sqlite.get(), conf.password.data(), conf.password.size());
            if (ret != SQLITE_OK)
            {
              const std::string msg = sqlite3_errmsg(sqlite.get());
              sqlite3_close(sqlite.get());
              throw sqlpp::exception("Sqlite3 error: Can't set password to database: " + msg);
            }
          }
#endif
        }

        connection_handle(const connection_handle&) = delete;
        connection_handle(connection_handle&&) = default;
        connection_handle& operator=(const connection_handle&) = delete;
        connection_handle& operator=(connection_handle&&) = default;
        ~connection_handle() = default;
      };

      inline detail::prepared_statement_handle_t prepare_statement(detail::connection_handle& handle,
                                                                   const std::string& statement)
      {
        if (handle.config.debug)
          std::cerr << "Sqlite3 debug: Preparing: '" << statement << "'" << std::endl;

        detail::prepared_statement_handle_t result(nullptr, handle.config.debug);

        auto rc = sqlite3_prepare_v2(handle.sqlite.get(), statement.c_str(), static_cast<int>(statement.size()),
                                     &result.sqlite_statement, nullptr);

        if (rc != SQLITE_OK)
        {
          throw sqlpp::exception(
              "Sqlite3 error: Could not prepare statement: " + std::string(sqlite3_errmsg(handle.sqlite.get())) +
              " (statement was >>" + (rc == SQLITE_TOOBIG ? statement.substr(0, 128) + "..." : statement) + "<<\n");
        }

        return result;
      }

      inline void execute_statement(detail::connection_handle& handle, detail::prepared_statement_handle_t& prepared)
      {
        auto rc = sqlite3_step(prepared.sqlite_statement);
        switch (rc)
        {
          case SQLITE_OK:
          case SQLITE_ROW:  // might occur if execute is called with a select
          case SQLITE_DONE:
            return;
          default:
            if (handle.config.debug)
              std::cerr << "Sqlite3 debug: sqlite3_step return code: " << rc << std::endl;
            throw sqlpp::exception("Sqlite3 error: Could not execute statement: " +
                                   std::string(sqlite3_errmsg(handle.sqlite.get())));
        }
      }
    }  // namespace detail

    class connection;

    struct serializer_t
    {
      serializer_t(const connection& db) : _db(db), _count(1)
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

      size_t count() const
      {
        return _count;
      }

      void pop_count()
      {
        ++_count;
      }

      const connection& _db;
      sqlpp::detail::float_safe_ostringstream _os;
      size_t _count;
    };

    class SQLPP11_SQLITE3_EXPORT connection : public sqlpp::connection
    {
      detail::connection_handle _handle;
      enum class transaction_status_type
      {
        none,
        maybe,
        active
      };

      transaction_status_type _transaction_status = transaction_status_type::none;

      // direct execution
      bind_result_t select_impl(const std::string& statement)
      {
        std::unique_ptr<detail::prepared_statement_handle_t> prepared(
            new detail::prepared_statement_handle_t(prepare_statement(_handle, statement)));
        if (!prepared)
        {
          throw sqlpp::exception("Sqlite3 error: Could not store result set");
        }

        return {std::move(prepared)};
      }

      size_t insert_impl(const std::string& statement)
      {
        auto prepared = prepare_statement(_handle, statement);
        execute_statement(_handle, prepared);

        return static_cast<size_t>(sqlite3_last_insert_rowid(_handle.sqlite.get()));
      }

      size_t update_impl(const std::string& statement)
      {
        auto prepared = prepare_statement(_handle, statement);
        execute_statement(_handle, prepared);
        return static_cast<size_t>(sqlite3_changes(_handle.sqlite.get()));
      }

      size_t remove_impl(const std::string& statement)
      {
        auto prepared = prepare_statement(_handle, statement);
        execute_statement(_handle, prepared);
        return static_cast<size_t>(sqlite3_changes(_handle.sqlite.get()));
      }

      // prepared execution
      prepared_statement_t prepare_impl(const std::string& statement)
      {
        return {std::unique_ptr<detail::prepared_statement_handle_t>(
            new detail::prepared_statement_handle_t(prepare_statement(_handle, statement)))};
      }

      bind_result_t run_prepared_select_impl(prepared_statement_t& prepared_statement)
      {
        return {prepared_statement._handle};
      }

      size_t run_prepared_insert_impl(prepared_statement_t& prepared_statement)
      {
        execute_statement(_handle, *prepared_statement._handle.get());

        return static_cast<size_t>(sqlite3_last_insert_rowid(_handle.sqlite.get()));
      }

      size_t run_prepared_update_impl(prepared_statement_t& prepared_statement)
      {
        execute_statement(_handle, *prepared_statement._handle.get());

        return static_cast<size_t>(sqlite3_changes(_handle.sqlite.get()));
      }

      size_t run_prepared_remove_impl(prepared_statement_t& prepared_statement)
      {
        execute_statement(_handle, *prepared_statement._handle.get());

        return static_cast<size_t>(sqlite3_changes(_handle.sqlite.get()));
      }

      size_t run_prepared_execute_impl(prepared_statement_t& prepared_statement)
      {
        execute_statement(_handle, *prepared_statement._handle.get());

        return static_cast<size_t>(sqlite3_changes(_handle.sqlite.get()));
      }

    public:
      using _prepared_statement_t = prepared_statement_t;
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
        return ::sqlpp::serialize(t, context);
      }

      template <typename T>
      static _context_t& _interpret_interpretable(const T& t, _context_t& context)
      {
        return ::sqlpp::serialize(t, context);
      }

      connection(connection_config config) : _handle(std::move(config))
      {
      }

      connection(connection&&) noexcept = default;
      connection& operator=(connection&&) noexcept = default;

      ~connection() = default;

      connection(const connection&) = delete;
      connection& operator=(const connection&) = delete;

      //! select returns a result (which can be iterated row by row)
      template <typename Select>
      bind_result_t select(const Select& s)
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
        return prepare_impl(context.str());
      }

      template <typename PreparedSelect>
      bind_result_t run_prepared_select(const PreparedSelect& s)
      {
        s._prepared_statement._reset();
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
        return prepare_impl(context.str());
      }

      template <typename PreparedInsert>
      size_t run_prepared_insert(const PreparedInsert& i)
      {
        i._prepared_statement._reset();
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
        return prepare_impl(context.str());
      }

      template <typename PreparedUpdate>
      size_t run_prepared_update(const PreparedUpdate& u)
      {
        u._prepared_statement._reset();
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
        return prepare_impl(context.str());
      }

      template <typename PreparedRemove>
      size_t run_prepared_remove(const PreparedRemove& r)
      {
        r._prepared_statement._reset();
        r._bind_params();
        return run_prepared_remove_impl(r._prepared_statement);
      }

      //! execute arbitrary command (e.g. create a table)
      size_t execute(const std::string& statement)
      {
        auto prepared = prepare_statement(_handle, statement);
        execute_statement(_handle, prepared);
        return static_cast<size_t>(sqlite3_changes(_handle.sqlite.get()));
      }

      template <
          typename Execute,
          typename Enable = typename std::enable_if<not std::is_convertible<Execute, std::string>::value, void>::type>
      size_t execute(const Execute& x)
      {
        _context_t context(*this);
        serialize(x, context);
        return execute(context.str());
      }

      template <typename Execute>
      _prepared_statement_t prepare_execute(Execute& x)
      {
        _context_t context(*this);
        serialize(x, context);
        return prepare_impl(context.str());
      }

      template <typename PreparedExecute>
      size_t run_prepared_execute(const PreparedExecute& x)
      {
        x._prepared_statement._reset();
        x._bind_params();
        return run_prepared_execute_impl(x._prepared_statement);
      }

      //! escape given string (does not quote, though)
      std::string escape(const std::string& s) const
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
      auto _prepare(const T& t, const std::true_type&) -> decltype(t._prepare(*this))
      {
        return t._prepare(*this);
      }

      template <typename T>
      auto _prepare(const T& t, const std::false_type&) -> void;

      template <typename T>
      auto prepare(const T& t)
          -> decltype(this->_prepare(t, typename sqlpp::prepare_check_t<_serializer_context_t, T>::type{}))
      {
        (void) sqlpp::prepare_check_t<_serializer_context_t, T>{};
        return _prepare(t, sqlpp::prepare_check_t<_serializer_context_t, T>{});
      }

      //! set the transaction isolation level for this connection
      void set_default_isolation_level(isolation_level level)
      {
        if (level == sqlpp::isolation_level::read_uncommitted)
        {
          execute("pragma read_uncommitted = true");
        }
        else
        {
          execute("pragma read_uncommitted = false");
        }
      }

      //! get the currently active transaction isolation level
      sqlpp::isolation_level get_default_isolation_level()
      {
        auto stmt = prepare_statement(_handle, "pragma read_uncommitted");
        execute_statement(_handle, stmt);

        int level = sqlite3_column_int(stmt.sqlite_statement, 0);

        return level == 0 ? sqlpp::isolation_level::serializable : sqlpp::isolation_level::read_uncommitted;
      }

      //! start transaction
      void start_transaction()
      {
        if (_transaction_status == transaction_status_type::active)
        {
          throw sqlpp::exception("Sqlite3 error: Cannot have more than one open transaction per connection");
        }

        _transaction_status = transaction_status_type::maybe;
        auto prepared = prepare_statement(_handle, "BEGIN");
        execute_statement(_handle, prepared);
        _transaction_status = transaction_status_type::active;
      }

      //! commit transaction (or throw if the transaction has been finished already)
      void commit_transaction()
      {
        if (_transaction_status == transaction_status_type::none)
        {
          throw sqlpp::exception("Sqlite3 error: Cannot commit a finished or failed transaction");
        }
        _transaction_status = transaction_status_type::maybe;
        auto prepared = prepare_statement(_handle, "COMMIT");
        execute_statement(_handle, prepared);
        _transaction_status = transaction_status_type::none;
      }

      //! rollback transaction with or without reporting the rollback (or throw if the transaction has been finished
      // already)
      void rollback_transaction(bool report)
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
        auto prepared = prepare_statement(_handle, "ROLLBACK");
        execute_statement(_handle, prepared);
        _transaction_status = transaction_status_type::none;
      }

      //! report a rollback failure (will be called by transactions in case of a rollback failure in the destructor)
      void report_rollback_failure(const std::string message) noexcept
      {
        std::cerr << "Sqlite3 message:" << message << std::endl;
      }

      //! get the last inserted id
      uint64_t last_insert_id() noexcept
      {
        return static_cast<size_t>(sqlite3_last_insert_rowid(_handle.sqlite.get()));
      }

      ::sqlite3* native_handle()
      {
        return _handle.sqlite.get();
      }

      schema_t attach(const connection_config& config, const std::string name)
      {
        auto prepared =
            prepare_statement(_handle, "ATTACH '" + escape(config.path_to_database) + "' AS " + escape(name));
        execute_statement(_handle, prepared);

        return {name};
      }
    };

    inline std::string serializer_t::escape(std::string arg)
    {
      return _db.escape(arg);
    }
  }  // namespace sqlite3
}  // namespace sqlpp

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <sqlpp11/sqlite3/serializer.h>

#endif

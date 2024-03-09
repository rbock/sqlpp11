#pragma once

/*
 * Copyright (c) 2013 - 2016, Roland Bock
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
#include <sqlpp11/sqlite3/detail/connection_handle.h>
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
      inline detail::prepared_statement_handle_t prepare_statement(std::unique_ptr<connection_handle>& handle,
                                                                   const std::string& statement)
      {
        if (handle->config->debug)
          std::cerr << "Sqlite3 debug: Preparing: '" << statement << "'" << std::endl;

        detail::prepared_statement_handle_t result{nullptr, handle->config->debug};

        const char* uncompiledTail = nullptr;
        const auto rc = sqlite3_prepare_v2(handle->native_handle(), statement.c_str(),
                                           static_cast<int>(statement.size()), &result.sqlite_statement, &uncompiledTail);

        if (rc != SQLITE_OK)
        {
          throw sqlpp::exception{
              "Sqlite3 error: Could not prepare statement: " + std::string(sqlite3_errmsg(handle->native_handle())) +
              " ,statement was >>" + (rc == SQLITE_TOOBIG ? statement.substr(0, 128) + "..." : statement) + "<<\n"};
        }

        if (uncompiledTail != statement.c_str() + statement.size())
        {
          throw sqlpp::exception{"Sqlite3 connector: Cannot execute multi-statements: >>" + statement + "<<\n"};
        }

        return result;
      }

      inline void execute_statement(std::unique_ptr<connection_handle>& handle, detail::prepared_statement_handle_t& prepared)
      {
        auto rc = sqlite3_step(prepared.sqlite_statement);
        switch (rc)
        {
          case SQLITE_OK:
          case SQLITE_ROW:  // might occur if execute is called with a select
          case SQLITE_DONE:
            return;
          default:
            if (handle->config->debug)
              std::cerr << "Sqlite3 debug: sqlite3_step return code: " << rc << std::endl;
            throw sqlpp::exception{"Sqlite3 error: Could not execute statement: " +
                                   std::string(sqlite3_errmsg(handle->native_handle()))};
        }
      }
    }  // namespace detail

    // Forward declaration
    class connection_base;

    struct context_t
    {
      context_t(const connection_base& db) : _db{db}, _count{1}
      {
      }

      template <typename T>
      std::ostream& operator<<(T t)
      {
        return _os << t;
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
      size_t _count;
    };

    // Base connection class
    class SQLPP11_SQLITE3_EXPORT connection_base : public sqlpp::connection
    {
    private:
      bool _transaction_active{false};

      // direct execution
      bind_result_t select_impl(const std::string& statement)
      {
        std::unique_ptr<detail::prepared_statement_handle_t> prepared{
            new detail::prepared_statement_handle_t(prepare_statement(_handle, statement))};
        if (!prepared)
        {
          throw sqlpp::exception{"Sqlite3 error: Could not store result set"};
        }

        return {std::move(prepared)};
      }

      size_t insert_impl(const std::string& statement)
      {
        auto prepared = prepare_statement(_handle, statement);
        execute_statement(_handle, prepared);

        return static_cast<size_t>(sqlite3_last_insert_rowid(native_handle()));
      }

      size_t update_impl(const std::string& statement)
      {
        auto prepared = prepare_statement(_handle, statement);
        execute_statement(_handle, prepared);
        return static_cast<size_t>(sqlite3_changes(native_handle()));
      }

      size_t remove_impl(const std::string& statement)
      {
        auto prepared = prepare_statement(_handle, statement);
        execute_statement(_handle, prepared);
        return static_cast<size_t>(sqlite3_changes(native_handle()));
      }

      // prepared execution
      prepared_statement_t prepare_impl(const std::string& statement)
      {
        return {std::unique_ptr<detail::prepared_statement_handle_t>{
            new detail::prepared_statement_handle_t(prepare_statement(_handle, statement))}};
      }

      bind_result_t run_prepared_select_impl(prepared_statement_t& prepared_statement)
      {
        return {prepared_statement._handle};
      }

      size_t run_prepared_insert_impl(prepared_statement_t& prepared_statement)
      {
        execute_statement(_handle, *prepared_statement._handle.get());

        return static_cast<size_t>(sqlite3_last_insert_rowid(native_handle()));
      }

      size_t run_prepared_update_impl(prepared_statement_t& prepared_statement)
      {
        execute_statement(_handle, *prepared_statement._handle.get());

        return static_cast<size_t>(sqlite3_changes(native_handle()));
      }

      size_t run_prepared_remove_impl(prepared_statement_t& prepared_statement)
      {
        execute_statement(_handle, *prepared_statement._handle.get());

        return static_cast<size_t>(sqlite3_changes(native_handle()));
      }

      size_t run_prepared_execute_impl(prepared_statement_t& prepared_statement)
      {
        execute_statement(_handle, *prepared_statement._handle.get());

        return static_cast<size_t>(sqlite3_changes(native_handle()));
      }

    public:
      using _connection_base_t = connection_base;
      using _config_t = connection_config;
      using _config_ptr_t = std::shared_ptr<const connection_config>;
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

      //! select returns a result (which can be iterated row by row)
      template <typename Select>
      bind_result_t select(const Select& s)
      {
        _context_t context{*this};
        serialize(s, context);
        return select_impl(context.str());
      }

      template <typename Select>
      _prepared_statement_t prepare_select(Select& s)
      {
        _context_t context{*this};
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
        _context_t context{*this};
        serialize(i, context);
        return insert_impl(context.str());
      }

      template <typename Insert>
      _prepared_statement_t prepare_insert(Insert& i)
      {
        _context_t context{*this};
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
        _context_t context{*this};
        serialize(u, context);
        return update_impl(context.str());
      }

      template <typename Update>
      _prepared_statement_t prepare_update(Update& u)
      {
        _context_t context{*this};
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
        _context_t context{*this};
        serialize(r, context);
        return remove_impl(context.str());
      }

      template <typename Remove>
      _prepared_statement_t prepare_remove(Remove& r)
      {
        _context_t context{*this};
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

      //! Execute a single arbitrary statement (e.g. create a table)
      //! Throws an exception if multiple statements are passed (e.g. separated by semicolon).
      size_t execute(const std::string& statement)
      {
        auto prepared = prepare_statement(_handle, statement);
        execute_statement(_handle, prepared);
        return static_cast<size_t>(sqlite3_changes(native_handle()));
      }

      template <
          typename Execute,
          typename Enable = typename std::enable_if<not std::is_convertible<Execute, std::string>::value, void>::type>
      size_t execute(const Execute& x)
      {
        _context_t context{*this};
        serialize(x, context);
        return execute(context.str());
      }

      template <typename Execute>
      _prepared_statement_t prepare_execute(Execute& x)
      {
        _context_t context{*this};
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
        if (_transaction_active)
        {
          throw sqlpp::exception{"Sqlite3 error: Cannot have more than one open transaction per connection"};
        }

        auto prepared = prepare_statement(_handle, "BEGIN");
        execute_statement(_handle, prepared);
        _transaction_active = true;
      }

      //! commit transaction (or throw if the transaction has been finished already)
      void commit_transaction()
      {
        if (!_transaction_active)
        {
          throw sqlpp::exception{"Sqlite3 error: Cannot commit a finished or failed transaction"};
        }
        auto prepared = prepare_statement(_handle, "COMMIT");
        execute_statement(_handle, prepared);
        _transaction_active = false;
      }

      //! rollback transaction with or without reporting the rollback (or throw if the transaction has been finished
      // already)
      void rollback_transaction(bool report)
      {
        if (!_transaction_active)
        {
          throw sqlpp::exception{"Sqlite3 error: Cannot rollback a finished or failed transaction"};
        }
        if (report)
        {
          std::cerr << "Sqlite3 warning: Rolling back unfinished transaction" << std::endl;
        }
        auto prepared = prepare_statement(_handle, "ROLLBACK");
        execute_statement(_handle, prepared);
        _transaction_active = false;
      }

      //! report a rollback failure (will be called by transactions in case of a rollback failure in the destructor)
      void report_rollback_failure(const std::string& message) noexcept
      {
        std::cerr << "Sqlite3 message:" << message << std::endl;
      }

      //! check if transaction is active
      bool is_transaction_active()
      {
        return _transaction_active;
      }

      //! get the last inserted id
      uint64_t last_insert_id() noexcept
      {
        return static_cast<size_t>(sqlite3_last_insert_rowid(native_handle()));
      }

      ::sqlite3* native_handle() const
      {
        return _handle->native_handle();
      }

      schema_t attach(const connection_config& config, const std::string& name)
      {
        auto prepared =
            prepare_statement(_handle, "ATTACH '" + escape(config.path_to_database) + "' AS " + escape(name));
        execute_statement(_handle, prepared);

        return {name};
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
  }  // namespace sqlite3
}  // namespace sqlpp

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <sqlpp11/sqlite3/serializer.h>

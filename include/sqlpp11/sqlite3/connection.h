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
#include <sqlpp11/schema.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/sqlite3/bind_result.h>
#include <sqlpp11/sqlite3/connection_config.h>
#include <sqlpp11/sqlite3/prepared_statement.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/sqlite3/export.h>
#include <sstream>
#include <string>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace sqlpp
{
  namespace sqlite3
  {
    namespace detail
    {
      struct connection_handle;
    }

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
      std::stringstream _os;
      size_t _count;
    };

    class SQLPP11_SQLITE3_EXPORT connection : public sqlpp::connection
    {
      std::unique_ptr<detail::connection_handle> _handle;
      enum class transaction_status_type
      {
        none,
        maybe,
        active
      };

      transaction_status_type _transaction_status = transaction_status_type::none;

      // direct execution
      bind_result_t select_impl(const std::string& statement);
      size_t insert_impl(const std::string& statement);
      size_t update_impl(const std::string& statement);
      size_t remove_impl(const std::string& statement);

      // prepared execution
      prepared_statement_t prepare_impl(const std::string& statement);
      bind_result_t run_prepared_select_impl(prepared_statement_t& prepared_statement);
      size_t run_prepared_execute_impl(prepared_statement_t& prepared_statement);
      size_t run_prepared_insert_impl(prepared_statement_t& prepared_statement);
      size_t run_prepared_update_impl(prepared_statement_t& prepared_statement);
      size_t run_prepared_remove_impl(prepared_statement_t& prepared_statement);

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

      connection(connection_config config);
      connection(connection&&) noexcept;
      connection& operator=(connection&&) noexcept;
      ~connection();
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
      size_t execute(const std::string& command);

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
      std::string escape(const std::string& s) const;

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
        sqlpp::prepare_check_t<_serializer_context_t, T>{};
        return _prepare(t, sqlpp::prepare_check_t<_serializer_context_t, T>{});
      }

      //! set the transaction isolation level for this connection
      void set_default_isolation_level(isolation_level level);

      //! get the currently active transaction isolation level
      isolation_level get_default_isolation_level();

      //! start transaction
      void start_transaction();

      //! commit transaction (or throw if the transaction has been finished already)
      void commit_transaction();

      //! rollback transaction with or without reporting the rollback (or throw if the transaction has been finished
      // already)
      void rollback_transaction(bool report);

      //! report a rollback failure (will be called by transactions in case of a rollback failure in the destructor)
      void report_rollback_failure(const std::string message) noexcept;

      //! get the last inserted id
      uint64_t last_insert_id() noexcept;

      ::sqlite3* native_handle();

      auto attach(const connection_config&, const std::string name) -> schema_t;
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

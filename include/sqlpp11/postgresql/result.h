/**
 * Copyright © 2015-2016, Bartosz Wieczorek
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

#ifndef SQLPP_POSTGRESQL_RESULT_H
#define SQLPP_POSTGRESQL_RESULT_H

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include <libpq-fe.h>

#include <sqlpp11/postgresql/visibility.h>
#include <sqlpp11/postgresql/exception.h>

#ifdef SQLPP_DYNAMIC_LOADING
#include <sqlpp11/postgresql/dynamic_libpq.h>
#endif

namespace sqlpp
{
  namespace postgresql
  {
#ifdef SQLPP_DYNAMIC_LOADING
    using namespace dynamic;
#endif

    class DLL_PUBLIC Result
    {
    public:
      Result();
      ~Result();

      ExecStatusType status();

      void clear();

      int affected_rows();
      int records_size() const;
      int field_count() const;
      int length(int record, int field) const;
      bool isNull(int record, int field) const;
      void operator=(PGresult* res);
      operator bool() const;

      template <typename T = const char*>
      inline T getValue(int record, int field) const
      {
        static_assert(std::is_arithmetic<T>::value, "Value must be numeric type");
        checkIndex(record, field);
        T t(0);
        auto txt = std::string(getPqValue(m_result, record, field));
        if(txt != "")
        {
          t = std::stold(txt);
        }

        return t;
      }

      const std::string& query() const
      {
        return m_query;
      }

      std::string& query()
      {
        return m_query;
      }

    private:
      void CheckStatus() const;
      [[noreturn]] void ThrowSQLError(const std::string& Err, const std::string& Query) const;
      std::string StatusError() const;
      int errorPosition() const noexcept;
      bool hasError();
      void checkIndex(int record, int field) const noexcept(false);

      // move PQgetvalue to implementation so we don't depend on the libpq in the
      // public interface
      const char* getPqValue(PGresult* result, int record, int field) const;

      PGresult* m_result;
      std::string m_query;
    };

    template <>
    inline const char* Result::getValue<const char*>(int record, int field) const
    {
      return const_cast<const char*>(getPqValue(m_result, record, field));
    }

    template <>
    inline std::string Result::getValue<std::string>(int record, int field) const
    {
      return {getValue<const char*>(record, field)};
    }

    template <>
    inline bool Result::getValue<bool>(int record, int field) const
    {
      checkIndex(record, field);
      auto val = getPqValue(m_result, record, field);
      if (*val == 't')
        return true;
      else if (*val == 'f')
        return false;
      return const_cast<const char*>(val);
    }


    template <>
    inline const uint8_t* Result::getValue<const uint8_t*>(int record, int field) const
    {
      return reinterpret_cast<const uint8_t*>(getValue<const char*>(record, field));
    }

    inline Result::Result() : m_result(nullptr)
    {
    }

    inline void Result::checkIndex(int record, int field) const noexcept(false)
    {
      if (record > records_size() || field > field_count())
        throw std::out_of_range("PostgreSQL error: index out of range");
    }

    inline void Result::operator=(PGresult* res)
    {
      m_result = res;
      CheckStatus();
    }

    inline void Result::CheckStatus() const
    {
      const std::string Err = StatusError();
      if (!Err.empty())
        ThrowSQLError(Err, query());
    }

    inline const char* Result::getPqValue(PGresult* result, int record, int field) const
    {
      return const_cast<const char*>(PQgetvalue(result, record, field));
    }

    [[noreturn]] inline void Result::ThrowSQLError(const std::string& Err, const std::string& Query) const
    {
      // Try to establish more precise error type, and throw corresponding exception
      const char* const code = PQresultErrorField(m_result, PG_DIAG_SQLSTATE);
      if (code)
        switch (code[0])
        {
          case '0':
            switch (code[1])
            {
              case '8':
                throw broken_connection(Err);
              case 'A':
                throw feature_not_supported(Err, Query);
            }
            break;
          case '2':
            switch (code[1])
            {
              case '2':
                throw data_exception(Err, Query);
              case '3':
                if (strcmp(code, "23001") == 0)
                  throw restrict_violation(Err, Query);
                if (strcmp(code, "23502") == 0)
                  throw not_null_violation(Err, Query);
                if (strcmp(code, "23503") == 0)
                  throw foreign_key_violation(Err, Query);
                if (strcmp(code, "23505") == 0)
                  throw unique_violation(Err, Query);
                if (strcmp(code, "23514") == 0)
                  throw check_violation(Err, Query);
                throw integrity_constraint_violation(Err, Query);
              case '4':
                throw invalid_cursor_state(Err, Query);
              case '6':
                throw invalid_sql_statement_name(Err, Query);
            }
            break;
          case '3':
            switch (code[1])
            {
              case '4':
                throw invalid_cursor_name(Err, Query);
            }
            break;
          case '4':
            switch (code[1])
            {
              case '2':
                if (strcmp(code, "42501") == 0)
                  throw insufficient_privilege(Err, Query);
                if (strcmp(code, "42601") == 0)
                  throw syntax_error(Err, Query, errorPosition());
                if (strcmp(code, "42703") == 0)
                  throw undefined_column(Err, Query);
                if (strcmp(code, "42883") == 0)
                  throw undefined_function(Err, Query);
                if (strcmp(code, "42P01") == 0)
                  throw undefined_table(Err, Query);
            }
            break;
          case '5':
            switch (code[1])
            {
              case '3':
                if (strcmp(code, "53100") == 0)
                  throw disk_full(Err, Query);
                if (strcmp(code, "53200") == 0)
                  throw out_of_memory(Err, Query);
                if (strcmp(code, "53300") == 0)
                  throw too_many_connections(Err);
                throw insufficient_resources(Err, Query);
            }
            break;

          case 'P':
            if (strcmp(code, "P0001") == 0)
              throw plpgsql_raise(Err, Query);
            if (strcmp(code, "P0002") == 0)
              throw plpgsql_no_data_found(Err, Query);
            if (strcmp(code, "P0003") == 0)
              throw plpgsql_too_many_rows(Err, Query);
            throw plpgsql_error(Err, Query);
        }
      throw sql_error(Err, Query);
    }

    inline std::string Result::StatusError() const
    {
      if (!m_result)
        throw failure("No result set given");

      std::string Err;

      switch (PQresultStatus(m_result))
      {
        case PGRES_EMPTY_QUERY:  // The string sent to the backend was empty.
        case PGRES_COMMAND_OK:   // Successful completion of a command returning no data
        case PGRES_TUPLES_OK:    // The query successfully executed
          break;

        case PGRES_COPY_OUT:  // Copy Out (from server) data transfer started
        case PGRES_COPY_IN:   // Copy In (to server) data transfer started
          break;

        case PGRES_BAD_RESPONSE:  // The server's response was not understood
        case PGRES_NONFATAL_ERROR:
        case PGRES_FATAL_ERROR:
          Err = PQresultErrorMessage(m_result);
          break;
        case PGRES_COPY_BOTH:
        case PGRES_SINGLE_TUPLE:
          throw sqlpp::exception("pqxx::result: Unrecognized response code " +
                                 std::to_string(PQresultStatus(m_result)));
      }
      return Err;
    }

    inline int Result::errorPosition() const noexcept
    {
      int pos = -1;
      if (m_result)
      {
        const char* p = PQresultErrorField(m_result, PG_DIAG_STATEMENT_POSITION);
        if (p)
          pos = std::stoi(std::string(p));
      }
      return pos;
    }

    inline sqlpp::postgresql::Result::operator bool() const
    {
      return m_result != 0;
    }

    inline void Result::clear()
    {
      if (m_result)
        PQclear(m_result);
      m_result = nullptr;
    }

    inline int Result::affected_rows()
    {
      const char* const RowsStr = PQcmdTuples(m_result);
      return RowsStr[0] ? std::stoi(std::string(RowsStr)) : 0;
    }

    inline int Result::records_size() const
    {
      return m_result ? PQntuples(m_result) : 0;
    }

    inline int Result::field_count() const
    {
      return m_result ? PQnfields(m_result) : 0;
    }

    inline bool Result::isNull(int record, int field) const
    {
      /// check index?
      return PQgetisnull(m_result, record, field);
    }

    inline int Result::length(int record, int field) const
    {
      /// check index?
      return PQgetlength(m_result, record, field);
    }

    inline Result::~Result()
    {
      clear();
    }

    inline ExecStatusType Result::status()
    {
      return PQresultStatus(m_result);
    }
  }
}

#endif

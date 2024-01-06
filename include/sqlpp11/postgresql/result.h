#pragma once

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

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include <pg_config.h>
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
      Result() : m_result(nullptr)
      {
      }

      ~Result()
      {
        clear();
      }

      ExecStatusType status()
      {
        return PQresultStatus(m_result);
      }

      void clear()
      {
        if (m_result)
          PQclear(m_result);
        m_result = nullptr;
      }

      int affected_rows()
      {
        const char* const rows_str = PQcmdTuples(m_result);
        return rows_str[0] ? std::stoi(std::string{rows_str}) : 0;
      }

      int records_size() const
      {
        return m_result ? PQntuples(m_result) : 0;
      }

      int field_count() const
      {
        return m_result ? PQnfields(m_result) : 0;
      }

      int length(int record, int field) const
      {
        /// check index?
        return PQgetlength(m_result, record, field);
      }

      bool is_null(int record, int field) const
      {
        /// check index?
        return PQgetisnull(m_result, record, field);
      }

      void operator=(PGresult* res)
      {
        m_result = res;
        check_status();
      }

      operator bool() const
      {
        return m_result != 0;
      }

      int64_t get_int64_value(int record, int field) const
      {
        check_index(record, field);
        auto t = int64_t{};
        const auto txt = std::string{get_pq_value(m_result, record, field)};
        if(txt != "")
        {
          t = std::stoll(txt);
        }

        return t;
      }

      uint64_t get_uint64_value(int record, int field) const
      {
        check_index(record, field);
        auto t = uint64_t{};
        const auto txt = std::string{get_pq_value(m_result, record, field)};
        if(txt != "")
        {
          t = std::stoull(txt);
        }

        return t;
      }

      double get_double_value(int record, int field) const
      {
        check_index(record, field);
        auto t = double{};
        auto txt = std::string{get_pq_value(m_result, record, field)};
        if(txt != "")
        {
          t = std::stod(txt);
        }

        return t;
      }

      const char* get_char_ptr_value(int record, int field) const
      {
        return const_cast<const char*>(get_pq_value(m_result, record, field));
      }

      std::string get_string_value(int record, int field) const
      {
        return {get_char_ptr_value(record, field)};
      }

      const uint8_t* get_blob_value(int record, int field) const
      {
        return reinterpret_cast<const uint8_t*>(get_pq_value(m_result, record, field));
      }

      bool get_bool_value(int record, int field) const
      {
        check_index(record, field);
        auto val = get_pq_value(m_result, record, field);
        if (*val == 't')
          return true;
        else if (*val == 'f')
          return false;
        return const_cast<const char*>(val);
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
      void check_status() const
      {
        const std::string err = status_error();
        if (!err.empty())
          throw_sql_error(err, query());
      }

      [[noreturn]] void throw_sql_error(const std::string& err, const std::string& query) const
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
                  throw broken_connection{err};
                case 'A':
                  throw feature_not_supported{err, query};
              }
              break;
            case '2':
              switch (code[1])
              {
                case '2':
                  throw data_exception{err, query};
                case '3':
                  if (strcmp(code, "23001") == 0)
                    throw restrict_violation{err, query};
                  if (strcmp(code, "23502") == 0)
                    throw not_null_violation{err, query};
                  if (strcmp(code, "23503") == 0)
                    throw foreign_key_violation{err, query};
                  if (strcmp(code, "23505") == 0)
                    throw unique_violation{err, query};
                  if (strcmp(code, "23514") == 0)
                    throw check_violation{err, query};
                  throw integrity_constraint_violation{err, query};
                case '4':
                  throw invalid_cursor_state{err, query};
                case '6':
                  throw invalid_sql_statement_name{err, query};
              }
              break;
            case '3':
              switch (code[1])
              {
                case '4':
                  throw invalid_cursor_name{err, query};
              }
              break;
            case '4':
              switch (code[1])
              {
                case '0':
                  if (strcmp(code, "40001") == 0)
                    throw serialization_failure{err, query};
                  if (strcmp(code, "40P01") == 0)
                    throw deadlock_detected{err, query};
                  break;
                case '2':
                  if (strcmp(code, "42501") == 0)
                    throw insufficient_privilege{err, query};
                  if (strcmp(code, "42601") == 0)
                    throw syntax_error{err, query, error_position()};
                  if (strcmp(code, "42703") == 0)
                    throw undefined_column{err, query};
                  if (strcmp(code, "42883") == 0)
                    throw undefined_function{err, query};
                  if (strcmp(code, "42P01") == 0)
                    throw undefined_table{err, query};
              }
              break;
            case '5':
              switch (code[1])
              {
                case '3':
                  if (strcmp(code, "53100") == 0)
                    throw disk_full{err, query};
                  if (strcmp(code, "53200") == 0)
                    throw out_of_memory{err, query};
                  if (strcmp(code, "53300") == 0)
                    throw too_many_connections{err};
                  throw insufficient_resources{err, query};
              }
              break;

            case 'P':
              if (strcmp(code, "P0001") == 0)
                throw plpgsql_raise{err, query};
              if (strcmp(code, "P0002") == 0)
                throw plpgsql_no_data_found{err, query};
              if (strcmp(code, "P0003") == 0)
                throw plpgsql_too_many_rows{err, query};
              throw plpgsql_error{err, query};
              break;
            default:
              throw sql_user_error{err, query, code};
          }
        throw sql_error{err, query};
      }

      std::string status_error() const
      {
        if (!m_result)
          throw failure{"No result set given"};

        std::string err;

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
            err = PQresultErrorMessage(m_result);
            break;
  #if PG_MAJORVERSION_NUM >= 13
          case PGRES_COPY_BOTH:
          case PGRES_SINGLE_TUPLE:
  #endif
  #if PG_MAJORVERSION_NUM >= 14
          case PGRES_PIPELINE_SYNC:
          case PGRES_PIPELINE_ABORTED:
  #endif
          default:
            throw sqlpp::exception{"pqxx::result: Unrecognized response code " +
                                  std::to_string(PQresultStatus(m_result))};
        }
        return err;
      }

      int error_position() const noexcept
      {
        int pos = -1;
        if (m_result)
        {
          const char* p = PQresultErrorField(m_result, PG_DIAG_STATEMENT_POSITION);
          if (p)
            pos = std::stoi(std::string{p});
        }
        return pos;
      }

      void check_index(int record, int field) const noexcept(false)
      {
        if (record > records_size() || field > field_count())
          throw std::out_of_range{"PostgreSQL error: index out of range"};
      }

      // move PQgetvalue to implementation so we don't depend on the libpq in the
      // public interface
      const char* get_pq_value(PGresult* result, int record, int field) const
      {
        return const_cast<const char*>(PQgetvalue(result, record, field));
      }

      PGresult* m_result;
      std::string m_query;
    };
  }
}

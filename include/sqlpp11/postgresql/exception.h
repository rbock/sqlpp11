#pragma once

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

/**
  classes copied from http://pqxx.org/devprojects/libpqxx/doc/3.1/html/Reference/a00032.html
*/

#include "visibility.h"
#include <sqlpp11/exception.h>
#include <string>

namespace sqlpp
{
  namespace postgresql
  {
    /**
     * @addtogroup exception Exception classes
     *
     * These exception classes follow, roughly, the two-level hierarchy defined by
     * the PostgreSQL error codes (see Appendix A of the PostgreSQL documentation
     * corresponding to your server version).  The hierarchy given here is, as yet,
     * not a complete mirror of the error codes.  There are some other differences
     * as well, e.g. the error code statement_completion_unknown has a separate
     * status in libpqxx as in_doubt_error, and too_many_connections is classified
     * as a broken_connection rather than a subtype of insufficient_resources.
     *
     * @see http://www.postgresql.org/docs/8.1/interactive/errcodes-appendix.html
     *
     * @{
     */

    /// Run-time failure encountered by sqlpp::postgresql connector, similar to std::runtime_error
    class DLL_PUBLIC failure : public ::sqlpp::exception
    {
      virtual const std::exception& base() const noexcept
      {
        return *this;
      }

    public:
      explicit failure(std::string whatarg) : sqlpp::exception{std::move(whatarg)}
      {
      }
    };

    /// Exception class for lost or failed backend connection.
    /**
     * @warning When this happens on Unix-like systems, you may also get a SIGPIPE
     * signal.  That signal aborts the program by default, so if you wish to be able
     * to continue after a connection breaks, be sure to disarm this signal.
     *
     * If you're working on a Unix-like system, see the manual page for
     * @c signal (2) on how to deal with SIGPIPE.  The easiest way to make this
     * signal harmless is to make your program ignore it:
     *
     * @code
     * #include <signal.h>
     *
     * int main()
     * {
     *   signal(SIGPIPE, SIG_IGN);
     *   // ...
     * @endcode
     */
    class DLL_PUBLIC broken_connection : public failure
    {
    public:
      broken_connection() : failure{"Connection to database failed"}
      {
      }

      explicit broken_connection(std::string whatarg) : failure{std::move(whatarg)}
      {
      }
    };

    /// Exception class for failed queries.
    /** Carries a copy of the failed query in addition to a regular error message */
    class DLL_PUBLIC sql_error : public failure
    {
      std::string m_Q;

    public:
      sql_error() : failure{"Failed query"}, m_Q{}
      {
      }
      explicit sql_error(std::string whatarg) : failure{std::move(whatarg)}, m_Q{}
      {
      }
      sql_error(std::string whatarg, std::string Q) : failure{std::move(whatarg)}, m_Q{std::move(Q)}
      {
      }

      /// The query whose execution triggered the exception
      const std::string& query() const noexcept
      {
        return m_Q;
      }
    };

    /** Any error not covered by standard errors.
     * For example custom error code from a user
     * defined pl/pgsql function
     *
     */
    class DLL_PUBLIC sql_user_error : public sql_error
    {
      std::string m_Code;

    public:
      sql_user_error(std::string whatarg, std::string code) : sql_error{std::move(whatarg)}, m_Code{std::move(code)}
      {
      }
      sql_user_error(std::string whatarg, std::string query, std::string code) : sql_error{std::move(whatarg),std::move(query)}, m_Code{std::move(code)}
      {
      }

      /// The code so the code raised
      const std::string& code() const noexcept
      {
        return m_Code;
      }
    };

    // TODO: should this be called statement_completion_unknown!?
    /// "Help, I don't know whether transaction was committed successfully!"
    /** Exception that might be thrown in rare cases where the connection to the
     * database is lost while finishing a database transaction, and there's no way
     * of telling whether it was actually executed by the backend.  In this case
     * the database is left in an indeterminate (but consistent) state, and only
     * manual inspection will tell which is the case.
     */
    class DLL_PUBLIC in_doubt_error : public failure
    {
    public:
      explicit in_doubt_error(std::string whatarg) : failure{std::move(whatarg)}
      {
      }
    };

    /// Database feature not supported in current setup
    class DLL_PUBLIC feature_not_supported : public sql_error
    {
    public:
      explicit feature_not_supported(std::string err) : sql_error{std::move(err)}
      {
      }
      feature_not_supported(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    /// Error in data provided to SQL statement
    class DLL_PUBLIC data_exception : public sql_error
    {
    public:
      explicit data_exception(std::string err) : sql_error{std::move(err)}
      {
      }
      data_exception(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC integrity_constraint_violation : public sql_error
    {
    public:
      explicit integrity_constraint_violation(std::string err) : sql_error{std::move(err)}
      {
      }
      integrity_constraint_violation(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC restrict_violation : public integrity_constraint_violation
    {
    public:
      explicit restrict_violation(std::string err) : integrity_constraint_violation(std::move(err))
      {
      }
      restrict_violation(std::string err, std::string Q) : integrity_constraint_violation(std::move(err), std::move(Q))
      {
      }
    };

    class DLL_PUBLIC not_null_violation : public integrity_constraint_violation
    {
    public:
      explicit not_null_violation(std::string err) : integrity_constraint_violation(std::move(err))
      {
      }
      not_null_violation(std::string err, std::string Q) : integrity_constraint_violation(std::move(err), std::move(Q))
      {
      }
    };

    class DLL_PUBLIC foreign_key_violation : public integrity_constraint_violation
    {
    public:
      explicit foreign_key_violation(std::string err) : integrity_constraint_violation(std::move(err))
      {
      }
      foreign_key_violation(std::string err, std::string Q)
          : integrity_constraint_violation(std::move(err), std::move(Q))
      {
      }
    };

    class DLL_PUBLIC unique_violation : public integrity_constraint_violation
    {
    public:
      explicit unique_violation(std::string err) : integrity_constraint_violation(std::move(err))
      {
      }
      unique_violation(std::string err, std::string Q) : integrity_constraint_violation(std::move(err), std::move(Q))
      {
      }
    };

    class DLL_PUBLIC check_violation : public integrity_constraint_violation
    {
    public:
      explicit check_violation(std::string err) : integrity_constraint_violation(std::move(err))
      {
      }
      check_violation(std::string err, std::string Q) : integrity_constraint_violation(std::move(err), std::move(Q))
      {
      }
    };

    class DLL_PUBLIC invalid_cursor_state : public sql_error
    {
    public:
      explicit invalid_cursor_state(std::string err) : sql_error{std::move(err)}
      {
      }
      invalid_cursor_state(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC invalid_sql_statement_name : public sql_error
    {
    public:
      explicit invalid_sql_statement_name(std::string err) : sql_error{std::move(err)}
      {
      }
      invalid_sql_statement_name(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC invalid_cursor_name : public sql_error
    {
    public:
      explicit invalid_cursor_name(std::string err) : sql_error{std::move(err)}
      {
      }
      invalid_cursor_name(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC serialization_failure : public sql_error
    {
    public:
      explicit serialization_failure(std::string err) : sql_error{std::move(err)}
      {
      }
      serialization_failure(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC deadlock_detected : public sql_error
    {
    public:
      explicit deadlock_detected(std::string err) : sql_error{std::move(err)}
      {
      }
      deadlock_detected(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC syntax_error : public sql_error
    {
    public:
      /// Approximate position in string where error occurred, or -1 if unknown.
      const int error_position;

      explicit syntax_error(std::string err, int pos = -1) : sql_error{std::move(err)}, error_position{pos}
      {
      }
      syntax_error(std::string err, std::string Q, int pos = -1) : sql_error{std::move(err), std::move(Q)}, error_position{pos}
      {
      }
    };

    class DLL_PUBLIC undefined_column : public syntax_error
    {
    public:
      explicit undefined_column(std::string err) : syntax_error{std::move(err)}
      {
      }
      undefined_column(std::string err, std::string Q) : syntax_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC undefined_function : public syntax_error
    {
    public:
      explicit undefined_function(std::string err) : syntax_error{std::move(err)}
      {
      }
      undefined_function(std::string err, std::string Q) : syntax_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC undefined_table : public syntax_error
    {
    public:
      explicit undefined_table(std::string err) : syntax_error{std::move(err)}
      {
      }
      undefined_table(std::string err, std::string Q) : syntax_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC insufficient_privilege : public sql_error
    {
    public:
      explicit insufficient_privilege(std::string err) : sql_error{std::move(err)}
      {
      }
      insufficient_privilege(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    /// Resource shortage on the server
    class DLL_PUBLIC insufficient_resources : public sql_error
    {
    public:
      explicit insufficient_resources(std::string err) : sql_error{std::move(err)}
      {
      }
      insufficient_resources(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC disk_full : public insufficient_resources
    {
    public:
      explicit disk_full(std::string err) : insufficient_resources{std::move(err)}
      {
      }
      disk_full(std::string err, std::string Q) : insufficient_resources{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC out_of_memory : public insufficient_resources
    {
    public:
      explicit out_of_memory(std::string err) : insufficient_resources{std::move(err)}
      {
      }
      out_of_memory(std::string err, std::string Q) : insufficient_resources{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC too_many_connections : public broken_connection
    {
    public:
      explicit too_many_connections(std::string err) : broken_connection{std::move(err)}
      {
      }
    };

    /// PL/pgSQL error
    /** Exceptions derived from this class are errors from PL/pgSQL procedures.*/
    class DLL_PUBLIC plpgsql_error : public sql_error
    {
    public:
      explicit plpgsql_error(std::string err) : sql_error{std::move(err)}
      {
      }
      plpgsql_error(std::string err, std::string Q) : sql_error{std::move(err), std::move(Q)}
      {
      }
    };

    /// Exception raised in PL/pgSQL procedure
    class DLL_PUBLIC plpgsql_raise : public plpgsql_error
    {
    public:
      explicit plpgsql_raise(std::string err) : plpgsql_error{std::move(err)}
      {
      }
      plpgsql_raise(std::string err, std::string Q) : plpgsql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC plpgsql_no_data_found : public plpgsql_error
    {
    public:
      explicit plpgsql_no_data_found(std::string err) : plpgsql_error{std::move(err)}
      {
      }
      plpgsql_no_data_found(std::string err, std::string Q) : plpgsql_error{std::move(err), std::move(Q)}
      {
      }
    };

    class DLL_PUBLIC plpgsql_too_many_rows : public plpgsql_error
    {
    public:
      explicit plpgsql_too_many_rows(std::string err) : plpgsql_error{std::move(err)}
      {
      }
      plpgsql_too_many_rows(std::string err, std::string Q) : plpgsql_error{std::move(err), std::move(Q)}
      {
      }
    };
  }
}

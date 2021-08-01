/*
 * Copyright (c) 2013 - 2015, Roland Bock
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

#ifndef SQLPP_SQLITE3_DETAIL_PREPARED_STATEMENT_HANDLE_H
#define SQLPP_SQLITE3_DETAIL_PREPARED_STATEMENT_HANDLE_H

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif

#ifdef SQLPP_DYNAMIC_LOADING
#include <sqlpp11/sqlite3/dynamic_libsqlite3.h>
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
      struct prepared_statement_handle_t
      {
        sqlite3_stmt* sqlite_statement;
        bool debug;

        prepared_statement_handle_t(sqlite3_stmt* statement, bool debug_) : sqlite_statement(statement), debug(debug_)
        {
        }

        prepared_statement_handle_t(const prepared_statement_handle_t&) = delete;
        prepared_statement_handle_t(prepared_statement_handle_t&& rhs)
        {
          sqlite_statement = rhs.sqlite_statement;
          rhs.sqlite_statement = nullptr;

          debug = rhs.debug;
        }
        prepared_statement_handle_t& operator=(const prepared_statement_handle_t&) = delete;
        prepared_statement_handle_t& operator=(prepared_statement_handle_t&& rhs)
        {
          if (sqlite_statement != rhs.sqlite_statement)
          {
            sqlite_statement = rhs.sqlite_statement;
            rhs.sqlite_statement = nullptr;
          }
          debug = rhs.debug;

          return *this;
        }

        ~prepared_statement_handle_t()
        {
          if (sqlite_statement)
          {
            sqlite3_finalize(sqlite_statement);
          }
        }

        bool operator!() const
        {
          return !sqlite_statement;
        }
      };
    }  // namespace detail
  }    // namespace sqlite3
}  // namespace sqlpp

#endif

#pragma once

/**
 * Copyright © 2014-2015, Matthijs Möhlmann
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
#include <random>
#include <string>
#include <vector>

#include <libpq-fe.h>
#include <sqlpp11/postgresql/result.h>
#include <sqlpp11/postgresql/visibility.h>

#include "connection_handle.h"

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
    namespace detail
    {
      struct DLL_PUBLIC statement_handle_t
      {
        detail::connection_handle& connection;
        Result result;
        bool valid = false;
        int count = 0;
        int totalCount = 0;
        int fields = 0;

        // ctor
        statement_handle_t(detail::connection_handle& _connection);
        statement_handle_t(const statement_handle_t&) = delete;
        statement_handle_t(statement_handle_t&&) = delete;
        statement_handle_t& operator=(const statement_handle_t&) = delete;
        statement_handle_t& operator=(statement_handle_t&&) = delete;

        virtual ~statement_handle_t();
        bool operator!() const;
        void clearResult();

        bool debug() const;
      };

      struct prepared_statement_handle_t : public statement_handle_t
      {
      private:
        std::string _name{"xxxxxx"};

      public:
        // Store prepared statement arguments
        std::vector<bool> nullValues;
        std::vector<std::string> paramValues;

        // ctor
        prepared_statement_handle_t(detail::connection_handle& _connection, std::string stmt, const size_t& paramCount);
        prepared_statement_handle_t(const prepared_statement_handle_t&) = delete;
        prepared_statement_handle_t(prepared_statement_handle_t&&) = delete;
        prepared_statement_handle_t& operator=(const prepared_statement_handle_t&) = delete;
        prepared_statement_handle_t& operator=(prepared_statement_handle_t&&) = delete;

        virtual ~prepared_statement_handle_t();

        void execute();

        std::string name() const
        {
          return _name;
        }

      private:
        void generate_name();
        void prepare(std::string stmt);
      };

      inline statement_handle_t::statement_handle_t(connection_handle& _connection) : connection(_connection)
      {
      }

      inline statement_handle_t::~statement_handle_t()
      {
        clearResult();
      }

      inline bool statement_handle_t::operator!() const
      {
        return !valid;
      }

      inline void statement_handle_t::clearResult()
      {
        if (result)
        {
          result.clear();
        }
      }

      inline bool statement_handle_t::debug() const
      {
        return connection.config->debug;
      }

      inline prepared_statement_handle_t::prepared_statement_handle_t(connection_handle& _connection,
                                                               std::string stmt,
                                                               const size_t& paramCount)
          : statement_handle_t(_connection), nullValues(paramCount), paramValues(paramCount)
      {
        generate_name();
        prepare(std::move(stmt));
      }

      inline prepared_statement_handle_t::~prepared_statement_handle_t()
      {
        if (valid && !_name.empty())
        {
          connection.deallocate_prepared_statement(_name);
        }
      }

      inline void prepared_statement_handle_t::execute()
      {
        const size_t size = paramValues.size();

        std::vector<const char*> values;
        for (size_t i = 0u; i < size; i++)
          values.push_back(nullValues[i] ? nullptr : const_cast<char*>(paramValues[i].c_str()));

        // Execute prepared statement with the parameters.
        clearResult();
        valid = false;
        count = 0;
        totalCount = 0;
        result = PQexecPrepared(connection.native_handle(), _name.data(), static_cast<int>(size), values.data(), nullptr, nullptr, 0);
		/// @todo validate result? is it really valid
        valid = true;
      }

      inline void prepared_statement_handle_t::generate_name()
      {
        // Generate a random name for the prepared statement
        while (connection.prepared_statement_names.find(_name) != connection.prepared_statement_names.end())
        {
          std::generate_n(_name.begin(), 6, []() {
            constexpr static auto charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                            "abcdefghijklmnopqrstuvwxyz";
            constexpr size_t max = (sizeof(charset) - 1);
            std::random_device rd;
            return charset[rd() % max];
          });
        }
        connection.prepared_statement_names.insert(_name);
      }

      inline void prepared_statement_handle_t::prepare(std::string stmt)
      {
        // Create the prepared statement
        result = PQprepare(connection.native_handle(), _name.c_str(), stmt.c_str(), 0, nullptr);
        valid = true;
      }
    }
  }
}

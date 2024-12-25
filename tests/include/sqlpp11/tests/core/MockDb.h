#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <sqlpp11/core/database/connection.h>
#include <sqlpp11/core/database/transaction.h>
#include <sqlpp11/core/basic/schema.h>
#include <sqlpp11/core/to_sql_string.h>
#include <sqlpp11/core/database/serializer_context.h>
#include <sstream>

// an object to store internal Mock flags and values to validate in tests
struct InternalMockData
{
  sqlpp::isolation_level _last_isolation_level;
  sqlpp::isolation_level _default_isolation_level;
};

struct MockDb : public sqlpp::connection
{
  struct _context_t
  {
  };

  class result_t
  {
  public:
    constexpr bool operator==(const result_t&) const
    {
      return true;
    }

    template <typename ResultRow>
    void next(ResultRow& result_row)
    {
      result_row._invalidate();
    }
  };

  // Directly executed statements start here
  template <typename T>
  auto _run(const T& t, ::sqlpp::consistent_t) -> decltype(t._run(*this))
  {
    return t._run(*this);
  }

  template <typename Check, typename T>
  auto _run(const T& t, Check) -> Check;

  template <typename T>
  auto operator()(const T& t) -> decltype(this->_run(t, sqlpp::run_check_t<_context_t, T>{}))
  {
    return _run(t, sqlpp::run_check_t<_context_t, T>{});
  }

  size_t execute(const std::string&)
  {
    return 0;
  }

  template <
      typename Statement,
      typename Enable = typename std::enable_if<not std::is_convertible<Statement, std::string>::value, void>::type>
  size_t execute(const Statement& x)
  {
    _context_t context;
    const auto query = to_sql_string(context, x);
    std::cout << "Running execute call with\n" << query << std::endl;
    return execute(query);
  }

  template <typename Insert>
  size_t insert(const Insert& x)
  {
    _context_t context;
    const auto query = to_sql_string(context, x);
    std::cout << "Running insert call with\n" << query << std::endl;
    return 0;
  }

  template <typename Update>
  size_t update(const Update& x)
  {
    _context_t context;
    const auto query = to_sql_string(context, x);
    std::cout << "Running update call with\n" << query << std::endl;
    return 0;
  }

  template <typename Remove>
  size_t remove(const Remove& x)
  {
    _context_t context;
    const auto query = to_sql_string(context, x);
    std::cout << "Running remove call with\n" << query << std::endl;
    return 0;
  }

  template <typename Select>
  result_t select(const Select& x)
  {
    _context_t context;
    const auto query = to_sql_string(context, x);
    std::cout << "Running select call with\n" << query << std::endl;
    return {};
  }

  // Prepared statements start here
  using _prepared_statement_t = std::nullptr_t;

  template <typename T>
  auto _prepare(const T& t, ::sqlpp::consistent_t) -> decltype(t._prepare(*this))
  {
    return t._prepare(*this);
  }

  template <typename Check, typename T>
  auto _prepare(const T& t, Check) -> Check;

  template <typename T>
  auto prepare(const T& t) -> decltype(this->_prepare(t, sqlpp::prepare_check_t<_context_t, T>{}))
  {
    return _prepare(t, sqlpp::prepare_check_t<_context_t, T>{});
  }

  template <typename Statement>
  _prepared_statement_t prepare_execute(Statement& x)
  {
    _context_t context;
    const auto query = to_sql_string(context, x);
    std::cout << "Running prepare execute call with\n" << query << std::endl;
    return nullptr;
  }

  template <typename Insert>
  _prepared_statement_t prepare_insert(Insert& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running prepare insert call with\n" << query << std::endl;
    return nullptr;
  }

  template <typename Update>
  _prepared_statement_t prepare_update(Update& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running prepare update call with\n" << query << std::endl;
    return nullptr;
  }

  template <typename PreparedExecute>
  size_t run_prepared_execute(const PreparedExecute&)
  {
    return 0;
  }

  template <typename PreparedInsert>
  size_t run_prepared_insert(const PreparedInsert&)
  {
    return 0;
  }

  template <typename PreparedUpdate>
  size_t run_prepared_update(const PreparedUpdate&)
  {
    return 0;
  }

  template <typename Select>
  _prepared_statement_t prepare_select(Select& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running prepare select call with\n" << query << std::endl;
    return nullptr;
  }

  template <typename PreparedSelect>
  result_t run_prepared_select(PreparedSelect&)
  {
    return {};
  }

  auto attach(std::string name) -> ::sqlpp::schema_t
  {
    return {name};
  }

  void start_transaction()
  {
    _mock_data._last_isolation_level = _mock_data._default_isolation_level;
  }

  void start_transaction(sqlpp::isolation_level level)
  {
    _mock_data._last_isolation_level = level;
  }

  void set_default_isolation_level(sqlpp::isolation_level level)
  {
    _mock_data._default_isolation_level = level;
  }

  sqlpp::isolation_level get_default_isolation_level()
  {
    return _mock_data._default_isolation_level;
  }

  void rollback_transaction(bool)
  {
  }

  void commit_transaction()
  {
  }

  void report_rollback_failure(std::string)
  {
  }

  // temporary data store to verify the expected results were produced
  InternalMockData _mock_data;
};

struct MockSizeDb : public sqlpp::connection
{
  using _context_t = MockDb::_context_t;

  class result_t : public MockDb::result_t
  {
  public:
    size_t size() const
    {
      return 0;
    }
  };

  // Directly executed statements start here
  template <typename T>
  auto _run(const T& t, ::sqlpp::consistent_t) -> decltype(t._run(*this))
  {
    return t._run(*this);
  }

  template <typename Check, typename T>
  auto _run(const T& t, Check) -> Check;

  template <typename T>
  auto operator()(const T& t) -> decltype(this->_run(t, sqlpp::run_check_t<_context_t, T>{}))
  {
    return _run(t, sqlpp::run_check_t<_context_t, T>{});
  }

  size_t execute(const std::string&)
  {
    return 0;
  }

  template <
      typename Statement,
      typename Enable = typename std::enable_if<not std::is_convertible<Statement, std::string>::value, void>::type>
  size_t execute(const Statement& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running execute call with\n" << query << std::endl;
    return execute(query);
  }

  template <typename Insert>
  size_t insert(const Insert& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running insert call with\n" << query << std::endl;
    return 0;
  }

  template <typename Update>
  size_t update(const Update& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running update call with\n" << query << std::endl;
    return 0;
  }

  template <typename Remove>
  size_t remove(const Remove& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running remove call with\n" << query << std::endl;
    return 0;
  }

  template <typename Select>
  result_t select(const Select& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running select call with\n" << query << std::endl;
    return {};
  }

  // Prepared statements start here
  using _prepared_statement_t = std::nullptr_t;

  template <typename T>
  auto _prepare(const T& t, ::sqlpp::consistent_t) -> decltype(t._prepare(*this))
  {
    return t._prepare(*this);
  }

  template <typename Check, typename T>
  auto _prepare(const T& t, Check) -> Check;

  template <typename T>
  auto prepare(const T& t) -> decltype(this->_prepare(t, sqlpp::prepare_check_t<_context_t, T>{}))
  {
    return _prepare(t, sqlpp::prepare_check_t<_context_t, T>{});
  }

  template <typename Statement>
  _prepared_statement_t prepare_execute(Statement& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running prepare execute call with\n" << query << std::endl;
    return nullptr;
  }

  template <typename Insert>
  _prepared_statement_t prepare_insert(Insert& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running prepare insert call with\n" << query << std::endl;
    return nullptr;
  }

  template <typename PreparedExecute>
  size_t run_prepared_execute(const PreparedExecute&)
  {
    return 0;
  }

  template <typename PreparedInsert>
  size_t run_prepared_insert(const PreparedInsert&)
  {
    return 0;
  }

  template <typename Select>
  _prepared_statement_t prepare_select(Select& x)
  {
    _context_t context;
const auto query =     to_sql_string(context, x);
    std::cout << "Running prepare select call with\n" << query << std::endl;
    return nullptr;
  }

  template <typename PreparedSelect>
  result_t run_prepared_select(PreparedSelect&)
  {
    return {};
  }

  auto attach(std::string name) -> ::sqlpp::schema_t
  {
    return {name};
  }

  void start_transaction()
  {
    _mock_data._last_isolation_level = _mock_data._default_isolation_level;
  }

  void start_transaction(sqlpp::isolation_level level)
  {
    _mock_data._last_isolation_level = level;
  }

  void set_default_isolation_level(sqlpp::isolation_level level)
  {
    _mock_data._default_isolation_level = level;
  }

  sqlpp::isolation_level get_default_isolation_level()
  {
    return _mock_data._default_isolation_level;
  }

  void rollback_transaction(bool)
  {
  }

  void commit_transaction()
  {
  }

  void report_rollback_failure(std::string)
  {
  }

  // temporary data store to verify the expected results were produced
  InternalMockData _mock_data;
};

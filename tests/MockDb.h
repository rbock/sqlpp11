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

#ifndef SQLPP_MOCK_DB_H
#define SQLPP_MOCK_DB_H

#include <iostream>
#include <sqlpp11/connection.h>
#include <sqlpp11/data_types/no_value.h>
#include <sqlpp11/schema.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/serializer_context.h>
#include <sstream>

template <bool enforceNullResultTreatment>
struct MockDbT : public sqlpp::connection
{
  using _traits =
      ::sqlpp::make_traits<::sqlpp::no_value_t,
                           ::sqlpp::tag_if<::sqlpp::tag::enforce_null_result_treatment, enforceNullResultTreatment>>;

  struct _serializer_context_t
  {
    std::ostringstream _os;

    _serializer_context_t() = default;
    _serializer_context_t(const _serializer_context_t& rhs)
    {
      _os << rhs._os.str();
    }

    std::string str() const
    {
      return _os.str();
    }

    void reset()
    {
      _os.str("");
    }

    template <typename T>
    std::ostream& operator<<(T t)
    {
      return _os << t;
    }

    static std::string escape(std::string arg)
    {
      return sqlpp::serializer_context_t::escape(arg);
    }
  };

  using _interpreter_context_t = _serializer_context_t;

  _serializer_context_t get_serializer_context()
  {
    return {};
  }

  template <typename T>
  static _serializer_context_t& _serialize_interpretable(const T& t, _serializer_context_t& context)
  {
    sqlpp::serialize(t, context);
    return context;
  }

  template <typename T>
  static _serializer_context_t& _interpret_interpretable(const T& t, _interpreter_context_t& context)
  {
    sqlpp::serialize(t, context);
    return context;
  }

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
  auto operator()(const T& t) -> decltype(this->_run(t, sqlpp::run_check_t<_serializer_context_t, T>{}))
  {
    return _run(t, sqlpp::run_check_t<_serializer_context_t, T>{});
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
    _serializer_context_t context;
    ::sqlpp::serialize(x, context);
    std::cout << "Running execute call with\n" << context.str() << std::endl;
    return execute(context.str());
  }

  template <typename Insert>
  size_t insert(const Insert& x)
  {
    _serializer_context_t context;
    ::sqlpp::serialize(x, context);
    std::cout << "Running insert call with\n" << context.str() << std::endl;
    return 0;
  }

  template <typename Update>
  size_t update(const Update& x)
  {
    _serializer_context_t context;
    ::sqlpp::serialize(x, context);
    std::cout << "Running update call with\n" << context.str() << std::endl;
    return 0;
  }

  template <typename Remove>
  size_t remove(const Remove& x)
  {
    _serializer_context_t context;
    ::sqlpp::serialize(x, context);
    std::cout << "Running remove call with\n" << context.str() << std::endl;
    return 0;
  }

  template <typename Select>
  result_t select(const Select& x)
  {
    _serializer_context_t context;
    ::sqlpp::serialize(x, context);
    std::cout << "Running select call with\n" << context.str() << std::endl;
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
  auto prepare(const T& t) -> decltype(this->_prepare(t, sqlpp::prepare_check_t<_serializer_context_t, T>{}))
  {
    return _prepare(t, sqlpp::prepare_check_t<_serializer_context_t, T>{});
  }

  template <typename Statement>
  _prepared_statement_t prepare_execute(Statement& x)
  {
    _serializer_context_t context;
    ::sqlpp::serialize(x, context);
    std::cout << "Running prepare execute call with\n" << context.str() << std::endl;
    return nullptr;
  }

  template <typename Insert>
  _prepared_statement_t prepare_insert(Insert& x)
  {
    _serializer_context_t context;
    ::sqlpp::serialize(x, context);
    std::cout << "Running prepare insert call with\n" << context.str() << std::endl;
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
    _serializer_context_t context;
    ::sqlpp::serialize(x, context);
    std::cout << "Running prepare select call with\n" << context.str() << std::endl;
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
};

using MockDb = MockDbT<false>;
using EnforceDb = MockDbT<true>;

#endif

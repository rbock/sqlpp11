#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename Limit>
  struct limit_t
  {
    limit_t(Limit value) : _value(value)
    {
    }

    limit_t(const limit_t&) = default;
    limit_t(limit_t&&) = default;
    limit_t& operator=(const limit_t&) = default;
    limit_t& operator=(limit_t&&) = default;
    ~limit_t() = default;

    Limit _value;
  };

  template <typename Limit>
  struct is_clause<limit_t<Limit>> : public std::true_type
  {
  };

  template<typename Statement, typename Limit>
    struct consistency_check<Statement, limit_t<Limit>> { using type = consistent_t; };

  struct no_limit_t
  {
  };

  template <typename Statement>
  struct clause_base<no_limit_t, Statement> : public clause_data<no_limit_t, Statement>
  {
    using clause_data<no_limit_t, Statement>::clause_data;

    template <typename Arg, typename = sqlpp::enable_if_t<is_integral<remove_dynamic_t<Arg>>::value or is_unsigned_integral<remove_dynamic_t<Arg>>::value>>
    auto limit(Arg arg) const -> decltype(new_statement(*this, limit_t<Arg>{std::move(arg)}))
    {
      return new_statement(*this, limit_t<Arg>{std::move(arg)});
    }
  };

  template <typename Statement>
  struct consistency_check<Statement, no_limit_t>
  {
    using type = consistent_t;
  };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const no_limit_t&) -> std::string
  {
    return "";
  }

  template <typename Context, typename Limit>
  auto to_sql_string(Context& context, const limit_t<Limit>& t) -> std::string
  {
    return  " LIMIT " + operand_to_sql_string(context, t._value);
  }

  template <typename Context, typename Limit>
  auto to_sql_string(Context& context, const limit_t<dynamic_t<Limit>>& t) -> std::string
  {
    if (not t._value._condition)
    {
      return "";
    }
    return  " LIMIT " + operand_to_sql_string(context, t._value._expr);
  }

  template <typename T>
  auto limit(T&& t) -> decltype(statement_t<no_limit_t>().limit(std::forward<T>(t)))
  {
    return statement_t<no_limit_t>().limit(std::forward<T>(t));
  }

}  // namespace sqlpp

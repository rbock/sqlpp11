/*
 * Copyright (c) 2013-2015, Roland Bock
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

#ifndef SQLPP_EVAL_H
#define SQLPP_EVAL_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/field_spec.h>
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/verbatim.h>

namespace sqlpp
{
  template <typename Db, typename Expr>
  struct eval_t
  {
    static_assert(is_database<Db>::value, "Db parameter of eval has to be a database connection");
    static_assert(is_expression_t<Expr>::value,
                  "Expression parameter of eval has to be an sqlpp expression or a string");
    static_assert(required_tables_of<Expr>::size::value == 0,
                  "Expression cannot be used in eval because it requires tables");
    using _name_type = alias::a_t::_alias_t;
    using _value_type = value_type_of<Expr>;
    using _field_spec = field_spec_t<_name_type, _value_type, true, false>;
    using type = result_field_t<Db, _field_spec>;
  };

  template <typename Db,
            typename Expr,
            typename std::enable_if<not std::is_convertible<Expr, std::string>::value, int>::type = 0>
  auto eval(Db& db, Expr expr) -> typename eval_t<Db, Expr>::type
  {
    return db(select(expr.as(alias::a))).front().a;
  }

  template <typename ValueType, typename Db>
  auto eval(Db& db, std::string sql_code) -> decltype(eval(db, verbatim<ValueType>(sql_code)))
  {
    return eval(db, verbatim<ValueType>(sql_code));
  }
}

#endif

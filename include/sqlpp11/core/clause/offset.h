#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
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

#include <sqlpp11/core/clause/expression_static_check.h>
#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp {
SQLPP_WRAPPED_STATIC_ASSERT(
    assert_no_unknown_tables_in_offset_t,
    "at least one expression in offset() requires a table which is otherwise "
    "not known in the statement");

SQLPP_WRAPPED_STATIC_ASSERT(
    assert_no_unknown_static_tables_in_offset_t,
    "at least one expression in offset() statically requires a table which is "
    "only known dynamically in the statement");

template <typename Expression> struct offset_t {
  Expression _expr;
};

template <typename Expression>
struct is_clause<offset_t<Expression>> : public std::true_type {};

template <typename Statement, typename Expression>
struct consistency_check<Statement, offset_t<Expression>> {
  using type = detail::expression_static_check_t<
      Statement, Expression, assert_no_unknown_static_tables_in_offset_t>;
};

template <typename Statement, typename Expression>
struct prepare_check<Statement, offset_t<Expression>> {
  using type = static_combined_check_t<
      static_check_t<Statement::template _no_unknown_tables<Expression>,
                     assert_no_unknown_tables_in_offset_t>,
      static_check_t<Statement::template _no_unknown_static_tables<Expression>,
                     assert_no_unknown_static_tables_in_offset_t>>;
};

struct no_offset_t {
  template <typename Statement, typename Arg>
    requires(is_integral<remove_dynamic_t<Arg>>::value or
             is_unsigned_integral<remove_dynamic_t<Arg>>::value)
  auto offset(this Statement &&statement, Arg arg) {
    return new_statement<no_offset_t>(std::forward<Statement>(statement),
                                      offset_t<Arg>{std::move(arg)});
  }
};

template <typename Statement> struct consistency_check<Statement, no_offset_t> {
  using type = consistent_t;
};

// Interpreters
template <typename Context>
auto to_sql_string(Context &, const no_offset_t &) -> std::string {
  return "";
}

template <typename Context, typename Expression>
auto to_sql_string(Context &context, const offset_t<Expression> &t)
    -> std::string {
  return " OFFSET " + operand_to_sql_string(context, t._expr);
}

template <typename Context, typename Expression>
auto to_sql_string(Context &context, const offset_t<dynamic_t<Expression>> &t)
    -> std::string {
  if (not t._expr._condition) {
    return "";
  }
  return " OFFSET " + operand_to_sql_string(context, t._expr._expr);
}

template <typename T>
  requires(is_integral<remove_dynamic_t<T>>::value or
           is_unsigned_integral<remove_dynamic_t<T>>::value)
auto offset(T t) {
  return statement_t<no_offset_t>().offset(std::move(t));
}

} // namespace sqlpp

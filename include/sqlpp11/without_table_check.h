/*
 * Copyright (c) 2016-2016, Roland Bock
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

#ifndef SQLPP_WITHOUT_TABLE_CHECK_H
#define SQLPP_ALIAS_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/serializer.h>

namespace sqlpp
{
  template <typename Expression>
  struct without_table_check_t : Expression
  {
    using _required_tables = detail::type_set<>;

    without_table_check_t(Expression expression) : Expression(expression)
    {
    }
  };

  template <typename Context, typename Expression>
  struct serializer_t<Context, without_table_check_t<Expression>>
  {
    using _serialize_check = serialize_check_of<Context, Expression>;
    using T = without_table_check_t<Expression>;

    static Context& _(const T& t, Context& context)
    {
      serialize<Expression>(t, context);
      return context;
    }
  };

  template <typename Expression>
  auto without_table_check(Expression expr) -> without_table_check_t<Expression>
  {
    static_assert(is_expression_t<Expression>::value, "invalid argument (expression expected)");

    return {expr};
  }
}

#endif

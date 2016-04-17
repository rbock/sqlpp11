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

#ifndef SQLPP_BOOLEAN_EXPRESSION_OPERATORS_H
#define SQLPP_BOOLEAN_EXPRESSION_OPERATORS_H

#include <sqlpp11/expression_return_types.h>
#include <sqlpp11/operand_check.h>
#include <sqlpp11/expression_operators.h>
#include <sqlpp11/basic_expression_operators.h>

namespace sqlpp
{
  template <typename Expression>
  struct expression_operators<Expression, boolean> : public basic_expression_operators<Expression>
  {
  };

  template <typename L, typename R>
  struct return_type_and<L, R, unwrapped_binary_operand_check_t<L, is_boolean_t, R, is_boolean_t>>
  {
    using check = consistent_t;
    using type = logical_and_t<wrap_operand_t<L>, wrap_operand_t<R>>;
  };

  template <typename L, typename R>
  struct return_type_or<L, R, unwrapped_binary_operand_check_t<L, is_boolean_t, R, is_boolean_t>>
  {
    using check = consistent_t;
    using type = logical_or_t<wrap_operand_t<L>, wrap_operand_t<R>>;
  };

  template <typename T, typename Defer>
  struct return_type_not<T, Defer, unwrapped_unary_operand_check_t<T, is_boolean_t>>
  {
    using check = consistent_t;
    using type = logical_not_t<wrap_operand_t<T>>;
  };
}

#endif

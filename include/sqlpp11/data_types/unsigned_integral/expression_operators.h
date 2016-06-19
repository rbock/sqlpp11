/*
 * Copyright (c) 2013-2016, Roland Bock, Aaron Bishop
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

#ifndef SQLPP_UNSIGNED_INTEGRAL_EXPRESSION_OPERATORS_H
#define SQLPP_UNSIGNED_INTEGRAL_EXPRESSION_OPERATORS_H

#include <sqlpp11/expression_return_types.h>
#include <sqlpp11/operand_check.h>
#include <sqlpp11/expression_operators.h>
#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/value_type.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/data_types/unsigned_integral/data_type.h>

namespace sqlpp
{
  struct integral;
  
  template <typename Expression>
  struct expression_operators<Expression, unsigned_integral> : public basic_expression_operators<Expression>
  {
  };

  template <typename L, typename R>
  struct return_type_plus<L, R, binary_operand_check_t<L, is_unsigned_integral_t, R, is_numeric_t>>
  {
    using check = consistent_t;
    using type = value_type_of<wrap_operand_t<R>>;
  };

  template <typename L, typename R>
  struct return_type_minus<L, R, binary_operand_check_t<L, is_unsigned_integral_t, R, is_numeric_not_unsigned_t>>
  {
    using check = consistent_t;
    using type = value_type_of<wrap_operand_t<R>>;
  };

  template <typename L, typename R>
  struct return_type_minus<L, R, binary_operand_check_t<L, is_unsigned_integral_t, R, is_unsigned_integral_t>>
  {
    using check = consistent_t;
    using type = minus_t<wrap_operand_t<L>, integral, wrap_operand_t<R>>;
  };

  template <typename L, typename R>
  struct return_type_multiplies<L, R, binary_operand_check_t<L, is_unsigned_integral_t, R, is_numeric_t>>
  {
    using check = consistent_t;
    using type = value_type_of<wrap_operand_t<R>>;
  };

  template <typename L, typename R>
  struct return_type_divides<L, R, binary_operand_check_t<L, is_unsigned_integral_t, R, is_numeric_t>>
  {
    using check = consistent_t;
    using type = value_type_of<wrap_operand_t<R>>;
  };

  template <typename L, typename R>
  struct return_type_modulus<L, R, binary_operand_check_t<L, is_unsigned_integral_t, R, is_unsigned_integral_t>>
  {
    using check = consistent_t;
    using type = modulus_t<wrap_operand_t<L>, wrap_operand_t<R>>;
  };

  template <typename T, typename Defer>
  struct return_type_unary_plus<T, Defer, unary_operand_check_t<T, is_unsigned_integral_t>>
  {
    using check = consistent_t;
    using type = unary_plus_t<unsigned_integral, wrap_operand_t<T>>;
  };

  template <typename T, typename Defer>
  struct return_type_unary_minus<T, Defer, unary_operand_check_t<T, is_unsigned_integral_t>>
  {
    using check = consistent_t;
    using type = unary_minus_t<integral, wrap_operand_t<T>>;
  };

  template <typename L, typename R>
  struct return_type_bitwise_and<L, R, binary_operand_check_t<L, is_unsigned_integral_t, R, is_unsigned_integral_t>>
  {
    using check = consistent_t;
    using type = bitwise_and_t<wrap_operand_t<L>, unsigned_integral, wrap_operand_t<R>>;
  };

  template <typename L, typename R>
  struct return_type_bitwise_or<L, R, binary_operand_check_t<L, is_unsigned_integral_t, R, is_unsigned_integral_t>>
  {
    using check = consistent_t;
    using type = bitwise_or_t<wrap_operand_t<L>, unsigned_integral, wrap_operand_t<R>>;
  };
}
#endif

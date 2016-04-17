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

#ifndef SQLPP_EXPRESSION_RETURN_TYPES_H
#define SQLPP_EXPRESSION_RETURN_TYPES_H

#include <sqlpp11/bad_expression.h>

namespace sqlpp
{
  template <typename L, typename R, typename Enable = void>
  struct return_type_and
  {
    using check = assert_valid_operands;
    using type = bad_expression<boolean>;
  };
  template <typename L, typename R>
  using return_type_and_t = typename return_type_and<L, R>::type;

  template <typename L, typename R, typename Enable = void>
  struct return_type_bitwise_and
  {
    using check = assert_valid_operands;
    using type = bad_expression<boolean>;
  };
  template <typename L, typename R>
  using return_type_bitwise_and_t = typename return_type_bitwise_and<L, R>::type;

  template <typename L, typename R, typename Enable = void>
  struct return_type_or
  {
    using check = assert_valid_operands;
    using type = bad_expression<boolean>;
  };
  template <typename L, typename R>
  using return_type_or_t = typename return_type_or<L, R>::type;

  template <typename L, typename R, typename Enable = void>
  struct return_type_bitwise_or
  {
    using check = assert_valid_operands;
    using type = bad_expression<boolean>;
  };
  template <typename L, typename R>
  using return_type_bitwise_or_t = typename return_type_bitwise_or<L, R>::type;

  template <typename T, typename Defer, typename Enable = void>
  struct return_type_not
  {
    using check = assert_valid_operands;
    using type = bad_expression<boolean>;
  };
  template <typename T, typename Defer>
  using return_type_not_t = typename return_type_not<T, Defer>::type;

  template <typename L, typename R, typename Enable = void>
  struct return_type_plus
  {
    using check = assert_valid_operands;
    using type = bad_expression<value_type_of<L>>;
  };
  template <typename L, typename R>
  using return_type_plus_t = typename return_type_plus<L, R>::type;

  template <typename L, typename R, typename Enable = void>
  struct return_type_minus
  {
    using check = assert_valid_operands;
    using type = bad_expression<value_type_of<L>>;
  };
  template <typename L, typename R>
  using return_type_minus_t = typename return_type_minus<L, R>::type;

  template <typename L, typename R, typename Enable = void>
  struct return_type_multiplies
  {
    using check = assert_valid_operands;
    using type = bad_expression<value_type_of<L>>;
  };
  template <typename L, typename R>
  using return_type_multiplies_t = typename return_type_multiplies<L, R>::type;

  template <typename L, typename R, typename Enable = void>
  struct return_type_divides
  {
    using check = assert_valid_operands;
    using type = bad_expression<value_type_of<L>>;
  };
  template <typename L, typename R>
  using return_type_divides_t = typename return_type_divides<L, R>::type;

  template <typename L, typename R, typename Enable = void>
  struct return_type_modulus
  {
    using check = assert_valid_operands;
    using type = bad_expression<value_type_of<L>>;
  };
  template <typename L, typename R>
  using return_type_modulus_t = typename return_type_modulus<L, R>::type;

  template <typename T, typename Defer, typename Enable = void>
  struct return_type_unary_plus
  {
    using check = assert_valid_operands;
    using type = bad_expression<value_type_of<T>>;
  };
  template <typename T, typename Defer>
  using return_type_unary_plus_t = typename return_type_unary_plus<T, Defer>::type;

  template <typename T, typename Defer, typename Enable = void>
  struct return_type_unary_minus
  {
    using check = assert_valid_operands;
    using type = bad_expression<value_type_of<T>>;
  };
  template <typename T, typename Defer>
  using return_type_unary_minus_t = typename return_type_unary_minus<T, Defer>::type;
}

#endif

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

#ifndef SQLPP_TEXT_EXPRESSION_OPERATORS_H
#define SQLPP_TEXT_EXPRESSION_OPERATORS_H

#include <sqlpp11/expression_operators.h>
#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/data_types/text/data_type.h>

namespace sqlpp
{
  template <typename... Args>
  struct concat_t;

  template <typename Operand, typename Pattern>
  struct like_t;

  template <typename T, typename Defer, typename Enable = void>
  struct return_type_like
  {
    using check = assert_valid_operands;
    using type = bad_expression<boolean>;
  };
  template <typename T, typename Defer>
  using return_type_like_t = typename return_type_like<T, Defer>::type;

  template <typename L, typename R>
  struct return_type_like<L, R, binary_operand_check_t<L, is_text_t, R, is_text_t>>
  {
    using check = consistent_t;
    using type = like_t<wrap_operand_t<L>, wrap_operand_t<R>>;
  };

  template <typename Expression>
  struct expression_operators<Expression, text> : public basic_expression_operators<Expression>
  {
    template <typename T>
    using _is_valid_operand = is_valid_operand<text, T>;

    template <typename R>
    auto like(const R& r) const -> return_type_like_t<Expression, R>
    {
      return_type_like<Expression, R>::check::_();
      return {*static_cast<const Expression*>(this), wrap_operand_t<R>{r}};
    }
  };

  template <typename L, typename R>
  struct return_type_plus<L, R, binary_operand_check_t<L, is_text_t, R, is_text_t>>
  {
    using check = consistent_t;
    using type = concat_t<wrap_operand_t<L>, wrap_operand_t<R>>;
  };
}
#endif

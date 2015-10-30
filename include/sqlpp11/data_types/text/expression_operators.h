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

  template <typename Base>
  struct expression_operators<Base, text> : public basic_expression_operators<Base, text>
  {
    template <typename T>
    using _is_valid_operand = is_valid_operand<text, T>;

    template <typename T>
    concat_t<Base, wrap_operand_t<T>> operator+(T t) const
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid rhs operand");

      return {*static_cast<const Base*>(this), {t}};
    }

    template <typename T>
    like_t<Base, wrap_operand_t<T>> like(T t) const
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid argument for like()");

      return {*static_cast<const Base*>(this), {t}};
    }
  };
}
#endif

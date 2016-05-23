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

#ifndef SQLPP_UNSIGNED_INTEGRAL_COLUMN_OPERATORS_H
#define SQLPP_UNSIGNED_INTEGRAL_COLUMN_OPERATORS_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/assignment.h>
#include <sqlpp11/value_type.h>
#include <sqlpp11/data_types/unsigned_integral/data_type.h>
#include <sqlpp11/data_types/column_operators.h>

namespace sqlpp
{
  template <typename Column>
  struct column_operators<Column, unsigned_integral>
  {
    template <typename T>
    using _is_valid_operand = is_valid_operand<unsigned_integral, T>;

    template <typename T>
    auto operator+=(T t) const -> assignment_t<Column, plus_t<Column, value_type_t<T>, wrap_operand_t<T>>>
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

      return {*static_cast<const Column*>(this), {{*static_cast<const Column*>(this), rhs{t}}}};
    }

    template <typename T>
    auto operator-=(T t) const -> assignment_t<Column, minus_t<Column, value_type_t<T>, wrap_operand_t<T>>>
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

      return {*static_cast<const Column*>(this), {{*static_cast<const Column*>(this), rhs{t}}}};
    }

    template <typename T>
    auto operator/=(T t) const -> assignment_t<Column, divides_t<Column, wrap_operand_t<T>>>
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

      return {*static_cast<const Column*>(this), {{*static_cast<const Column*>(this), rhs{t}}}};
    }

    template <typename T>
    auto operator*=(T t) const -> assignment_t<Column, multiplies_t<Column, value_type_t<T>, wrap_operand_t<T>>>
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

      return {*static_cast<const Column*>(this), {{*static_cast<const Column*>(this), rhs{t}}}};
    }
  };
}

#endif

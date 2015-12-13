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

#ifndef SQLPP_VALUE_OR_NULL_H
#define SQLPP_VALUE_OR_NULL_H

#include <sqlpp11/wrap_operand.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  template <typename ValueType>
  struct value_or_null_t
  {
    using _cpp_value_type = typename ValueType::_cpp_value_type;

    using _traits = make_traits<ValueType, tag::is_expression>;
    using _nodes = detail::type_vector<>;

    value_or_null_t(_cpp_value_type value) : _value(value), _is_null(false)
    {
    }

    value_or_null_t(const null_t&) : _value(), _is_null(true)
    {
    }

    typename ValueType::_cpp_value_type _value;
    bool _is_null;
  };

  template <typename T>
  auto value_or_null(T t) -> value_or_null_t<value_type_of<wrap_operand_t<T>>>
  {
    static_assert(is_wrapped_value_t<wrap_operand_t<T>>::value,
                  "value_or_null() is to be called with non-sql-type like int, or string or null");
    return {t};
  }

  template <typename ValueType>
  auto value_or_null(null_t t) -> value_or_null_t<ValueType>
  {
    static_assert(is_value_type_t<ValueType>::value,
                  "value_or_null() is to be called with non-sql-type like int, or string");
    return {t};
  }
}

#endif

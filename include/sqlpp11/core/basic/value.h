#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <sqlpp11/core/operator/as_expression.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename T>
  struct value_t
  {
    template <typename alias_provider>
    as_expression<value_t, alias_provider> as(const alias_provider& /*unused*/) const
    {
      return {*this};
    }

    T _value;
  };

  template<typename T>
  struct value_type_of<value_t<T>>
  {
    using type = value_type_of_t<T>;
  };

  template <typename T>
  using check_value_arg = std::enable_if_t<not std::is_same<value_type_of_t<T>, no_value_t>::value and values_are_comparable<T, T>::value>;

  template <typename Context, typename T>
  Context& serialize(Context& context, const value_t<T>& t)
  {
#warning: Untested
    serialize(context, t._value);

    return context;
  }

  template <typename T, typename = check_value_arg<T>>
  auto value(T t) -> value_t<T>
  {
    return {std::move(t)};
  }

}  // namespace sqlpp

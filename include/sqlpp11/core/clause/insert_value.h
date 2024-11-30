#pragma once

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

#include <sqlpp11/core/default_value.h>
#include <sqlpp11/core/basic/value.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/query/dynamic.h>

namespace sqlpp
{
  namespace detail
  {
    template <typename Type, bool>
    struct type_if
    {
      using type = Type;
    };

    template <typename Type>
    struct type_if<Type, false>
    {
      struct type
      {
        using _nodes = detail::type_vector<>;
      };
    };
  }  // namespace detail

  template <typename Column>
  struct insert_value_t
  {
    using _is_insert_value = std::true_type;
    using _value_t = parameter_value_t<value_type_of_t<Column>>;

    insert_value_t(_value_t value)
        : _is_default(false), _value(std::move(value))
    {
    }

    insert_value_t(const default_value_t& /*unused*/)
        : _is_default(true), _value{}
    {
    }

    insert_value_t(const insert_value_t&) = default;
    insert_value_t(insert_value_t&&) = default;
    insert_value_t& operator=(const insert_value_t&) = default;
    insert_value_t& operator=(insert_value_t&&) = default;
    ~insert_value_t() = default;

    bool _is_default;
    _value_t _value;
  };

  template<typename Column>
    struct make_insert_value {
      using type = insert_value_t<Column>;
    };

  template<typename Column>
    struct make_insert_value<dynamic_t<Column>> {
      using type = dynamic_t<insert_value_t<Column>>;
    };
  template<typename Column>
    using make_insert_value_t = typename make_insert_value<Column>::type;

  template <typename Context, typename ValueType>
  auto to_sql_string(Context& context, const insert_value_t<ValueType>& t) -> std::string
  {
    if (t._is_default)
    {
      return "DEFAULT";
    }
    return operand_to_sql_string(context, t._value);
  }
}  // namespace sqlpp

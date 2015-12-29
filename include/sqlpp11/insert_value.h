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

#ifndef SQLPP_INSERT_VALUE_H
#define SQLPP_INSERT_VALUE_H

#include <sqlpp11/default_value.h>
#include <sqlpp11/null.h>
#include <sqlpp11/tvin.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/detail/type_set.h>

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
        using _traits = make_traits<no_value_t, tag::is_noop>;
        using _nodes = detail::type_vector<>;
      };
    };
  }

  template <typename Column>
  struct insert_value_t
  {
    using _is_insert_value = std::true_type;
    using _column_t = Column;
    static constexpr bool _trivial_value_is_null = trivial_value_is_null_t<Column>::value;
    using _pure_value_t = typename value_type_of<Column>::_cpp_value_type;
    using _wrapped_value_t = wrap_operand_t<_pure_value_t>;
    using _tvin_t = tvin_t<_wrapped_value_t>;

    insert_value_t(rhs_wrap_t<_wrapped_value_t, _trivial_value_is_null> rhs)
        : _is_null(rhs._is_null()), _is_default(rhs._is_default()), _value(rhs._expr._t)
    {
    }

    insert_value_t(rhs_wrap_t<_tvin_t, _trivial_value_is_null> rhs)
        : _is_null(rhs._is_null()), _is_default(rhs._is_default()), _value(rhs._expr._value)
    {
    }

    insert_value_t(const rhs_wrap_t<null_t, _trivial_value_is_null>&) : _is_null(true), _is_default(false), _value{}
    {
    }

    insert_value_t(const rhs_wrap_t<default_value_t, _trivial_value_is_null>&)
        : _is_null(false), _is_default(true), _value{}
    {
    }

    insert_value_t(const insert_value_t&) = default;
    insert_value_t(insert_value_t&&) = default;
    insert_value_t& operator=(const insert_value_t&) = default;
    insert_value_t& operator=(insert_value_t&&) = default;
    ~insert_value_t() = default;

    bool _is_null;
    bool _is_default;
    _wrapped_value_t _value;
  };

  template <typename Context, typename ValueType>
  struct serializer_t<Context, insert_value_t<ValueType>>
  {
    using _serialize_check = serialize_check_of<Context, ValueType>;
    using T = insert_value_t<ValueType>;

    static Context& _(const T& t, Context& context)
    {
      if ((trivial_value_is_null_t<typename T::_column_t>::value and t._value._is_trivial()) or t._is_null)
      {
        context << "NULL";
      }
      else if (t._is_default)
        context << "DEFAULT";
      else
        serialize_operand(t._value, context);
      return context;
    }
  };
}

#endif

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

#ifndef SQLPP_TVIN_H
#define SQLPP_TVIN_H

// TVIN: Trivial value is NULL

#include <sqlpp11/serialize.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/wrap_operand.h>

namespace sqlpp
{
  template <typename Operand>
  struct tvin_arg_t
  {
    using _traits = make_traits<value_type_of<Operand>, tag::is_expression>;
    using _nodes = detail::type_vector<Operand>;

    using _operand_t = Operand;

    tvin_arg_t(_operand_t operand) : _value(operand)
    {
    }
    tvin_arg_t(const tvin_arg_t&) = default;
    tvin_arg_t(tvin_arg_t&&) = default;
    tvin_arg_t& operator=(const tvin_arg_t&) = default;
    tvin_arg_t& operator=(tvin_arg_t&&) = default;
    ~tvin_arg_t() = default;

    _operand_t _value;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_tvin_with_correct_operator_t,
                               "tvin may only be used with operators =, == and !=");

  template <typename Context, typename Operand>
  struct serializer_t<Context, tvin_arg_t<Operand>>
  {
    using _serialize_check = assert_tvin_with_correct_operator_t;
    using T = tvin_arg_t<Operand>;

    static Context& _(const T&, Context&)
    {
      _serialize_check{};
    }
  };

  template <typename T>
  struct tvin_t;

  namespace detail
  {
    template <typename T>
    struct allow_tvin_impl
    {
      using type = T;
    };
    template <typename T>
    struct allow_tvin_impl<tvin_arg_t<T>>
    {
      using type = tvin_t<T>;
    };
  }
  template <typename T>
  using allow_tvin_t = typename detail::allow_tvin_impl<T>::type;

  template <typename Operand>
  struct tvin_t
  {
    using _traits = make_traits<value_type_of<Operand>, tag::is_expression>;
    using _nodes = detail::type_vector<Operand>;

    using _operand_t = Operand;

    tvin_t(tvin_arg_t<Operand> arg) : _value(arg._value)
    {
    }
    tvin_t(const tvin_t&) = default;
    tvin_t(tvin_t&&) = default;
    tvin_t& operator=(const tvin_t&) = default;
    tvin_t& operator=(tvin_t&&) = default;
    ~tvin_t() = default;

    bool _is_trivial() const
    {
      return _value._is_trivial();
    }

    bool _is_null() const
    {
      return _value._is_trivial();
    }

    _operand_t _value;
  };

  namespace detail
  {
    template <typename T>
    struct is_tvin_impl
    {
      using type = std::false_type;
    };
    template <typename T>
    struct is_tvin_impl<tvin_t<T>>
    {
      using type = std::true_type;
    };
  }
  template <typename T>
  using is_tvin_t = typename detail::is_tvin_impl<T>::type;

  template <typename Context, typename Operand>
  struct serializer_t<Context, tvin_t<Operand>>
  {
    using _serialize_check = serialize_check_of<Context, Operand>;
    using T = tvin_t<Operand>;

    static Context& _(const T& t, Context& context)
    {
      if (t._is_trivial())
      {
        context << "NULL";
      }
      else
      {
        serialize(t._value, context);
      }
      return context;
    }
  };

  template <typename Operand>
  auto tvin(Operand operand) -> tvin_arg_t<wrap_operand_t<Operand>>
  {
    using _operand_t = wrap_operand_t<Operand>;
    static_assert(not std::is_same<_operand_t, Operand>::value or is_result_field_t<Operand>::value,
                  "tvin() used with invalid type (only string and primitive types allowed)");
    return {{operand}};
  }
}

#endif

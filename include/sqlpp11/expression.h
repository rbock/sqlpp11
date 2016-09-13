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

#ifndef SQLPP_EXPRESSION_H
#define SQLPP_EXPRESSION_H

#include <sqlpp11/alias.h>
#include <sqlpp11/data_types/boolean.h>
#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/noop.h>
#include <sqlpp11/rhs_wrap.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/tvin.h>
#include <sqlpp11/wrap_operand.h>

namespace sqlpp
{
  template <typename Lhs, typename Rhs>
  struct binary_expression_t<Lhs, op::equal_to, Rhs>
      : public expression_operators<binary_expression_t<Lhs, op::equal_to, Rhs>, boolean>,
        public alias_operators<binary_expression_t<Lhs, op::equal_to, Rhs>>
  {
    using _traits = make_traits<boolean, tag::is_expression>;
    using _lhs_t = Lhs;
    using _rhs_t = rhs_wrap_t<allow_tvin_t<Rhs>, trivial_value_is_null_t<_lhs_t>::value>;
    using _nodes = detail::type_vector<_lhs_t, _rhs_t>;

    binary_expression_t(Lhs lhs, Rhs rhs) : _lhs(lhs), _rhs(rhs)
    {
    }

    binary_expression_t(const binary_expression_t&) = default;
    binary_expression_t(binary_expression_t&&) = default;
    binary_expression_t& operator=(const binary_expression_t&) = default;
    binary_expression_t& operator=(binary_expression_t&&) = default;
    ~binary_expression_t() = default;

    _lhs_t _lhs;
    _rhs_t _rhs;
  };

  template <typename Context, typename Lhs, typename Rhs>
  struct serializer_t<Context, binary_expression_t<Lhs, op::equal_to, Rhs>>
  {
    using T = binary_expression_t<Lhs, op::equal_to, Rhs>;
    using _serialize_check = serialize_check_of<Context, typename T::_lhs_t, typename T::_rhs_t>;

    static Context& _(const T& t, Context& context)
    {
      context << "(";
      serialize_operand(t._lhs, context);
      if (t._rhs._is_null())
      {
        context << " IS NULL";
      }
      else
      {
        context << "=";
        serialize_operand(t._rhs, context);
      }
      context << ")";
      return context;
    }
  };

  template <typename Lhs, typename Rhs>
  struct binary_expression_t<Lhs, op::not_equal_to, Rhs>
      : public expression_operators<binary_expression_t<Lhs, op::not_equal_to, Rhs>, boolean>,
        public alias_operators<binary_expression_t<Lhs, op::not_equal_to, Rhs>>
  {
    using _traits = make_traits<boolean, tag::is_expression>;
    using _lhs_t = Lhs;
    using _rhs_t = rhs_wrap_t<allow_tvin_t<Rhs>, trivial_value_is_null_t<_lhs_t>::value>;
    using _nodes = detail::type_vector<_lhs_t, _rhs_t>;

    binary_expression_t(Lhs lhs, Rhs rhs) : _lhs(lhs), _rhs(rhs)
    {
    }

    binary_expression_t(const binary_expression_t&) = default;
    binary_expression_t(binary_expression_t&&) = default;
    binary_expression_t& operator=(const binary_expression_t&) = default;
    binary_expression_t& operator=(binary_expression_t&&) = default;
    ~binary_expression_t() = default;

    _lhs_t _lhs;
    _rhs_t _rhs;
  };

  template <typename Context, typename Lhs, typename Rhs>
  struct serializer_t<Context, binary_expression_t<Lhs, op::not_equal_to, Rhs>>
  {
    using T = binary_expression_t<Lhs, op::not_equal_to, Rhs>;
    using _serialize_check = serialize_check_of<Context, typename T::_lhs_t, typename T::_rhs_t>;

    static Context& _(const T& t, Context& context)
    {
      context << "(";
      serialize_operand(t._lhs, context);
      if (t._rhs._is_null())
      {
        context << " IS NOT NULL";
      }
      else
      {
        context << "<>";
        serialize_operand(t._rhs, context);
      }
      context << ")";
      return context;
    }
  };

  template <typename Rhs>
  struct unary_expression_t<op::logical_not, Rhs>
      : public expression_operators<unary_expression_t<op::logical_not, Rhs>, boolean>,
        public alias_operators<unary_expression_t<op::logical_not, Rhs>>
  {
    using _traits = make_traits<boolean, tag::is_expression>;
    using _nodes = detail::type_vector<Rhs>;

    unary_expression_t(Rhs rhs) : _rhs(rhs)
    {
    }

    unary_expression_t(const unary_expression_t&) = default;
    unary_expression_t(unary_expression_t&&) = default;
    unary_expression_t& operator=(const unary_expression_t&) = default;
    unary_expression_t& operator=(unary_expression_t&&) = default;
    ~unary_expression_t() = default;

    Rhs _rhs;
  };

  template <typename Context, typename Rhs>
  struct serializer_t<Context, unary_expression_t<op::logical_not, Rhs>>
  {
    using _serialize_check = serialize_check_of<Context, Rhs>;
    using T = unary_expression_t<op::logical_not, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      context << "(";
      if (trivial_value_is_null_t<Rhs>::value)
      {
        serialize_operand(t._rhs, context);
        context << " IS NULL ";
      }
      else
      {
        context << "NOT ";
        serialize_operand(t._rhs, context);
      }
      context << ")";

      return context;
    }
  };

  template <typename Lhs, typename O, typename Rhs>
  struct binary_expression_t : public expression_operators<binary_expression_t<Lhs, O, Rhs>, value_type_of<O>>,
                               public alias_operators<binary_expression_t<Lhs, O, Rhs>>
  {
    using _traits = make_traits<value_type_of<O>, tag::is_expression>;
    using _nodes = detail::type_vector<Lhs, Rhs>;

    binary_expression_t(Lhs lhs, Rhs rhs) : _lhs(lhs), _rhs(rhs)
    {
    }

    binary_expression_t(const binary_expression_t&) = default;
    binary_expression_t(binary_expression_t&&) = default;
    binary_expression_t& operator=(const binary_expression_t&) = default;
    binary_expression_t& operator=(binary_expression_t&&) = default;
    ~binary_expression_t() = default;

    Lhs _lhs;
    Rhs _rhs;
  };

  template <typename Context, typename Lhs, typename O, typename Rhs>
  struct serializer_t<Context, binary_expression_t<Lhs, O, Rhs>>
  {
    using _serialize_check = serialize_check_of<Context, Lhs, Rhs>;
    using T = binary_expression_t<Lhs, O, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      context << "(";
      serialize_operand(t._lhs, context);
      context << O::_name;
      serialize_operand(t._rhs, context);
      context << ")";
      return context;
    }
  };

  template <typename O, typename Rhs>
  struct unary_expression_t : public expression_operators<unary_expression_t<O, Rhs>, value_type_of<O>>,
                              public alias_operators<unary_expression_t<O, Rhs>>
  {
    using _traits = make_traits<value_type_of<O>, tag::is_expression>;
    using _nodes = detail::type_vector<Rhs>;

    unary_expression_t(Rhs rhs) : _rhs(rhs)
    {
    }

    unary_expression_t(const unary_expression_t&) = default;
    unary_expression_t(unary_expression_t&&) = default;
    unary_expression_t& operator=(const unary_expression_t&) = default;
    unary_expression_t& operator=(unary_expression_t&&) = default;
    ~unary_expression_t() = default;

    Rhs _rhs;
  };

  template <typename Context, typename O, typename Rhs>
  struct serializer_t<Context, unary_expression_t<O, Rhs>>
  {
    using _serialize_check = serialize_check_of<Context, Rhs>;
    using T = unary_expression_t<O, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      context << "(";
      context << O::_name;
      serialize_operand(t._rhs, context);
      context << ")";
      return context;
    }
  };
}

#endif

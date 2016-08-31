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

#ifndef SQLPP_RHS_WRAP_H
#define SQLPP_RHS_WRAP_H

#include <sqlpp11/default_value.h>
#include <sqlpp11/null.h>
#include <sqlpp11/tvin.h>

namespace sqlpp
{
  namespace detail
  {
    template <typename Expr, typename Enable = void>
    struct rhs_is_trivial_t
    {
      static constexpr bool _(const Expr&)
      {
        return false;
      }
    };

    template <typename Expr>
    struct rhs_is_trivial_t<Expr, typename std::enable_if<is_wrapped_value_t<Expr>::value, void>::type>
    {
      static bool _(const Expr& t)
      {
        return t._is_trivial();
      }
    };

    template <typename Expr>
    struct rhs_is_trivial_t<Expr, typename std::enable_if<is_tvin_t<Expr>::value, void>::type>
    {
      static bool _(const Expr& t)
      {
        return t._is_trivial();
      }
    };

    template <typename Expr>
    struct rhs_is_trivial_t<Expr, typename std::enable_if<is_result_field_t<Expr>::value, void>::type>
    {
      static bool _(const Expr& t)
      {
        if (null_is_trivial_value_t<Expr>::value)
        {
          return t.is_null();
        }
        else
        {
          if (t.is_null())
          {
            return false;
          }
          else
          {
            return t._is_trivial();
          }
        }
      }
    };

    template <typename Expr, typename Enable = void>
    struct rhs_is_null_t
    {
      static constexpr bool _(const Expr&)
      {
        return false;
      }
    };

    template <typename Expr>
    struct rhs_is_null_t<Expr, typename std::enable_if<is_tvin_t<Expr>::value, void>::type>
    {
      static bool _(const Expr& t)
      {
        return t._is_null();
      }
    };

    template <typename Expr>
    struct rhs_is_null_t<Expr, typename std::enable_if<is_result_field_t<Expr>::value, void>::type>
    {
      static bool _(const Expr& t)
      {
        return t.is_null();
      }
    };
  }

  template <typename Expr, bool TrivialValueIsNull>
  struct rhs_wrap_t
  {
    using _traits = typename Expr::_traits;
    using _nodes = detail::type_vector<Expr>;

    rhs_wrap_t(Expr expr) : _expr(expr)
    {
    }

    rhs_wrap_t(const rhs_wrap_t&) = default;
    rhs_wrap_t(rhs_wrap_t&&) = default;
    rhs_wrap_t& operator=(const rhs_wrap_t&) = default;
    rhs_wrap_t& operator=(rhs_wrap_t&&) = default;
    ~rhs_wrap_t() = default;

    bool _is_null() const
    {
      return (TrivialValueIsNull and detail::rhs_is_trivial_t<Expr>::_(_expr)) or detail::rhs_is_null_t<Expr>::_(_expr);
    }

    static constexpr bool _is_default()
    {
      return std::is_same<Expr, default_value_t>::value;
    }

    Expr _expr;
  };

  template <typename Context, typename Expr, bool TrivialValueIsNull>
  struct serializer_t<Context, rhs_wrap_t<Expr, TrivialValueIsNull>>
  {
    using _serialize_check = serialize_check_of<Context, Expr>;
    using T = rhs_wrap_t<Expr, TrivialValueIsNull>;

    static Context& _(const T& t, Context& context)
    {
      if (t._is_null())
      {
        context << "NULL";
      }
      else if (t._is_default())
      {
        context << "DEFAULT";
      }
      else
      {
        serialize(t._expr, context);
      }
      return context;
    }
  };
}

#endif

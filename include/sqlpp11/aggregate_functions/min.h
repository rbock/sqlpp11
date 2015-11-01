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

#ifndef SQLPP_MIN_H
#define SQLPP_MIN_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/char_sequence.h>

namespace sqlpp
{
  struct min_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "min_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T min;
        T& operator()()
        {
          return min;
        }
        const T& operator()() const
        {
          return min;
        }
      };
    };
  };

  template <typename Expr>
  struct min_t : public expression_operators<min_t<Expr>, value_type_of<Expr>>, public alias_operators<min_t<Expr>>
  {
    using _traits = make_traits<value_type_of<Expr>, tag::is_expression, tag::is_selectable>;
    using _nodes = detail::type_vector<Expr, aggregate_function>;
    using _can_be_null = std::true_type;
    using _is_aggregate_expression = std::true_type;

    using _auto_alias_t = min_alias_t;

    min_t(Expr expr) : _expr(expr)
    {
    }

    min_t(const min_t&) = default;
    min_t(min_t&&) = default;
    min_t& operator=(const min_t&) = default;
    min_t& operator=(min_t&&) = default;
    ~min_t() = default;

    Expr _expr;
  };

  template <typename Context, typename Expr>
  struct serializer_t<Context, min_t<Expr>>
  {
    using _serialize_check = serialize_check_of<Context, Expr>;
    using T = min_t<Expr>;

    static Context& _(const T& t, Context& context)
    {
      context << "MIN(";
      serialize(t._expr, context);
      context << ")";
      return context;
    }
  };

  template <typename T>
  auto min(T t) -> min_t<wrap_operand_t<T>>
  {
    static_assert(not contains_aggregate_function_t<wrap_operand_t<T>>::value,
                  "min() cannot be used on an aggregate function");
    static_assert(is_expression_t<wrap_operand_t<T>>::value, "min() requires an expression as argument");
    return {t};
  }
}

#endif

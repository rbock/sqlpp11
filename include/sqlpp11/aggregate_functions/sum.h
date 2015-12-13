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

#ifndef SQLPP_SUM_H
#define SQLPP_SUM_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/char_sequence.h>

namespace sqlpp
{
  struct sum_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "sum_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T sum;
        T& operator()()
        {
          return sum;
        }
        const T& operator()() const
        {
          return sum;
        }
      };
    };
  };

  template <typename Flag, typename Expr>
  struct sum_t : public expression_operators<sum_t<Flag, Expr>, value_type_of<Expr>>,
                 public alias_operators<sum_t<Flag, Expr>>
  {
    using _traits = make_traits<value_type_of<Expr>, tag::is_expression, tag::is_selectable>;
    using _nodes = detail::type_vector<Expr, aggregate_function>;
    using _can_be_null = std::true_type;
    using _is_aggregate_expression = std::true_type;

    static_assert(is_noop<Flag>::value or std::is_same<distinct_t, Flag>::value,
                  "sum() used with flag other than 'distinct'");
    static_assert(is_numeric_t<Expr>::value, "sum() requires a numeric expression as argument");

    using _auto_alias_t = sum_alias_t;

    sum_t(Expr expr) : _expr(expr)
    {
    }

    sum_t(const sum_t&) = default;
    sum_t(sum_t&&) = default;
    sum_t& operator=(const sum_t&) = default;
    sum_t& operator=(sum_t&&) = default;
    ~sum_t() = default;

    Expr _expr;
  };

  template <typename Context, typename Flag, typename Expr>
  struct serializer_t<Context, sum_t<Flag, Expr>>
  {
    using _serialize_check = serialize_check_of<Context, Flag, Expr>;
    using T = sum_t<Flag, Expr>;

    static Context& _(const T& t, Context& context)
    {
      context << "SUM(";
      if (std::is_same<distinct_t, Flag>::value)
      {
        serialize(Flag(), context);
        context << ' ';
        serialize_operand(t._expr, context);
      }
      else
        serialize(t._expr, context);

      context << ")";
      return context;
    }
  };

  template <typename T>
  auto sum(T t) -> sum_t<noop, wrap_operand_t<T>>
  {
    static_assert(not contains_aggregate_function_t<wrap_operand_t<T>>::value,
                  "sum() cannot be used on an aggregate function");
    static_assert(is_numeric_t<wrap_operand_t<T>>::value, "sum() requires a numeric expression as argument");
    return {t};
  }

  template <typename T>
  auto sum(const distinct_t&, T t) -> sum_t<distinct_t, wrap_operand_t<T>>
  {
    static_assert(not contains_aggregate_function_t<wrap_operand_t<T>>::value,
                  "sum() cannot be used on an aggregate function");
    static_assert(is_numeric_t<wrap_operand_t<T>>::value, "sum() requires a numeric expression as argument");
    return {t};
  }
}

#endif

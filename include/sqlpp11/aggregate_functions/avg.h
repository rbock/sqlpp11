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

#ifndef SQLPP_AVG_H
#define SQLPP_AVG_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/char_sequence.h>

namespace sqlpp
{
  struct avg_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "avg_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T avg;
        T& operator()()
        {
          return avg;
        }
        const T& operator()() const
        {
          return avg;
        }
      };
    };
  };

  template <typename Flag, typename Expr>
  struct avg_t : public expression_operators<avg_t<Flag, Expr>, floating_point>,
                 public alias_operators<avg_t<Flag, Expr>>
  {
    using _traits = make_traits<floating_point, tag::is_expression, tag::is_selectable>;
    using _nodes = detail::type_vector<Expr, aggregate_function>;
    using _can_be_null = std::true_type;
    using _is_aggregate_expression = std::true_type;

    static_assert(is_noop<Flag>::value or std::is_same<distinct_t, Flag>::value,
                  "avg() used with flag other than 'distinct'");
    static_assert(is_numeric_t<Expr>::value, "avg() requires a value expression as argument");

    using _auto_alias_t = avg_alias_t;

    avg_t(Expr expr) : _expr(expr)
    {
    }

    avg_t(const avg_t&) = default;
    avg_t(avg_t&&) = default;
    avg_t& operator=(const avg_t&) = default;
    avg_t& operator=(avg_t&&) = default;
    ~avg_t() = default;

    Expr _expr;
  };

  template <typename Context, typename Flag, typename Expr>
  struct serializer_t<Context, avg_t<Flag, Expr>>
  {
    using _serialize_check = serialize_check_of<Context, Flag, Expr>;
    using T = avg_t<Flag, Expr>;

    static Context& _(const T& t, Context& context)
    {
      context << "AVG(";
      if (std::is_same<distinct_t, Flag>::value)
      {
        serialize(Flag(), context);
        context << ' ';
        serialize_operand(t._expr, context);
      }
      else
      {
        serialize(t._expr, context);
      }
      context << ")";
      return context;
    }
  };

  template <typename T>
  auto avg(T t) -> avg_t<noop, wrap_operand_t<T>>
  {
    static_assert(not contains_aggregate_function_t<wrap_operand_t<T>>::value,
                  "avg() cannot be used on an aggregate function");
    static_assert(is_numeric_t<wrap_operand_t<T>>::value, "avg() requires a numeric value expression as argument");
    return {t};
  }

  template <typename T>
  auto avg(const distinct_t&, T t) -> avg_t<distinct_t, wrap_operand_t<T>>
  {
    static_assert(not contains_aggregate_function_t<wrap_operand_t<T>>::value,
                  "avg() cannot be used on an aggregate function");
    static_assert(is_numeric_t<wrap_operand_t<T>>::value, "avg() requires a numeric value expression as argument");
    return {t};
  }
}

#endif

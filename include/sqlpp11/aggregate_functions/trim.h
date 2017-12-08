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

#ifndef SQLPP11_AGGREGATE_FUNCTIONS_TRIM_H
#define SQLPP11_AGGREGATE_FUNCTIONS_TRIM_H

#include <sqlpp11/char_sequence.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/data_types/integral/data_type.h>

namespace sqlpp
{
  struct trim_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "trim_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T trim;
        T& operator()()
        {
          return trim;
        }
        const T& operator()() const
        {
          return trim;
        }
      };
    };
  };

  template <typename Flag, typename Expr>
  struct trim_t : public expression_operators<trim_t<Flag, Expr>, text>,
                   public alias_operators<trim_t<Flag, Expr>>
  {
    using _traits = make_traits<text, tag::is_expression, tag::is_selectable>;

    using _nodes = detail::type_vector<Expr, aggregate_function>;
    using _can_be_null = std::true_type;
    using _is_aggregate_expression = std::true_type;

    using _auto_alias_t = trim_alias_t;

    trim_t(const Expr expr) : _expr(expr)
    {
    }

    trim_t(const trim_t&) = default;
    trim_t(trim_t&&) = default;
    trim_t& operator=(const trim_t&) = default;
    trim_t& operator=(trim_t&&) = default;
    ~trim_t() = default;

    Expr _expr;
  };

  template <typename Context, typename Flag, typename Expr>
  struct serializer_t<Context, trim_t<Flag, Expr>>
  {
    using _serialize_check = serialize_check_of<Context, Flag, Expr>;
    using T = trim_t<Flag, Expr>;

    static Context& _(const T& t, Context& context)
    {
      context << "TRIM(";
      serialize(t._expr, context);
      context << ")";
      return context;
    }

  };

  template <typename T>
  auto trim(T t) -> trim_t<noop, wrap_operand_t<T>>
  {
    static_assert(not contains_aggregate_function_t<wrap_operand_t<T>>::value,
                  "trim() cannot be used on an aggregate function");
    static_assert(is_expression_t<wrap_operand_t<T>>::value, "trim() requires an expression as argument");
    return {t};
  }

}  // namespace sqlpp

#endif

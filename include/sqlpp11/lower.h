#pragma once

/*
 * Copyright (c) 2023, Roland Bock
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

#include <sqlpp11/char_sequence.h>
#include <sqlpp11/data_types/integral/data_type.h>
#include <sqlpp11/data_types/text/data_type.h>

namespace sqlpp
{
  struct lower_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "lower_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T lower;
        T& operator()()
        {
          return lower;
        }
        const T& operator()() const
        {
          return lower;
        }
      };
    };
  };

  template <typename Flag, typename Expr>
  struct lower_t : public expression_operators<lower_t<Flag, Expr>, text>, public alias_operators<lower_t<Flag, Expr>>
  {
    using _traits = make_traits<text, tag::is_expression, tag::is_selectable>;

    using _nodes = detail::type_vector<Expr>;
    using _can_be_null = can_be_null_t<Expr>;

    using _auto_alias_t = lower_alias_t;

    lower_t(const Expr expr) : _expr(expr)
    {
    }

    lower_t(const lower_t&) = default;
    lower_t(lower_t&&) = default;
    lower_t& operator=(const lower_t&) = default;
    lower_t& operator=(lower_t&&) = default;
    ~lower_t() = default;

    Expr _expr;
  };

  template <typename Context, typename Flag, typename Expr>
  Context& serialize(const lower_t<Flag, Expr>& t, Context& context)
  {
    context << "LOWER(";
    serialize_operand(t._expr, context);
    context << ")";
    return context;
  }

  template <typename T>
  auto lower(T t) -> lower_t<noop, wrap_operand_t<T>>
  {
    static_assert(is_text_t<wrap_operand_t<T>>::value, "lower() requires a text expression as argument");
    return {t};
  }

}  // namespace sqlpp

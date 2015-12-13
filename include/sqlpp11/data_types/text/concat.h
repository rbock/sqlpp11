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

#ifndef SQLPP_CONCAT_H
#define SQLPP_CONCAT_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/expression_operators.h>
#include <sqlpp11/alias_operators.h>
#include <sqlpp11/logic.h>
#include <sqlpp11/wrap_operand.h>

namespace sqlpp
{
  struct text;

  struct concat_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "concat_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T concat;
      };
    };
  };

  template <typename... Args>
  struct concat_t : public expression_operators<concat_t<Args...>, text>, public alias_operators<concat_t<Args...>>
  {
    using _traits = make_traits<text, tag::is_expression, tag::is_selectable>;
    using _nodes = detail::type_vector<Args...>;

    using _auto_alias_t = concat_alias_t;

    concat_t(Args... args) : _args(args...)
    {
    }

    concat_t(const concat_t&) = default;
    concat_t(concat_t&&) = default;
    concat_t& operator=(const concat_t&) = default;
    concat_t& operator=(concat_t&&) = default;
    ~concat_t() = default;

    std::tuple<Args...> _args;
  };

  template <typename Context, typename... Args>
  struct serializer_t<Context, concat_t<Args...>>
  {
    using _serialize_check = serialize_check_of<Context, Args...>;
    using T = concat_t<Args...>;

    static Context& _(const T& t, Context& context)
    {
      context << "(";
      interpret_tuple(t._args, "||", context);
      context << ")";
      return context;
    }
  };

  template <typename... Args>
  auto concat(Args... args) -> concat_t<Args...>
  {
    static_assert(sizeof...(Args) >= 2, "concat requires two arguments at least");
    static_assert(logic::all_t<is_text_t<wrap_operand_t<Args>>::value...>::value,
                  "at least one non-text argument detected in concat()");

    return {args...};
  }
}

#endif

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

#ifndef SQLPP_IN_H
#define SQLPP_IN_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/data_types/boolean.h>
#include <sqlpp11/in_fwd.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
  struct in_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "in_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T in;
      };
    };
  };

  template <typename Operand, typename... Args>
  struct in_t : public expression_operators<in_t<Operand, Args...>, boolean>,
                public alias_operators<in_t<Operand, Args...>>
  {
    using _traits = make_traits<boolean, tag::is_expression, tag::is_selectable>;
    using _nodes = detail::type_vector<Operand, Args...>;

    using _auto_alias_t = in_alias_t;

    in_t(Operand operand, Args... args) : _operand(operand), _args(args...)
    {
    }

    in_t(const in_t&) = default;
    in_t(in_t&&) = default;
    in_t& operator=(const in_t&) = default;
    in_t& operator=(in_t&&) = default;
    ~in_t() = default;

    Operand _operand;
    std::tuple<Args...> _args;
  };

  template <typename Context, typename Operand, typename... Args>
  struct serializer_t<Context, in_t<Operand, Args...>>
  {
    using _serialize_check = serialize_check_of<Context, Args...>;
    using T = in_t<Operand, Args...>;

    static Context& _(const T& t, Context& context)
    {
      serialize(t._operand, context);
      context << " IN(";
      if (sizeof...(Args) == 1)
        serialize(std::get<0>(t._args), context);
      else
        interpret_tuple(t._args, ',', context);
      context << ')';
      return context;
    }
  };

  template <typename Context, typename Operand>
  struct serializer_t<Context, in_t<Operand>>
  {
    using _serialize_check = consistent_t;
    using T = in_t<Operand>;

    static Context& _(const T&, Context& context)
    {
      serialize(boolean_operand{false}, context);
      return context;
    }
  };

  template <typename Container>
  struct value_list_t;

  template <typename Context, typename Operand, typename Container>
  struct serializer_t<Context, in_t<Operand, value_list_t<Container>>>
  {
    using _serialize_check = serialize_check_of<Context, value_list_t<Container>>;
    using T = in_t<Operand, value_list_t<Container>>;

    static Context& _(const T& t, Context& context)
    {
      const auto& value_list = std::get<0>(t._args);
      if (value_list._container.empty())
      {
        serialize(boolean_operand{false}, context);
      }
      else
      {
        serialize(t._operand, context);
        context << " IN(";
        serialize(value_list, context);
        context << ')';
      }
      return context;
    }
  };
}

#endif

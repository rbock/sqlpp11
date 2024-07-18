#pragma once

/*
Copyright (c) 2024, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <tuple>
#include <vector>

#include <sqlpp11/type_traits.h>
#include <sqlpp11/logic.h>

namespace sqlpp
{
  template<typename L, typename Operator, typename Container>
  struct in_expression
  {
    L l;
    Container container;
  };

  template <typename L, typename Operator, typename... Args>
  struct in_expression<L, Operator, std::tuple<Args...>>
  {
    L l;
    std::tuple<Args...> args;
  };

  template <typename L, typename... Args>
  using check_in_args =
      std::enable_if_t<(sizeof...(Args) != 0) and logic::all_t<values_are_comparable<L, Args>::value...>::value>;

  template <typename L, typename Operator, typename R>
  struct value_type_of<in_expression<L, Operator, std::vector<R>>>
      : std::conditional<sqlpp::is_optional<value_type_of_t<L>>::value or sqlpp::is_optional<value_type_of_t<R>>::value,
                         sqlpp::compat::optional<boolean>,
                         boolean>
  {
  };

  template <typename L, typename Operator, typename... Args>
  struct value_type_of<in_expression<L, Operator, std::tuple<Args...>>>
      : std::conditional<sqlpp::is_optional<value_type_of_t<L>>::value or
                             logic::any_t<sqlpp::is_optional<value_type_of_t<Args>>::value...>::value,
                         sqlpp::compat::optional<boolean>,
                         boolean>
  {
  };

  /*

  template <typename L, typename... Args>
  struct nodes_of<in_t<L, Args...>>
  {
    using type = type_vector<L, Args...>;
  };

  template <typename L, typename... Args>
  constexpr auto in(L l, Args... args)
      -> std::enable_if_t<((sizeof...(Args) > 0) and ... and values_are_compatible_v<L, Args>), in_t<L, Args...>>
  {
    return in_t<L, Args...>{l, std::tuple{args...}};
  }

  template <typename L, typename... Args>
  constexpr auto requires_braces_v<in_t<L, Args...>> = true;

  template <typename Context, typename L, typename... Args>
  [[nodiscard]] auto to_sql_string(Context& context, const in_t<L, Args...>& t)
  {
    if constexpr (sizeof...(Args) == 1)
    {
      return to_sql_string(context, embrace(t.l)) + " IN(" + to_sql_string(context, std::get<0>(t.args)) + ")";
    }
    else
    {
      return to_sql_string(context, embrace(t.l)) + " IN(" + tuple_to_sql_string(context, ", ", t.args) + ")";
    }
  }
  */

  struct operator_in
  {
    static constexpr auto symbol = " IN ";
  };

#warning: something.in(select(...)); should be suppported as is, need to test
  template <typename L, typename... Args, typename = check_in_args<L, Args...>>
  constexpr auto in(L l, std::tuple<Args...> args) -> in_expression<L, operator_in, std::tuple<Args...>>
  {
    return {std::move(l), std::move(args)};
  }

  template <typename L, typename... Args, typename = check_in_args<L, Args...>>
  constexpr auto in(L l, Args... args) -> in_expression<L, operator_in, std::tuple<Args...>>
  {
    return {std::move(l), std::make_tuple(std::move(args)...)};
  }

  template <typename L, typename Arg, typename = check_in_args<L, Arg>>
  constexpr auto in(L l, std::vector<Arg> args) -> in_expression<L, operator_in, std::vector<Arg>>
  {
    return {std::move(l), std::move(args)};
  }

  struct operator_not_in
  {
    static constexpr auto symbol = " NOT IN ";
  };

#if 0 // original serialize implementation
  template <typename Context, typename Operand, typename Arg, typename... Args>
  Context& serialize(Context& context, const in_t<Operand, Arg, Args...>& t)
  {
    serialize(context, t._operand);
    context << " IN(";
    if (sizeof...(Args) == 0)
    {
      serialize(context, std::get<0>(t._args));
    }
    else
    {
      interpret_tuple(t._args, ',', context);
    }
    context << ')';
    return context;
  }

  template <typename Context, typename Operand>
  Context& serialize(Context& context, const in_t<Operand>&)
  {
    serialize(context, boolean_operand{false});
    return context;
  }

  template <typename Container>
  struct value_list_t;

  template <typename Context, typename Operand, typename Container>
  Context& serialize(Context& context, const in_t<Operand, value_list_t<Container>>& t)
  {
    const auto& value_list = std::get<0>(t._args);
    if (value_list._container.empty())
    {
      serialize(context, boolean_operand{false});
    }
    else
    {
      serialize(context, t._operand);
      context << " IN(";
      serialize(context, value_list);
      context << ')';
    }
    return context;
  }

#endif

#warning: something.not_in(select(...)); should be suppported as is
  template <typename L, typename... Args, typename = check_in_args<L, Args...>>
  constexpr auto not_in(L l, std::tuple<Args...> args) -> in_expression<L, operator_not_in, std::tuple<Args...>>
  {
    return {std::move(l), std::move(args)};
  }

  template <typename L, typename... Args, typename = check_in_args<L, Args...>>
  constexpr auto not_in(L l, Args... args) -> in_expression<L, operator_not_in, std::tuple<Args...>>
  {
    return {std::move(l), std::make_tuple(std::move(args)...)};
  }

  template <typename L, typename Arg, typename = check_in_args<L, Arg>>
  constexpr auto not_in(L l, std::vector<Arg> args) -> in_expression<L, operator_not_in, std::vector<Arg>>
  {
    return {std::move(l), std::move(args)};
  }


}  // namespace sqlpp

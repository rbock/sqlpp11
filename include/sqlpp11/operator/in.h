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

#include <sqlpp11/in_expression.h>

namespace sqlpp
{
  struct operator_in
  {
    static constexpr auto symbol = " IN ";
  };

#warning: something.in(select(...)); should be suppported as is
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

}  // namespace sqlpp

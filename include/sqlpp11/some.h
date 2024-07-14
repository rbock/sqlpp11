#pragma once

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

#include <sqlpp11/statement_fwd.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/char_sequence.h>

namespace sqlpp
{
  template <typename Select>
  struct some_t
  {
    using _traits = make_traits<value_type_of_t<Select>, tag::is_multi_expression>;
    using _nodes = detail::type_vector<Select>;

    some_t(Select select) : _select(select)
    {
    }

    some_t(const some_t&) = default;
    some_t(some_t&&) = default;
    some_t& operator=(const some_t&) = default;
    some_t& operator=(some_t&&) = default;
    ~some_t() = default;

    Select _select;
  };

  template<typename Select>
  struct value_type_of<some_t<Select>> : value_type_of<Select>
  {
  };

  template <typename Context, typename Select>
  Context& serialize(const some_t<Select>& t, Context& context)
  {
    context << "SOME";
    serialize_operand(t._select, context);
    return context;
  }

#warning : Need tests
  template <typename Select>
  using check_some_args = std::enable_if_t<has_value_type<Select>::value>;

  template <typename ... Policies, typename = check_some_args<statement_t<Policies...>>>
  auto some(statement_t<Policies...> t) -> some_t<statement_t<Policies...>>
  {
    return {std::move(t)};
  }
}  // namespace sqlpp

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

#ifndef SQLPP_JOIN_H
#define SQLPP_JOIN_H

#include <sqlpp11/join_types.h>
#include <sqlpp11/cross_join.h>
#include <sqlpp11/on.h>

namespace sqlpp
{
  template <typename CrossJoin, typename On>
  struct join_t
  {
    using _traits = make_traits<no_value_t, tag::is_table, tag::is_join>;
    using _nodes = detail::type_vector<CrossJoin, On>;
    using _can_be_null = std::false_type;
    using _provided_tables = provided_tables_of<CrossJoin>;
    using _required_tables = detail::make_difference_set_t<required_tables_of<On>, _provided_tables>;

    static_assert(is_cross_join_t<CrossJoin>::value, "lhs argument for join() has to be a table or join");
    static_assert(is_on_t<On>::value, "invalid on expression in join().on()");

    static_assert(required_tables_of<CrossJoin>::size::value == 0, "joined tables must not depend on other tables");
    static_assert(detail::is_subset_of<required_tables_of<On>, provided_tables_of<CrossJoin>>::value,
                  "on() condition must not depend on other tables");

    template <typename T>
    auto join(T t) const -> decltype(::sqlpp::join(*this, t))
    {
      return ::sqlpp::join(*this, t);
    }

    template <typename T>
    auto inner_join(T t) const -> decltype(::sqlpp::inner_join(*this, t))
    {
      return ::sqlpp::inner_join(*this, t);
    }

    template <typename T>
    auto left_outer_join(T t) const -> decltype(::sqlpp::left_outer_join(*this, t))
    {
      return ::sqlpp::left_outer_join(*this, t);
    }

    template <typename T>
    auto right_outer_join(T t) const -> decltype(::sqlpp::right_outer_join(*this, t))
    {
      return ::sqlpp::right_outer_join(*this, t);
    }

    template <typename T>
    auto outer_join(T t) const -> decltype(::sqlpp::outer_join(*this, t))
    {
      return ::sqlpp::outer_join(*this, t);
    }

    CrossJoin _cross_join;
    On _on;
  };

  template <typename Context, typename CrossJoin, typename On>
  struct serializer_t<Context, join_t<CrossJoin, On>>
  {
    using _serialize_check = serialize_check_of<Context, CrossJoin, On>;
    using T = join_t<CrossJoin, On>;

    static Context& _(const T& t, Context& context)
    {
      serialize(t._cross_join, context);
      serialize(t._on, context);
      return context;
    }
  };
}

#endif

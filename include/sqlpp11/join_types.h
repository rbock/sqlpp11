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

#ifndef SQLPP_JOIN_TYPES_H
#define SQLPP_JOIN_TYPES_H

#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  struct inner_join_t
  {
    template <typename Lhs, typename Rhs>
    using _provided_outer_tables =
        detail::make_joined_set_t<provided_outer_tables_of<Lhs>, provided_outer_tables_of<Rhs>>;

    static constexpr const char* _name = " INNER";
  };
  struct outer_join_t
  {
    template <typename Lhs, typename Rhs>
    using _provided_outer_tables = detail::make_joined_set_t<provided_tables_of<Lhs>, provided_tables_of<Rhs>>;

    static constexpr const char* _name = " OUTER";
  };
  struct left_outer_join_t
  {
    template <typename Lhs, typename Rhs>
    using _provided_outer_tables = detail::make_joined_set_t<provided_outer_tables_of<Lhs>, provided_tables_of<Rhs>>;

    static constexpr const char* _name = " LEFT OUTER";
  };
  struct right_outer_join_t
  {
    template <typename Lhs, typename Rhs>
    using _provided_outer_tables = detail::make_joined_set_t<provided_tables_of<Lhs>, provided_outer_tables_of<Rhs>>;

    static constexpr const char* _name = " RIGHT OUTER";
  };
  struct cross_join_t
  {
    template <typename Lhs, typename Rhs>
    using _provided_outer_tables =
        detail::make_joined_set_t<provided_outer_tables_of<Lhs>, provided_outer_tables_of<Rhs>>;

    static constexpr const char* _name = " CROSS";
  };
}

#endif

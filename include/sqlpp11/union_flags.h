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

#ifndef SQLPP_UNION_FLAGS_H
#define SQLPP_UNION_FLAGS_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/interpret_tuple.h>
#include <tuple>

namespace sqlpp
{
  // standard select flags
  struct union_all_t
  {
    using _traits = make_traits<no_value_t, tag::is_union_flag>;
    using _nodes = detail::type_vector<>;
  };

  template <typename Context>
  struct serializer_t<Context, union_all_t>
  {
    using _serialize_check = consistent_t;

    static Context& _(const union_all_t&, Context& context)
    {
      context << "ALL";
      return context;
    }
  };

  struct union_distinct_t
  {
    using _traits = make_traits<no_value_t, tag::is_union_flag>;
    using _nodes = detail::type_vector<>;
  };

  template <typename Context>
  struct serializer_t<Context, union_distinct_t>
  {
    using _serialize_check = consistent_t;

    static Context& _(const union_distinct_t&, Context& context)
    {
      return context;
    }
  };
}

#endif

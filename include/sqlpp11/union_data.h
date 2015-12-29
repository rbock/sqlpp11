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

#ifndef SQLPP_UNION_DATA_H
#define SQLPP_UNION_DATA_H

#include <sqlpp11/serializer.h>

namespace sqlpp
{
  template <typename Database, typename Flag, typename Lhs, typename Rhs>
  struct union_data_t
  {
    union_data_t(Lhs lhs, Rhs rhs) : _lhs(lhs), _rhs(rhs)
    {
    }

    union_data_t(const union_data_t&) = default;
    union_data_t(union_data_t&&) = default;
    union_data_t& operator=(const union_data_t&) = default;
    union_data_t& operator=(union_data_t&&) = default;
    ~union_data_t() = default;

    Lhs _lhs;
    Rhs _rhs;
  };

  // Interpreters
  template <typename Context, typename Database, typename Flag, typename Lhs, typename Rhs>
  struct serializer_t<Context, union_data_t<Database, Flag, Lhs, Rhs>>
  {
    using _serialize_check = serialize_check_of<Context, Lhs, Rhs>;
    using T = union_data_t<Database, Flag, Lhs, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      serialize(t._lhs, context);
      context << " UNION ";
      serialize(Flag{}, context);
      context << " ";
      serialize(t._rhs, context);
      return context;
    }
  };
}

#endif

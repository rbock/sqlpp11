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

#ifndef SQLPP_SIMPLE_COLUMN_H
#define SQLPP_SIMPLE_COLUMN_H

#include <sqlpp11/serializer.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  template <typename Column>
  struct simple_column_t
  {
    using _column_t = Column;
    _column_t _column;

    // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
    simple_column_t() = default;
    simple_column_t(const _column_t& column) : _column{column}
    {
    }

    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;
  };

  template <typename Context, typename Column>
  struct serializer_t<Context, simple_column_t<Column>>
  {
    using _serialize_check = serialize_check_of<Context, Column>;
    using T = simple_column_t<Column>;

    static Context& _(const T&, Context& context)
    {
      context << name_of<typename T::_column_t>::char_ptr();
      return context;
    }
  };

  template <typename Column>
  simple_column_t<Column> simple_column(Column c)
  {
    return {c};
  }
}

#endif

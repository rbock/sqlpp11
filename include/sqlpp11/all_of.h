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

#ifndef SQLPP_ALL_OF_H
#define SQLPP_ALL_OF_H

#include <sqlpp11/alias.h>
#include <sqlpp11/interpret.h>
#include <sqlpp11/multi_column.h>
#include <sqlpp11/portable_static_assert.h>

namespace sqlpp
{
  template <typename Table>
  struct all_of_t
  {
    using _column_tuple_t = typename Table::_column_tuple_t;

    template <typename AliasProvider>
    detail::copy_tuple_args_t<multi_column_alias_t, AliasProvider, _column_tuple_t> as(const AliasProvider& alias)
    {
      return multi_column(_column_tuple_t{}).as(alias);
    }
  };

  template <typename Table>
  auto all_of(Table) -> all_of_t<Table>
  {
    return {};
  }

  SQLPP_PORTABLE_STATIC_ASSERT(assert_no_stand_alone_all_of_t, "all_of(table) seems to be used outside of select");

  template <typename Context, typename Table>
  struct serializer_t<Context, all_of_t<Table>>
  {
    using _serialize_check = assert_no_stand_alone_all_of_t;
    using T = all_of_t<Table>;

    static Context& _(const T&, const Context&)
    {
      _serialize_check{};
    }
  };
}

#endif

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

#ifndef SQLPP_SORT_ORDER_H
#define SQLPP_SORT_ORDER_H

#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
  struct no_value_t;

  enum class sort_type
  {
    asc,
    desc
  };

  template <typename Expression, sort_type SortType>
  struct sort_order_t
  {
    using _traits = make_traits<no_value_t, tag::is_sort_order>;
    using _nodes = detail::type_vector<Expression>;

    Expression _expression;
  };

  template <typename Context, typename Expression, sort_type SortType>
  struct serializer_t<Context, sort_order_t<Expression, SortType>>
  {
    using _serialize_check = serialize_check_of<Context, Expression>;
    using T = sort_order_t<Expression, SortType>;

    static Context& _(const T& t, Context& context)
    {
      serialize_operand(t._expression, context);
      switch (SortType)
      {
        case sort_type::asc:
          context << " ASC";
          break;
        default:
          context << " DESC";
          break;
      }
      return context;
    }
  };
}

#endif

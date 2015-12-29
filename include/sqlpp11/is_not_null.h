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

#ifndef SQLPP_IS_NOT_NULL_H
#define SQLPP_IS_NOT_NULL_H

#include <sqlpp11/data_types/boolean.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
  struct is_not_null_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "is_not_null_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T is_not_null;
      };
    };
  };

  template <typename Operand>
  struct is_not_null_t : public expression_operators<is_not_null_t<Operand>, boolean>,
                         public alias_operators<is_not_null_t<Operand>>
  {
    using _traits = make_traits<boolean, tag::is_expression, tag::is_selectable>;
    using _nodes = detail::type_vector<Operand>;

    using _auto_alias_t = is_not_null_alias_t;

    is_not_null_t(Operand operand) : _operand(operand)
    {
    }

    is_not_null_t(const is_not_null_t&) = default;
    is_not_null_t(is_not_null_t&&) = default;
    is_not_null_t& operator=(const is_not_null_t&) = default;
    is_not_null_t& operator=(is_not_null_t&&) = default;
    ~is_not_null_t() = default;

    Operand _operand;
  };

  template <typename Context, typename Operand>
  struct serializer_t<Context, is_not_null_t<Operand>>
  {
    using _serialize_check = serialize_check_of<Context, Operand>;
    using T = is_not_null_t<Operand>;

    static Context& _(const T& t, Context& context)
    {
      serialize_operand(t._operand, context);
      context << " IS NOT NULL";
      return context;
    }
  };
}

#endif

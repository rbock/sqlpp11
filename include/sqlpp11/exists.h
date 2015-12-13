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

#ifndef SQLPP_EXISTS_H
#define SQLPP_EXISTS_H

#include <sqlpp11/char_sequence.h>
#include <sqlpp11/data_types/boolean.h>

namespace sqlpp
{
  struct exists_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "exists_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T exists;
        T& operator()()
        {
          return exists;
        }
        const T& operator()() const
        {
          return exists;
        }
      };
    };
  };

  template <typename Select>
  struct exists_t : public expression_operators<exists_t<Select>, boolean>, public alias_operators<exists_t<Select>>
  {
    using _traits = make_traits<boolean, tag::is_expression, tag::is_selectable>;
    using _nodes = detail::type_vector<Select>;

    static_assert(is_select_t<Select>::value, "exists() requires a select expression as argument");

    using _auto_alias_t = exists_alias_t;

    exists_t(Select select) : _select(select)
    {
    }

    exists_t(const exists_t&) = default;
    exists_t(exists_t&&) = default;
    exists_t& operator=(const exists_t&) = default;
    exists_t& operator=(exists_t&&) = default;
    ~exists_t() = default;

    Select _select;
  };

  template <typename Context, typename Select>
  struct serializer_t<Context, exists_t<Select>>
  {
    using _serialize_check = serialize_check_of<Context, Select>;
    using T = exists_t<Select>;

    static Context& _(const T& t, Context& context)
    {
      context << "EXISTS(";
      serialize(t._select, context);
      context << ")";
      return context;
    }
  };

  template <typename T>
  auto exists(T t) -> exists_t<wrap_operand_t<T>>
  {
    static_assert(is_select_t<wrap_operand_t<T>>::value, "exists() requires a select expression as argument");
    return {t};
  }
}

#endif

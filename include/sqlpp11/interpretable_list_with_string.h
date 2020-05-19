/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list_with_string of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list_with_string of conditions and the following disclaimer in the documentation and/or
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

#ifndef SQLPP11_INTERPRETABLE_LIST_WITH_STRING_H
#define SQLPP11_INTERPRETABLE_LIST_WITH_STRING_H

#include <sqlpp11/interpretable.h>
#include <vector>
#include <string>

namespace sqlpp
{
  template <typename Db>
  struct interpretable_list_with_string_t
  {
    struct elem_t {
        interpretable_t<Db> first;
        std::string second;
    };

    std::vector<elem_t> _serializables;

    std::size_t size() const
    {
      return _serializables.size();
    }

    bool empty() const
    {
      return _serializables.empty();
    }

    template <typename Expr>
    void emplace_back(Expr expr, const std::string& what)
    {
      _serializables.emplace_back(elem_t{expr, what});
    }
  };

  template <>
  struct interpretable_list_with_string_t<void>
  {
    static constexpr std::size_t size()
    {
      return 0;
    }

    static constexpr bool empty()
    {
      return true;
    }
  };

  template <typename Context, typename List>
  struct serializable_list_with_string_interpreter_t
  {
    using T = List;

    static Context& _(const T& t, Context& context)
    {
      bool first = true;
      for (const auto entry : t._serializables)
      {
        if (not first)
        {
          context << entry.second;
        }
        first = false;
        serialize(entry.first, context);
      }
      return context;
    }
  };

  template <typename Context>
  struct serializable_list_with_string_interpreter_t<Context, interpretable_list_with_string_t<void>>
  {
    using T = interpretable_list_with_string_t<void>;

    static Context& _(const T& /*unused*/, Context& context)
    {
      return context;
    }
  };

  template <typename T, typename Context>
  auto interpret_list_with_string(const T& t, Context& context)
      -> decltype(serializable_list_with_string_interpreter_t<Context, T>::_(t, context))
  {
    return serializable_list_with_string_interpreter_t<Context, T>::_(t, context);
  }
}  // namespace sqlpp

#endif

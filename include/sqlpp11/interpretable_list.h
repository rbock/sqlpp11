#pragma once

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

#include <sqlpp11/interpretable.h>
#include <vector>

namespace sqlpp
{
  template <typename Db>
  struct interpretable_list_t
  {
    std::vector<interpretable_t<Db>> _serializables;

    std::size_t size() const
    {
      return _serializables.size();
    }

    bool empty() const
    {
      return _serializables.empty();
    }

    template <typename Expr>
    void emplace_back(Expr expr)
    {
      _serializables.emplace_back(expr);
    }
  };

  template <>
  struct interpretable_list_t<void>
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

  template <typename Db, typename Separator, typename Context>
  auto interpret_list(const interpretable_list_t<Db>& t, const Separator& separator, Context& context)
      -> Context&
  {
    bool first = true;
    for (const auto& entry : t._serializables)
    {
      if (not first)
      {
        context << separator;
      }
      first = false;
      serialize(entry, context);
      }
      return context;
  }

  template <typename Separator, typename Context>
  auto interpret_list(const interpretable_list_t<void>&, const Separator&, Context& context)
      -> Context&
  {
      return context;
  }
}  // namespace sqlpp

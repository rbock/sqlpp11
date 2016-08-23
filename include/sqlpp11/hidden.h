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

#ifndef SQLPP_HIDDEN_H
#define SQLPP_HIDDEN_H

namespace sqlpp
{
  template <typename Clause>
  struct hidden_t : Clause
  {
    hidden_t(Clause clause) : Clause(clause)
    {
    }

    hidden_t(const hidden_t&) = default;
    hidden_t(hidden_t&&) = default;
    hidden_t& operator=(const hidden_t&) = default;
    hidden_t& operator=(hidden_t&&) = default;
    ~hidden_t() = default;
  };

  template <typename Context, typename Clause>
  struct serializer_t<Context, hidden_t<Clause>>
  {
    using _serialize_check = consistent_t;
    using T = hidden_t<Clause>;

    static Context& _(const T&, Context& context)
    {
      return context;
    }
  };

  template <typename Clause>
  auto hidden(Clause part) -> hidden_t<Clause>
  {
    return {part};
  }
}
#endif

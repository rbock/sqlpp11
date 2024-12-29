#pragma once

/**
 * Copyright © 2014-2019, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/core/to_sql_string.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  namespace postgresql
  {
    template <typename OnConflict>
    struct on_conflict_do_nothing_t
    {
      on_conflict_do_nothing_t(OnConflict on_conflict) : _on_conflict(on_conflict)
      {
      }

      on_conflict_do_nothing_t(const on_conflict_do_nothing_t&) = default;
      on_conflict_do_nothing_t(on_conflict_do_nothing_t&&) = default;
      on_conflict_do_nothing_t& operator=(const on_conflict_do_nothing_t&) = default;
      on_conflict_do_nothing_t& operator=(on_conflict_do_nothing_t&&) = default;
      ~on_conflict_do_nothing_t() = default;

      OnConflict _on_conflict;

    };

    // Serialization
    template <typename OnConflict>
    auto to_sql_string(postgresql::context_t& context, const postgresql::on_conflict_do_nothing_t<OnConflict>& t) -> std::string
    {
#warning: need tests
      return to_sql_string(context, t._on_conflict) + " DO NOTHING";
    }
  }  // namespace postgresql

  template <typename Statement, typename ConflictTarget>
  struct consistency_check<Statement, postgresql::on_conflict_do_nothing_t<ConflictTarget>>
  {
    using type = consistent_t;
  };

}  // namespace sqlpp

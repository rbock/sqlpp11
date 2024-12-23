#pragma once

/*
 * Copyright (c) 2017, Serge Robyns
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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  struct for_update_t
  {
  };

  template <>
  struct is_clause<for_update_t> : public std::true_type
  {
  };

  template<typename Statement>
    struct consistency_check<Statement, for_update_t> { using type = consistent_t; };


  struct no_for_update_t
  {
  };

  template <typename Statement>
  struct clause_base<no_for_update_t, Statement> : public clause_data<no_for_update_t, Statement>
  {
    using clause_data<no_for_update_t, Statement>::clause_data;

    auto for_update() const -> decltype(new_statement(*this, for_update_t{}))
    {
      return new_statement(*this, for_update_t{});
    }
  };

  template<typename Statement>
    struct consistency_check<Statement, no_for_update_t> { using type = consistent_t; };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const for_update_t&) -> std::string
  {
    return  " FOR UPDATE";
  }

  inline auto for_update() -> decltype(statement_t<no_for_update_t>().for_update())
  {
    return statement_t<no_for_update_t>().for_update();
  }
}  // namespace sqlpp

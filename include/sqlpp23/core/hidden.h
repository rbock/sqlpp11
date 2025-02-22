#pragma once

/*
 * Copyright (c) 2013-2017, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
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

#include <sqlpp23/core/query/statement_fwd.h>
#include <sqlpp23/core/result_type_provider.h>
#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename Clause> struct hidden_t {};

template <typename Clause>
struct is_clause<hidden_t<Clause>> : public std::true_type {};

template <typename Statement, typename Clause>
struct consistency_check<Statement, hidden_t<Clause>> {
  using type = consistent_t;
};

template <typename Context, typename Clause>
auto to_sql_string(Context &, const hidden_t<Clause> &) -> std::string {
  return {};
}

template <typename... Clauses>
auto hidden(const statement_t<Clauses...> &)
    -> hidden_t<result_type_provider_t<Clauses...>> {
  return {};
}

template <typename... Clauses>
auto with_result_type_of(const statement_t<Clauses...> &)
    -> hidden_t<result_type_provider_t<Clauses...>> {
  return {};
}

} // namespace sqlpp

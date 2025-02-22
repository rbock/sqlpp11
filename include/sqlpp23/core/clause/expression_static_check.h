#pragma once

/*
 * Copyright (c) 2025, Roland Bock
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

#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
namespace detail {
// If an expression (or sub-expression) is statically selected, the respective
// table needs to be statically provided, too. Note that the select could be a
// sub-select that refers to tables provided by the enclosing query. We cannot
// know if those are dynamically or statically provided.
template <typename Statement, typename Expression, typename Assert>
struct expression_static_check;
template <typename Statement, typename Expression, typename Assert>
using expression_static_check_t =
    typename expression_static_check<Statement, Expression, Assert>::type;

template <typename Statement, typename Expression, typename Assert>
struct expression_static_check {
  static constexpr bool uses_external_tables =
      not Statement::template _no_unknown_tables<Expression>;

  using type = std::conditional_t<
      uses_external_tables,
      // Drill down into nodes to separate internal from external table
      // dependencies.
      expression_static_check_t<Statement, nodes_of_t<Expression>, Assert>,
      // If no external tables are used, then statically required tables also
      // need to be provided statically.
      static_check_t<Statement::template _no_unknown_static_tables<Expression>,
                     Assert>>;
};

template <typename Statement, typename... Nodes, typename Assert>
struct expression_static_check<Statement, detail::type_vector<Nodes...>,
                               Assert> {
  using type = static_combined_check_t<
      expression_static_check_t<Statement, Nodes, Assert>...>;
};
} // namespace detail

} // namespace sqlpp

#pragma once

/*
 * Copyright (c) 2013-2020, Roland Bock, MacDue
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

#include <sqlpp11/data_types/no_value.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/serialize.h>

#include <utility>

namespace sqlpp
{
  template <typename ValueType, typename Expr>
  struct parameterized_verbatim_t : public expression_operators<verbatim_t<ValueType>, ValueType>,
                                    public alias_operators<verbatim_t<ValueType>>
  {
    using _traits = make_traits<ValueType, tag::is_expression>;
    using _nodes = detail::type_vector<Expr>;
    using _can_be_null =
        std::true_type;  // since we do not know what's going on inside the verbatim, we assume it can be null

    parameterized_verbatim_t(
        const Expr expr, std::string verbatim_lhs, std::string verbatim_rhs)
      : _expr(expr), _verbatim_lhs(std::move(verbatim_lhs)), _verbatim_rhs(std::move(verbatim_rhs)) { }

    parameterized_verbatim_t(const parameterized_verbatim_t&) = default;
    parameterized_verbatim_t(parameterized_verbatim_t&&) = default;
    parameterized_verbatim_t& operator=(const parameterized_verbatim_t&) = default;
    parameterized_verbatim_t& operator=(parameterized_verbatim_t&&) = default;
    ~parameterized_verbatim_t() = default;

    Expr _expr;
    std::string _verbatim_lhs, _verbatim_rhs;
  };

  template <typename Context, typename ValueType, typename Expr>
  Context& serialize(const parameterized_verbatim_t<ValueType, Expr>& t, Context& context)
  {
    context << t._verbatim_lhs;
    serialize(t._expr, context);
    context << t._verbatim_rhs;
    return context;
  }

  template <typename ValueType, typename Expr>
  auto parameterized_verbatim(std::string lhs, Expr expr, std::string rhs)
    -> parameterized_verbatim_t<ValueType, wrap_operand_t<Expr>>
  {
    static_assert(is_expression_t<wrap_operand_t<Expr>>::value, "parameterized_verbatim() requires an expression as argument");
    return {expr, lhs, rhs};
  }

}  // namespace sqlpp

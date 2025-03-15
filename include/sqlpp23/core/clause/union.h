#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <sqlpp23/core/clause/union_flags.h>
#include <sqlpp23/core/logic.h>
#include <sqlpp23/core/query/result_row.h>
#include <sqlpp23/core/query/statement.h>
#include <sqlpp23/core/tuple_to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
// There is no order by or limit or offset in union, use it as a pseudo table to
// do that.

template <typename Flag, typename Lhs, typename Rhs> struct union_t {
  union_t(Lhs lhs, Rhs rhs) : _lhs(lhs), _rhs(rhs) {}

  union_t(const union_t &) = default;
  union_t(union_t &&) = default;
  union_t &operator=(const union_t &) = default;
  union_t &operator=(union_t &&) = default;
  ~union_t() = default;

  Lhs _lhs;
  Rhs _rhs;
};

  template <typename Context, typename Flag, typename Lhs, typename Rhs>
  auto to_sql_string(Context& context, const union_t<Flag, Lhs, Rhs>& t) -> std::string {
    if constexpr (is_dynamic<Rhs>::value) {
      if (t._rhs.has_value()) {
        // Note: Temporary required to enforce parameter ordering.
        auto ret_val = to_sql_string(context, t._lhs) + " UNION ";
        ret_val += to_sql_string(context, Flag{});
        return ret_val += to_sql_string(context, t._rhs.value());
      }
      return to_sql_string(context, t._lhs);
    } else {
      // Note: Temporary required to enforce parameter ordering.
      auto ret_val = to_sql_string(context, t._lhs) + " UNION ";
      ret_val += to_sql_string(context, Flag{});
      return ret_val += to_sql_string(context, t._rhs);
    }
  }

template <typename Flag, typename Lhs, typename Rhs>
struct has_result_row<union_t<Flag, Lhs, Rhs>> : public std::true_type {};

template <typename Statement, typename Flag, typename Lhs, typename Rhs>
struct result_row_of<Statement, union_t<Flag, Lhs, Rhs>> {
  using type = get_result_row_t<Lhs>;
};

template <typename Flag, typename Lhs, typename Rhs>
struct result_methods_of<union_t<Flag, Lhs, Rhs>> {
  using type = result_methods_of_t<Lhs>;
};

template <typename Flag, typename Lhs, typename Rhs>
struct is_clause<union_t<Flag, Lhs, Rhs>> : public std::true_type {};

template <typename Statement, typename Flag, typename Lhs, typename Rhs>
struct consistency_check<Statement, union_t<Flag, Lhs, Rhs>> {
  using type = static_combined_check_t<statement_consistency_check_t<Lhs>,
                                       statement_consistency_check_t<Lhs>>;
};

template <typename Statement, typename Flag, typename Lhs, typename Rhs>
struct prepare_check<Statement, union_t<Flag, Lhs, Rhs>> {
  using type = static_combined_check_t<statement_prepare_check_t<Lhs>,
                                       statement_prepare_check_t<Lhs>>;
};

template <typename Statement, typename Flag, typename Lhs, typename Rhs>
struct run_check<Statement, union_t<Flag, Lhs, Rhs>> {
  using type = static_combined_check_t<statement_run_check_t<Lhs>,
                                       statement_run_check_t<Lhs>>;
};

template <typename Flag, typename Lhs, typename Rhs>
struct is_result_clause<union_t<Flag, Lhs, Rhs>> : public std::true_type {};

SQLPP_WRAPPED_STATIC_ASSERT(
    assert_union_lhs_is_select_t,
    "left hand side argument of a union has to be a select statement or union");
SQLPP_WRAPPED_STATIC_ASSERT(assert_union_rhs_is_select_t,
                            "right hand side argument of a union has to be a "
                            "select statement or union");
SQLPP_WRAPPED_STATIC_ASSERT(assert_union_result_rows_match_t,
                            "both arguments in a union have to have the same "
                            "result columns (type and name)");

template <typename Lhs, typename Rhs>
using check_union_args_t = static_combined_check_t<
    static_check_t<has_result_row<Lhs>::value, assert_union_lhs_is_select_t>,
    static_check_t<has_result_row<Rhs>::value, assert_union_rhs_is_select_t>,
    statement_prepare_check_t<Lhs>, statement_prepare_check_t<Rhs>,
    static_check_t<is_result_compatible<get_result_row_t<Lhs>,
                                        get_result_row_t<Rhs>>::value,
                   assert_union_result_rows_match_t>>;

struct no_union_t {
  template <typename Statement, typename Rhs>
    requires(is_statement<remove_dynamic_t<Rhs>>::value)
  auto union_distinct(this Statement &&statement, Rhs rhs) {
    using S = std::decay_t<Statement>;
    check_union_args_t<S, remove_dynamic_t<Rhs>>::verify();

    return statement_t<union_t<union_distinct_t, S, Rhs>, no_union_t>{
        statement_constructor_arg<union_t<union_distinct_t, S, Rhs>,
                                  no_union_t>{
            union_t<union_distinct_t, S, Rhs>{
                std::forward<Statement>(statement), rhs},
            no_union_t{}}};
  }

  template <typename Statement, typename Rhs>
    requires(is_statement<remove_dynamic_t<Rhs>>::value)
  auto union_all(this Statement &&statement, Rhs rhs) {
    using S = std::decay_t<Statement>;
    check_union_args_t<S, remove_dynamic_t<Rhs>>::verify();

    return statement_t<union_t<union_all_t, S, Rhs>, no_union_t>{
        statement_constructor_arg<union_t<union_all_t, S, Rhs>, no_union_t>{
            union_t<union_all_t, S, Rhs>{std::forward<Statement>(statement),
                                         rhs},
            no_union_t{}}};
  }
};

  template <typename Context>
  auto to_sql_string(Context&, const no_union_t&) -> std::string {
    return "";
  }

template <typename Statement> struct consistency_check<Statement, no_union_t> {
  using type = consistent_t;
};

template <typename Lhs, typename Rhs>
  requires(is_statement<Lhs>::value and
           is_statement<remove_dynamic_t<Rhs>>::value)
auto union_all(Lhs lhs, Rhs rhs) {
  return lhs.union_all(std::move(rhs));
}

template <typename Lhs, typename Rhs>
  requires(is_statement<Lhs>::value and
           is_statement<remove_dynamic_t<Rhs>>::value)
auto union_distinct(Lhs lhs, Rhs rhs) {
  return lhs.union_distinct(std::move(rhs));
}

} // namespace sqlpp

#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/core/unconditional.h>
#include <sqlpp11/core/query/dynamic.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/compat/type_traits.h>
#include <sqlpp11/core/detail/type_vector.h>

namespace sqlpp
{
  // Join types.
  struct cross_join_t
  {
    static constexpr const char* _name = " CROSS JOIN ";
  };

  struct inner_join_t
  {
    static constexpr const char* _name = " INNER JOIN ";
  };

  struct left_outer_join_t
  {
    static constexpr const char* _name = " LEFT OUTER JOIN ";
  };

  struct right_outer_join_t
  {
    static constexpr const char* _name = " RIGHT OUTER JOIN ";
  };

  struct full_outer_join_t
  {
    static constexpr const char* _name = " FULL OUTER JOIN ";
  };

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  struct join_t;

  template <typename Lhs, typename JoinType, typename Rhs>
  struct pre_join_t;

  template <typename Lhs, typename Rhs>
  using check_join_args =
      sqlpp::enable_if_t<is_table<Lhs>::value and is_table<remove_dynamic_t<Rhs>>::value and
                         required_tables_of_t<Lhs>::is_empty() and required_tables_of_t<Rhs>::is_empty() and
                         sqlpp::detail::transform_set_t<provided_tables_of_t<Lhs>, make_char_sequence>::contains_none(
                             sqlpp::detail::transform_set_t<provided_tables_of_t<Rhs>, make_char_sequence>{})>;

  template <typename Lhs, typename Rhs, typename = check_join_args<Lhs, Rhs>>
  auto join(Lhs lhs, Rhs rhs) -> pre_join_t<table_ref_t<Lhs>, inner_join_t, table_ref_t<Rhs>>;

  template <typename Lhs, typename Rhs, typename = check_join_args<Lhs, Rhs>>
  auto inner_join(Lhs lhs, Rhs rhs) -> pre_join_t<table_ref_t<Lhs>, inner_join_t, table_ref_t<Rhs>>;

  template <typename Lhs, typename Rhs, typename = check_join_args<Lhs, Rhs>>
  auto left_outer_join(Lhs lhs, Rhs rhs) -> pre_join_t<table_ref_t<Lhs>, left_outer_join_t, table_ref_t<Rhs>>;

  template <typename Lhs, typename Rhs, typename = check_join_args<Lhs, Rhs>>
  auto right_outer_join(Lhs lhs, Rhs rhs) -> pre_join_t<table_ref_t<Lhs>, right_outer_join_t, table_ref_t<Rhs>>;

  template <typename Lhs, typename Rhs, typename = check_join_args<Lhs, Rhs>>
  auto full_outer_join(Lhs lhs, Rhs rhs) -> pre_join_t<table_ref_t<Lhs>, full_outer_join_t, table_ref_t<Rhs>>;

  template <typename Lhs, typename Rhs, typename = check_join_args<Lhs, Rhs>>
  auto cross_join(Lhs lhs, Rhs rhs) -> join_t<table_ref_t<Lhs>, cross_join_t, table_ref_t<Rhs>, unconditional_t>;

}  // namespace sqlpp

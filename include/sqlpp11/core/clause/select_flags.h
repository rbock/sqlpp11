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

#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <tuple>

namespace sqlpp
{
  // standard select flags
  struct all_t
  {
  };
  static constexpr all_t all = {};

  template <>
    struct is_select_flag<all_t> : public std::true_type {};

  template <typename Context>
  auto to_sql_string(Context& , const all_t&) -> std::string
  {
    return "ALL";
  }

  struct distinct_t
  {
  };
  static constexpr distinct_t distinct = {};

  template <>
    struct is_select_flag<distinct_t> : public std::true_type {};

  template <typename Context>
  auto to_sql_string(Context& , const distinct_t&) -> std::string
  {
    return "DISTINCT";
  }

}  // namespace sqlpp

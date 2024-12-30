/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/tests/core/constraints_helpers.h>
#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/core/compat/type_traits.h>

namespace
{
  SQLPP_CREATE_NAME_TAG(something);

  // Returns true if `select_flags(declval<Expressions>()...)` is a valid function call.
  template <typename TypeVector, typename = void>
  struct can_call_select_flags_with_impl : public std::false_type
  {
  };

  template <typename... Expressions>
  struct can_call_select_flags_with_impl<sqlpp::detail::type_vector<Expressions...>,
                                  sqlpp::void_t<decltype(sqlpp::select_flags(std::declval<Expressions>()...))>>
      : public std::true_type
  {
  };

  template <typename... Expressions>
  struct can_call_select_flags_with : public can_call_select_flags_with_impl<sqlpp::detail::type_vector<Expressions...>>
  {
  };

}  // namespace


int main()
{
  const auto maybe = true;

  // OK
  select_flags(sqlpp::all);
  select_flags(sqlpp::distinct);

  // Try to select no flags
  SQLPP_CHECK_STATIC_ASSERT(sqlpp::select_flags(), "at least one flag required in select_flags()");

  // select_flags requires select flags as arguments
  static_assert(can_call_select_flags_with<decltype(sqlpp::all)>::value, "");
  static_assert(not can_call_select_flags_with<decltype(sqlpp::union_all_t{})>::value, "");
  static_assert(not can_call_select_flags_with<decltype("all")>::value, "");
  static_assert(not can_call_select_flags_with<decltype(sqlpp::distinct, sqlpp::union_all_t{})>::value, "");
  static_assert(not can_call_select_flags_with<decltype(sqlpp::distinct, "all")>::value, "");

  // Try duplicate flags
  SQLPP_CHECK_STATIC_ASSERT(select_flags(sqlpp::all, sqlpp::all), "at least one duplicate argument detected in select_flags()");
  SQLPP_CHECK_STATIC_ASSERT(select_flags(sqlpp::all, sqlpp::distinct, sqlpp::all), "at least one duplicate argument detected in select_flags()");
  SQLPP_CHECK_STATIC_ASSERT(select_flags(dynamic(maybe, sqlpp::all), sqlpp::all), "at least one duplicate argument detected in select_flags()");
  SQLPP_CHECK_STATIC_ASSERT(select_flags(sqlpp::all, dynamic(maybe, sqlpp::all)), "at least one duplicate argument detected in select_flags()");
  SQLPP_CHECK_STATIC_ASSERT(select_flags(dynamic(maybe, sqlpp::all), dynamic(maybe, sqlpp::all)), "at least one duplicate argument detected in select_flags()");

  // Select flags are not strictly required.
  {
  auto s = sqlpp::statement_t<sqlpp::no_select_flag_list_t>{};
  using S = decltype(s);
  static_assert(std::is_same<sqlpp::statement_prepare_check_t<S>, sqlpp::consistent_t>::value, "");
  }
}


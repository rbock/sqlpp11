#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
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
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/no_data.h>
#include <sqlpp11/core/static_assert.h>
#include <sqlpp11/core/clause/select_flags.h>
#include <sqlpp11/core/type_traits.h>
#include <tuple>

namespace sqlpp
{
  template <typename... Flags>
  struct select_flag_list_t
  {
    select_flag_list_t(Flags... flags) : _flags(flags...)
    {
    }

    select_flag_list_t(const select_flag_list_t&) = default;
    select_flag_list_t(select_flag_list_t&&) = default;
    select_flag_list_t& operator=(const select_flag_list_t&) = default;
    select_flag_list_t& operator=(select_flag_list_t&&) = default;
    ~select_flag_list_t() = default;

    std::tuple<Flags...> _flags;
  };

  template <typename... Flags>
  struct is_clause<select_flag_list_t<Flags...>> : public std::true_type
  {
  };

  template <typename Statement, typename... Flags>
  struct consistency_check<Statement, select_flag_list_t<Flags...>>
  {
    using type = consistent_t;
  };

  struct no_select_flag_list_t
  {
    template <typename Statement,
              typename... Flags>
              requires(logic::all<is_select_flag<remove_dynamic_t<Flags>>::value...>::value)
    auto flags(this Statement&& statement, Flags... flags)
    {
      SQLPP_STATIC_ASSERT(sizeof...(Flags), "at least one flag required in select_flags()");
      SQLPP_STATIC_ASSERT(not detail::has_duplicates<remove_dynamic_t<Flags>...>::value,
                          "at least one duplicate argument detected in select_flags()");

      return new_statement<no_select_flag_list_t>(std::forward<Statement>(statement),
                                                  select_flag_list_t<Flags...>{flags...});
    }
  };

  template <typename Statement>
  struct consistency_check<Statement, no_select_flag_list_t>
  {
    using type = consistent_t;
  };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const no_select_flag_list_t&) -> std::string
  {
    return "";
  }

  template <typename Context, typename... Flags>
  auto to_sql_string(Context& context, const select_flag_list_t<Flags...>& t) -> std::string
  {
    return tuple_to_sql_string(context, t._flags, tuple_operand_no_dynamic{""});
  }

  template <typename... Flags>
              requires(logic::all<is_select_flag<remove_dynamic_t<Flags>>::value...>::value)
  auto select_flags(Flags... flags)
  {
    return statement_t<no_select_flag_list_t>().flags(std::move(flags)...);
  }

}  // namespace sqlpp

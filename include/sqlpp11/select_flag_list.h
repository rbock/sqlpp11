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

#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/type_traits.h>
#include <tuple>

namespace sqlpp
{
  // SELECTED FLAGS DATA
  template <typename... Flags>
  struct select_flag_list_data_t
  {
    select_flag_list_data_t(Flags... flgs) : _flags(flgs...)
    {
    }

    select_flag_list_data_t(const select_flag_list_data_t&) = default;
    select_flag_list_data_t(select_flag_list_data_t&&) = default;
    select_flag_list_data_t& operator=(const select_flag_list_data_t&) = default;
    select_flag_list_data_t& operator=(select_flag_list_data_t&&) = default;
    ~select_flag_list_data_t() = default;

    std::tuple<Flags...> _flags;
  };

  // SELECT FLAGS
  template <typename... Flags>
  struct select_flag_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_select_flag_list>;
    using _nodes = detail::type_vector<Flags...>;

    using _data_t = select_flag_list_data_t<Flags...>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      using _consistency_check = consistent_t;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_select_flags_are_flags_t, "arguments for flags() must be known select flags");
  template <typename... Flags>
  struct check_select_flags
  {
    using type = static_combined_check_t<
        static_check_t<logic::all_t<is_select_flag_t<Flags>::value...>::value, assert_select_flags_are_flags_t>>;
  };
  template <typename... Flags>
  using check_select_flags_t = typename check_select_flags<Flags...>::type;

  struct no_select_flag_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

    using _data_t = no_data_t;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t() = default;
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_select_flag_list_t, T>;

      using _consistency_check = consistent_t;

      template <typename... Flags>
      auto flags(Flags... flgs) const
          -> _new_statement_t<check_select_flags_t<Flags...>, select_flag_list_t<Flags...>>
      {
        return _flags_impl(check_select_flags_t<Flags...>{}, flgs...);
      }

    private:
      template <typename Check, typename... Flags>
      auto _flags_impl(Check, Flags... flgs) const -> inconsistent<Check>;

      template <typename... Flags>
      auto _flags_impl(consistent_t /*unused*/, Flags... flgs) const
          -> _new_statement_t<consistent_t, select_flag_list_t<Flags...>>
      {
        static_assert(not detail::has_duplicates<Flags...>::value,
                      "at least one duplicate argument detected in select flag list");

        return {static_cast<const derived_statement_t<Policies>&>(*this),
                select_flag_list_data_t<Flags...>{flgs...}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename... Flags>
  Context& serialize(Context& context, const select_flag_list_data_t<Flags...>& t)
  {
    interpret_tuple(t._flags, ' ', context);
    if (sizeof...(Flags) != 0u)
    {
      context << ' ';
    }
    return context;
  }

  template <typename T>
  auto select_flags(T&& t) -> decltype(statement_t<no_select_flag_list_t>().flags(std::forward<T>(t)))
  {
    return statement_t<no_select_flag_list_t>().flags(std::forward<T>(t));
  }

}  // namespace sqlpp

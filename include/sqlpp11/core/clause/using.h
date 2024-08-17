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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  // USING DATA
  template <typename... Tables>
  struct using_data_t
  {
    using_data_t(Tables... tables) : _tables(tables...)
    {
    }

    using_data_t(const using_data_t&) = default;
    using_data_t(using_data_t&&) = default;
    using_data_t& operator=(const using_data_t&) = default;
    using_data_t& operator=(using_data_t&&) = default;
    ~using_data_t() = default;

    std::tuple<Tables...> _tables;
  };

  // USING
  template <typename... Tables>
  struct using_t
  {
    using _traits = make_traits<no_value_t, tag::is_using_>;
    using _nodes = detail::type_vector<Tables...>;

    using _data_t = using_data_t<Tables...>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      // FIXME: Maybe check for unused tables, similar to from
      using _consistency_check = consistent_t;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_using_args_are_tables_t, "arguments for using() must be tables");
  template <typename... Tables>
  struct check_using
  {
    using type = static_combined_check_t<
        static_check_t<logic::all<is_table<Tables>::value...>::value, assert_using_args_are_tables_t>>;
  };
  template <typename... Tables>
  using check_using_t = typename check_using<Tables...>::type;

  // NO USING YET
  struct no_using_t
  {
    using _traits = make_traits<no_value_t, tag::is_where>;
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
      using _new_statement_t = new_statement_t<Check, Policies, no_using_t, T>;

      using _consistency_check = consistent_t;

      template <typename... Tables>
      auto using_(Tables... tables) const -> _new_statement_t<check_using_t<Tables...>, using_t<Tables...>>
      {
        static_assert(not detail::has_duplicates<Tables...>::value,
                      "at least one duplicate argument detected in using()");
        static_assert(sizeof...(Tables), "at least one table required in using()");

        return {_using_impl(check_using_t<Tables...>{}, tables...)};
      }

    private:
      template <typename Check, typename... Tables>
      auto _using_impl(Check, Tables... tables) const -> inconsistent<Check>;

      template <typename... Tables>
      auto _using_impl(consistent_t /*unused*/, Tables... tables) const
          -> _new_statement_t<consistent_t, using_t<Tables...>>
      {
        static_assert(not detail::has_duplicates<Tables...>::value,
                      "at least one duplicate argument detected in using()");

        return {static_cast<const derived_statement_t<Policies>&>(*this), using_data_t<Tables...>{tables...}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename... Tables>
  auto to_sql_string(Context& context, const using_data_t<Tables...>& t) -> std::string
  {
    return " USING " + tuple_to_sql_string(context, t._tables, tuple_operand{", "});
  }
}  // namespace sqlpp

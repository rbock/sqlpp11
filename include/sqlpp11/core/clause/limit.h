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
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  // LIMIT DATA
  template <typename Limit>
  struct limit_data_t
  {
    limit_data_t(Limit value) : _value(value)
    {
    }

    limit_data_t(const limit_data_t&) = default;
    limit_data_t(limit_data_t&&) = default;
    limit_data_t& operator=(const limit_data_t&) = default;
    limit_data_t& operator=(limit_data_t&&) = default;
    ~limit_data_t() = default;

    Limit _value;
  };

  // LIMIT
  template <typename Limit>
  struct limit_t
  {
    using _traits = make_traits<no_value_t, tag::is_limit>;
    using _nodes = detail::type_vector<Limit>;

    // Data
    using _data_t = limit_data_t<Limit>;

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

  SQLPP_PORTABLE_STATIC_ASSERT(assert_limit_is_unsigned_integral,
                               "argument for limit() must be an unsigned integral expressions");
  template <typename T>
  struct check_limit
  {
    using type =
        static_combined_check_t<static_check_t<is_unsigned_integral<T>::value, assert_limit_is_unsigned_integral>>;
  };
  template <typename T>
  using check_limit_t = typename check_limit<T>::type;

  struct no_limit_t
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
      using _new_statement_t = new_statement_t<Check, Policies, no_limit_t, T>;

      using _consistency_check = consistent_t;

      template <typename Arg>
      auto limit(Arg arg) const -> _new_statement_t<check_limit_t<Arg>, limit_t<Arg>>
      {
        return _limit_impl(check_limit_t<Arg>{}, std::move(arg));
      }

    private:
      template <typename Check, typename Arg>
      auto _limit_impl(Check, Arg arg) const -> inconsistent<Check>;

      template <typename Arg>
      auto _limit_impl(consistent_t /*unused*/, Arg arg) const -> _new_statement_t<consistent_t, limit_t<Arg>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this), limit_data_t<Arg>{std::move(arg)}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Limit>
  auto to_sql_string(Context& context, const limit_data_t<Limit>& t) -> std::string
  {
    context << " LIMIT ";
    operand_to_sql_string(context, t._value);
    return context;
  }

  template <typename T>
  auto limit(T&& t) -> decltype(statement_t<no_limit_t>().limit(std::forward<T>(t)))
  {
    return statement_t<no_limit_t>().limit(std::forward<T>(t));
  }

}  // namespace sqlpp

#pragma once

/*
 * Copyright (c) 2017, Serge Robyns
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
#include <sqlpp11/policy_update.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  // FOR_UPDATE DATA
  struct for_update_data_t
  {
  };

  // FOR_UPDATE
  struct for_update_t
  {
    using _traits = make_traits<no_value_t, tag::is_for_update>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = for_update_data_t;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = for_update_data_t;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : for_update{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> for_update;
      _impl_t<Policies>& operator()()
      {
        return for_update;
      }
      const _impl_t<Policies>& operator()() const
      {
        return for_update;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.for_update)
      {
        return t.for_update;
      }

      using _consistency_check = consistent_t;
    };
  };

  struct no_for_update_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = no_data_t;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : no_for_update{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> no_for_update;
      _impl_t<Policies>& operator()()
      {
        return no_for_update;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_for_update;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_for_update)
      {
        return t.no_for_update;
      }

      using _database_t = typename Policies::_database_t;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_for_update_t, T>;

      using _consistency_check = consistent_t;

      auto for_update() const -> _new_statement_t<consistent_t, for_update_t>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this), for_update_data_t{}};
      }
    };
  };

  // Interpreters
  template <typename Context>
  Context& serialize(const for_update_data_t&, Context& context)
  {
    context << " FOR UPDATE ";
    return context;
  }

  template <typename T>
  auto for_update(T&& t) -> decltype(statement_t<void, no_for_update_t>().for_update(std::forward<T>(t)))
  {
    return statement_t<void, no_for_update_t>().for_update(std::forward<T>(t));
  }
}  // namespace sqlpp

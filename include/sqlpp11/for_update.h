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

    using _data_t = for_update_data_t;

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

  struct no_for_update_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

    // Data
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
  Context& serialize(Context& context, const for_update_data_t&)
  {
    context << " FOR UPDATE ";
    return context;
  }

  auto for_update() -> decltype(statement_t<no_for_update_t>().for_update())
  {
    return statement_t<no_for_update_t>().for_update();
  }
}  // namespace sqlpp

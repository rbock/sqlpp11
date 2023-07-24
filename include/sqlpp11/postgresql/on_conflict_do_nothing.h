#pragma once

/**
 * Copyright © 2014-2019, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
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

#include <sqlpp11/detail/type_vector.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  SQLPP_VALUE_TRAIT_GENERATOR(is_on_conflict_do_nothing)

  namespace postgresql
  {
    // Forward declaration
    template <typename ConflictTarget>
    struct on_conflict_data_t;

    template <typename ConflictTarget>
    struct on_conflict_do_nothing_data_t
    {
      on_conflict_do_nothing_data_t(on_conflict_data_t<ConflictTarget> column) : _column(column)
      {
      }

      on_conflict_do_nothing_data_t(const on_conflict_do_nothing_data_t&) = default;
      on_conflict_do_nothing_data_t(on_conflict_do_nothing_data_t&&) = default;
      on_conflict_do_nothing_data_t& operator=(const on_conflict_do_nothing_data_t&) = default;
      on_conflict_do_nothing_data_t& operator=(on_conflict_do_nothing_data_t&&) = default;
      ~on_conflict_do_nothing_data_t() = default;

      on_conflict_data_t<ConflictTarget> _column;
    };

    template <typename ConflictTarget>
    struct on_conflict_do_nothing_t
    {
      using _traits = make_traits<no_value_t, tag::is_on_conflict_do_nothing>;
      using _nodes = sqlpp::detail::type_vector<ConflictTarget>;

      // Data
      using _data_t = on_conflict_do_nothing_data_t<ConflictTarget>;

      // Member implementation and methods
      template <typename Policies>
      struct _impl_t
      {
        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
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
        using _data_t = on_conflict_do_nothing_data_t<ConflictTarget>;
        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269

        template <typename... Args>
        _base_t(Args&&... args) : column{std::forward<Args>(args)...}
        {
        }

        _impl_t<Policies> column;
        _impl_t<Policies>& operator()()
        {
          return column;
        }
        const _impl_t<Policies>& operator()() const
        {
          return column;
        }

        template <typename T>
        static auto _get_member(T t) -> decltype(t.column)
        {
          return t.column;
        }

        // No consistency check needed, do nothing is just do nothing.
        using _consistency_check = consistent_t;
      };
    };

    template <typename ConflictTarget>
    postgresql::context_t& serialize(const postgresql::on_conflict_do_nothing_data_t<ConflictTarget>& o,
                                     postgresql::context_t& context)
    {
      serialize(o._column, context);
      context << "DO NOTHING";
      return context;
    }
  }  // namespace postgresql
}  // namespace sqlpp

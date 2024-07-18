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
#include <sqlpp11/policy_update.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  // OFFSET DATA
  template <typename Offset>
  struct offset_data_t
  {
    offset_data_t(Offset value) : _value(value)
    {
    }

    offset_data_t(const offset_data_t&) = default;
    offset_data_t(offset_data_t&&) = default;
    offset_data_t& operator=(const offset_data_t&) = default;
    offset_data_t& operator=(offset_data_t&&) = default;
    ~offset_data_t() = default;

    Offset _value;
  };

  // OFFSET
  template <typename Offset>
  struct offset_t
  {
    using _traits = make_traits<no_value_t, tag::is_offset>;
    using _nodes = detail::type_vector<Offset>;

    using _data_t = offset_data_t<Offset>;

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

  SQLPP_PORTABLE_STATIC_ASSERT(assert_offset_is_unsigned_integral,
                               "argument for offset() must be an integral expressions");
  template <typename T>
  struct check_offset
  {
    using type =
        static_combined_check_t<static_check_t<is_unsigned_integral<T>::value, assert_offset_is_unsigned_integral>>;
  };
  template <typename T>
  using check_offset_t = typename check_offset<T>::type;

  struct no_offset_t
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
      _base_t() = default;
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_offset_t, T>;

      using _consistency_check = consistent_t;

      template <typename Arg>
      auto offset(Arg arg) const -> _new_statement_t<check_offset_t<Arg>, offset_t<Arg>>
      {
        return _offset_impl(check_offset_t<Arg>{}, std::move(arg));
      }

    private:
      template <typename Check, typename Arg>
      auto _offset_impl(Check, Arg arg) const -> inconsistent<Check>;

      template <typename Arg>
      auto _offset_impl(consistent_t /*unused*/, Arg arg) const -> _new_statement_t<consistent_t, offset_t<Arg>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this), offset_data_t<Arg>{std::move(arg)}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Offset>
  Context& serialize(Context& context, const offset_data_t<Offset>& t)
  {
    context << " OFFSET ";
    serialize_operand(context, t._value);
    return context;
  }

  template <typename T>
  auto offset(T&& t) -> decltype(statement_t<no_offset_t>().offset(std::forward<T>(t)))
  {
    return statement_t<no_offset_t>().offset(std::forward<T>(t));
  }

}  // namespace sqlpp
